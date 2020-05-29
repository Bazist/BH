/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.h"
#include "HArrayFixHDD.h"

void HArrayFixHDD::insert(uint32* key, ulong64 value)
{
	//insert value ==========================================================================================
	uint32 keyOffset = 0;
	uint32 contentOffset = 0;

	uint32 headerOffset = key[0]>>HeaderBits;
	
	HeaderCell headerCell;

	readHeaderCellHDD(headerCell, headerOffset);

	contentOffset = *(uint32*)&headerCell;
	pActivePartition = &partitions[headerCell.Partition];

	if(!contentOffset)
	{
		*(uint32*)&headerCell = pActivePartition->lastContentOffset;

		writeHeaderCellHDD(headerCell, headerOffset);
		
		ContentCell contentCells[MAX_ROW_LEN];

		contentCells[0].Type = (ONLY_CONTENT_TYPE + KeyLen);

		//fill key
		uint32 i=0;
		for(; keyOffset < KeyLen; i++, keyOffset++)
		{
			*(uint32*)&contentCells[i] = key[keyOffset];
		}
		contentCells[i].Type = VALUE_MIN_TYPE + (value >> 32);
		*(uint32*)&contentCells[i] = value & 0xFFFFFFFF;

		writeContentCellsHDD(contentCells, pActivePartition->lastContentOffset, RowLen);

		pActivePartition->lastContentOffset += RowLen;
		
		#ifndef _RELEASE
		tempValues[10]++;
		#endif

		return;
	}
	
	#ifndef _RELEASE
	tempValues[11]++;
	#endif

	//TWO KEYS =============================================================================================
NEXT_KEY_PART:

	if(contentOffset == 16)
	{
		contentOffset = contentOffset;
	}

		
	ContentCell contentCell;
	readContentCellHDD(contentCell, contentOffset);

	uchar8& contentCellType = contentCell.Type;

	if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================
	{
		#ifndef _RELEASE
		tempValues[13]++;
		#endif

		for(; keyOffset < KeyLen; contentOffset++, keyOffset++)
		{
			readContentCellHDD(contentCell, contentOffset);

			if(*(uint32*)&contentCell != key[keyOffset])
			{
				//get free branch cell
				uint32 branchOffset;
				BranchCell branchCell;

				if(pActivePartition->countFreeBranchCell)
				{
					branchOffset = pActivePartition->freeBranchCells[--pActivePartition->countFreeBranchCell];
				}
				else
				{
					branchOffset = pActivePartition->lastBranchOffset++;
				}
				
				//create branch
				//branch
				uint32 nextContentOffset = contentOffset + 1;

				branchCell.Values[0] = *(uint32*)&contentCell;
				branchCell.Offsets[0] = nextContentOffset;

				branchCell.Values[1] = key[keyOffset];
				branchCell.Offsets[1] = pActivePartition->lastContentOffset;

				writeBranchCellHDD(branchCell, branchOffset);

				//content
				contentCell.Type = MIN_BRANCH_TYPE1 + 1;
				*(uint32*)&contentCell = branchOffset;

				writeContentCellHDD(contentCell, contentOffset);

				//next content
				uint32 len = KeyLen - keyOffset - 1;

				if (len) //do not overwrite value type
				{
					readContentCellHDD(contentCell, nextContentOffset);

					contentCell.Type = (ONLY_CONTENT_TYPE + len);

					writeContentCellHDD(contentCell, nextContentOffset);
				}

				goto FILL_KEY;
			}
			else
			{
				contentCell.Type = CURRENT_VALUE_TYPE; //reset to current value	

				writeContentCellHDD(contentCell, contentOffset);
			}
		}

		readContentCellHDD(contentCell, contentOffset);
		
		contentCell.Type = VALUE_MIN_TYPE + (value >> 32);
		*(uint32*)&contentCell = value & 0xFFFFFFFF; //key is exists, update

		writeContentCellHDD(contentCell, contentOffset);

		return;
	}

	uint32& keyValue = key[keyOffset];
	uint32& contentCellValueOrOffset = *(uint32*)&contentCell;
	
	if(contentCellType <= MAX_BRANCH_TYPE1) //BRANCH =====================================================================
	{
		#ifndef _RELEASE
		tempValues[14]++;
		#endif

		BranchCell branchCell;
		readBranchCellHDD(branchCell, contentCellValueOrOffset);

		//try find value in the list
		for(uint32 i=0; i<contentCellType; i++)
		{
			if(branchCell.Values[i] == keyValue)
			{
				contentOffset = branchCell.Offsets[i];
				keyOffset++;
				
				goto NEXT_KEY_PART;
			}
		}

		//create value in branch
		if(contentCellType < MAX_BRANCH_TYPE1)
		{
			branchCell.Values[contentCellType] = keyValue;
			branchCell.Offsets[contentCellType] = pActivePartition->lastContentOffset;
			
			writeBranchCellHDD(branchCell, contentCellValueOrOffset);

			contentCellType++;
			
			writeContentCellHDD(contentCell, contentOffset);

			goto FILL_KEY;
		}
		else
		{
			//EXTRACT BRANCH AND CREATE BLOCK =========================================================
			uchar8 idxKeyValue = 0;
			uchar8 currContentCellType = MIN_BLOCK_TYPE; 

			if(pActivePartition->countFreeBranchCell < MAX_SHORT)
			{
				pActivePartition->freeBranchCells[pActivePartition->countFreeBranchCell++] = contentCellValueOrOffset;
			}

			const ushort16 countCell = BRANCH_ENGINE_SIZE + 1;

EXTRACT_BRANCH:
			BlockCell blockCells[countCell];
			uchar8 indexes[countCell];

			for(uint32 i=0; i < BRANCH_ENGINE_SIZE; i++)
			{
				BlockCell& currBlockCell = blockCells[i];

				currBlockCell.Type = CURRENT_VALUE_TYPE;
				currBlockCell.Offset = branchCell.Offsets[i];

				uint32& value = branchCell.Values[i];
				currBlockCell.ValueOrOffset = value;

				indexes[i] = ((value << idxKeyValue) >> BLOCK_ENGINE_SHIFT); //((uchar8*)&value)[idxKeyValue];
			}

			//add current value
			BlockCell& currBlockCell4 = blockCells[BRANCH_ENGINE_SIZE];
			currBlockCell4.Type = CURRENT_VALUE_TYPE;
			currBlockCell4.Offset = pActivePartition->lastContentOffset;
			currBlockCell4.ValueOrOffset = keyValue;

			indexes[BRANCH_ENGINE_SIZE] = ((keyValue << idxKeyValue) >> BLOCK_ENGINE_SHIFT);
			
			//clear map
			uchar8 mapIndexes[BLOCK_ENGINE_SIZE];
			for(uint32 i=0; i < countCell; i++)
			{
				mapIndexes[indexes[i]] = 0;
			}

			//fill map
			for(uint32 i=0; i < countCell; i++)
			{
				uchar8& countIndexes = mapIndexes[indexes[i]];
				countIndexes++;

				if(countIndexes > BRANCH_ENGINE_SIZE)
				{
					idxKeyValue += BLOCK_ENGINE_STEP;
					currContentCellType++;

					goto EXTRACT_BRANCH; //use another byte
				}
			}

			//fill block
			contentCellType = currContentCellType;
			contentCellValueOrOffset = pActivePartition->lastBlockOffset;

			writeContentCellHDD(contentCell, contentOffset);

			for(uint32 i = 0; i < countCell; i++)
			{
				uchar8 idx = indexes[i];
				uint32 blockOffset = pActivePartition->lastBlockOffset + idx;

				uchar8 count = mapIndexes[idx];

				if(count == 1) //one value in block cell
				{
					writeBlockCellHDD(blockCells[i], blockOffset);
				}
				else if(count <= BRANCH_ENGINE_SIZE) //create branch cell
				{
					BlockCell currBlockCell;
					readBlockCellHDD(currBlockCell, blockOffset);
				
					BranchCell currBranchCell;
						
					if(currBlockCell.Type == 0) //create branch
					{
						//get free branch cell
						uint32 branchOffset;

						if(pActivePartition->countFreeBranchCell)
						{
							branchOffset = pActivePartition->freeBranchCells[--pActivePartition->countFreeBranchCell];
						}
						else
						{
							branchOffset = pActivePartition->lastBranchOffset++;
						}

						currBlockCell.Offset = branchOffset;
						currBlockCell.Type = MIN_BRANCH_TYPE1;

						writeBlockCellHDD(currBlockCell, blockOffset);
						
						currBranchCell.Values[0] = blockCells[i].ValueOrOffset;
						currBranchCell.Offsets[0] = blockCells[i].Offset;

						writeBranchCellHDD(currBranchCell, branchOffset);
						
					}
					else //types 1..4
					{
						readBranchCellHDD(currBranchCell, currBlockCell.Offset);

						currBranchCell.Values[currBlockCell.Type] = blockCells[i].ValueOrOffset;
						currBranchCell.Offsets[currBlockCell.Type] = blockCells[i].Offset;

						writeBranchCellHDD(currBranchCell, currBlockCell.Offset);

						currBlockCell.Type++;

						writeBlockCellHDD(currBlockCell, blockOffset);
					}
				}
				else
				{
					printf("FAIL STATE.");
				}
			}

			pActivePartition->lastBlockOffset += BLOCK_ENGINE_SIZE;

			#ifndef _RELEASE
			tempValues[4]++;
			#endif

			goto FILL_KEY;
		}
	}
	else if(contentCellType <= MAX_BLOCK_TYPE) //VALUE IN BLOCK ===================================================================
	{
		#ifndef _RELEASE
		tempValues[15]++;
		#endif

		//uint32 level = 1;

		uchar8 idxKeyValue = (contentCellType - MIN_BLOCK_TYPE) * BLOCK_ENGINE_STEP;

		uint32 startOffset = contentCellValueOrOffset;

NEXT_BLOCK:
		uint32 subOffset = ((keyValue << idxKeyValue) >> BLOCK_ENGINE_SHIFT);
		uint32 blockOffset = startOffset + subOffset;
		
		BlockCell blockCell;
		readBlockCellHDD(blockCell, blockOffset);

		uchar8& blockCellType = blockCell.Type;

		if(blockCellType == EMPTY_TYPE) //free cell, fill
		{
			blockCellType = CURRENT_VALUE_TYPE;
			blockCell.ValueOrOffset = keyValue;
			blockCell.Offset = pActivePartition->lastContentOffset;

			writeBlockCellHDD(blockCell, blockOffset);
			
			goto FILL_KEY;
		}
		else //block cell 
		{
			if(blockCellType == CURRENT_VALUE_TYPE) //current value
			{
				if(blockCell.ValueOrOffset == keyValue) //value is exists
				{
					contentOffset = blockCell.Offset;
					keyOffset++;

					goto NEXT_KEY_PART;
				}
				else //create branch with two values
				{
					//get free branch cell
					uint32 branchOffset;

					if(pActivePartition->countFreeBranchCell)
					{
						branchOffset = pActivePartition->freeBranchCells[--pActivePartition->countFreeBranchCell];
					}
					else
					{
						branchOffset = pActivePartition->lastBranchOffset++;
					}

					BranchCell branchCell;

					branchCell.Values[0] = blockCell.ValueOrOffset;
					branchCell.Offsets[0] = blockCell.Offset;
					
					branchCell.Values[1] = keyValue;
					branchCell.Offsets[1] = pActivePartition->lastContentOffset;

					writeBranchCellHDD(branchCell, branchOffset);

					blockCellType = MIN_BRANCH_TYPE1 + 1;
					blockCell.Offset = branchOffset;

					writeBlockCellHDD(blockCell, blockOffset);
					
					goto FILL_KEY;
				}
			}
			else if(blockCellType <= MAX_BRANCH_TYPE1) //branch cell 1
			{
				BranchCell branchCell1;
				readBranchCellHDD(branchCell1, blockCell.Offset);

				//first branch
				//try find value in the list
				for(uint32 i=0; i<blockCellType; i++)
				{
					if(branchCell1.Values[i] == keyValue)
					{
						contentOffset = branchCell1.Offsets[i];
						keyOffset++;

						goto NEXT_KEY_PART;
					}
				}

				if(blockCellType < MAX_BRANCH_TYPE1)
				{
					branchCell1.Values[blockCellType] = keyValue;
					branchCell1.Offsets[blockCellType] = pActivePartition->lastContentOffset;
					
					writeBranchCellHDD(branchCell1, blockCell.Offset);

					blockCellType++;

					writeBlockCellHDD(blockCell, blockOffset);

					goto FILL_KEY;
				}

				//create second branch
				//get free branch cell
				uint32 branchOffset;
				if(pActivePartition->countFreeBranchCell)
				{
					branchOffset = pActivePartition->freeBranchCells[--pActivePartition->countFreeBranchCell];
				}
				else
				{
					branchOffset = pActivePartition->lastBranchOffset++;
				}
				
				BranchCell branchCell;

				branchCell.Values[0] = keyValue;
				branchCell.Offsets[0] = pActivePartition->lastContentOffset;
					
				writeBranchCellHDD(branchCell, branchOffset);

				blockCellType = MIN_BRANCH_TYPE2;
				blockCell.ValueOrOffset = branchOffset;

				writeBlockCellHDD(blockCell, blockOffset);
					
				goto FILL_KEY;
			}
			else if(blockCellType <= MAX_BRANCH_TYPE2) //branch cell 2
			{
				BranchCell branchCell1;
				readBranchCellHDD(branchCell1, blockCell.Offset);
				
				//first branch
				//try find value in the list
				for(uint32 i=0; i < BRANCH_ENGINE_SIZE; i++)
				{
					if(branchCell1.Values[i] == keyValue)
					{
						contentOffset = branchCell1.Offsets[i];
						keyOffset++;

						goto NEXT_KEY_PART;
					}
				}

				//second branch
				BranchCell branchCell2;
				readBranchCellHDD(branchCell2, blockCell.ValueOrOffset);

				//try find value in the list
				uint32 countValues = blockCellType - MAX_BRANCH_TYPE1;

				for(uchar8 i=0; i<countValues; i++)
				{
					if(branchCell2.Values[i] == keyValue)
					{
						contentOffset = branchCell2.Offsets[i];
						keyOffset++;
						
						goto NEXT_KEY_PART;
					}
				}

				//create value in branch
				if(blockCellType < MAX_BRANCH_TYPE2) //add to branch value
				{
					branchCell2.Values[countValues] = keyValue;
					branchCell2.Offsets[countValues] = pActivePartition->lastContentOffset;

					writeBranchCellHDD(branchCell2, blockCell.ValueOrOffset);

					blockCellType++;

					writeBlockCellHDD(blockCell, blockOffset);

					goto FILL_KEY;
				}
				else
				{
					//CREATE NEXT BLOCK ==========================================================
					#ifndef _RELEASE
					tempValues[9]++;

					/*if(level >= 3)
					{
						tempValues[21]++;
					}*/
					#endif

					const ushort16 branchesSize = BRANCH_ENGINE_SIZE * 2;
					const ushort16 countCell = branchesSize + 1;

					BlockCell blockCells[countCell];
					uchar8 indexes[countCell];

					if(pActivePartition->countFreeBranchCell < MAX_SHORT)
					{
						pActivePartition->freeBranchCells[pActivePartition->countFreeBranchCell++] = blockCell.Offset;
					}

					if(pActivePartition->countFreeBranchCell < MAX_SHORT)
					{
						pActivePartition->freeBranchCells[pActivePartition->countFreeBranchCell++] = blockCell.ValueOrOffset;
					}

EXTRACT_BRANCH2:
					idxKeyValue += BLOCK_ENGINE_STEP;
					if(idxKeyValue > BLOCK_ENGINE_SHIFT)
						idxKeyValue = 0;
					
					//idxKeyValue &= BLOCK_ENGINE_MASK;

					uchar8 j=0;

					//first branch
					for(uint32 i=0; i < BRANCH_ENGINE_SIZE; i++, j++)
					{
						BlockCell& currBlockCell = blockCells[j];
						currBlockCell.Type = CURRENT_VALUE_TYPE;
						currBlockCell.Offset = branchCell1.Offsets[i];

						uint32& value = branchCell1.Values[i];
						currBlockCell.ValueOrOffset = value;

						indexes[j] = ((value << idxKeyValue) >> BLOCK_ENGINE_SHIFT);
					}

					//second branch
					for(uint32 i=0; i < BRANCH_ENGINE_SIZE; i++, j++)
					{
						BlockCell& currBlockCell = blockCells[j];
						currBlockCell.Type = CURRENT_VALUE_TYPE;
						currBlockCell.Offset = branchCell2.Offsets[i];

						uint32& value = branchCell2.Values[i];
						currBlockCell.ValueOrOffset = value;

						indexes[j] = ((value << idxKeyValue) >> BLOCK_ENGINE_SHIFT);
					}

					//add current value
					BlockCell& currBlockCell8 = blockCells[branchesSize];
					currBlockCell8.Type = CURRENT_VALUE_TYPE;
					currBlockCell8.Offset = pActivePartition->lastContentOffset;
					currBlockCell8.ValueOrOffset = keyValue;

					indexes[branchesSize] = ((keyValue << idxKeyValue) >> BLOCK_ENGINE_SHIFT);
			
					//clear map
					uchar8 mapIndexes[BLOCK_ENGINE_SIZE];
					for(uchar8 i=0; i < countCell; i++)
					{
						mapIndexes[indexes[i]] = 0;
					}

					//fill map
					for(uint32 i=0; i < countCell; i++)
					{
						uchar8& countIndexes = mapIndexes[indexes[i]];
						countIndexes++;

						if(countIndexes > branchesSize)
						{
							goto EXTRACT_BRANCH2; //use another byte
						}
					}

					//fill block
					blockCellType = MIN_BLOCK_TYPE + (idxKeyValue / BLOCK_ENGINE_STEP);
					blockCell.Offset = pActivePartition->lastBlockOffset;

					writeBlockCellHDD(blockCell, blockOffset);

					for(uint32 i = 0; i < countCell; i++)
					{
						uchar8 idx = indexes[i];
						uint32 currBlockOffset = pActivePartition->lastBlockOffset + idx;

						uchar8 count = mapIndexes[idx];
						if(count == 1) //one value in block cell
						{
							writeBlockCellHDD(blockCells[i], currBlockOffset);
						}
						else
						{
							BlockCell currBlockCell;
							readBlockCellHDD(currBlockCell, currBlockOffset);

							if(currBlockCell.Type == 0) //create branch
							{
								//get free branch cell
								uint32 branchOffset;

								if(pActivePartition->countFreeBranchCell)
								{
									branchOffset = pActivePartition->freeBranchCells[--pActivePartition->countFreeBranchCell];
								}
								else
								{
									branchOffset = pActivePartition->lastBranchOffset++;
								}
																	
								BranchCell branchCell;
								
								branchCell.Values[0] = blockCells[i].ValueOrOffset;
								branchCell.Offsets[0] = blockCells[i].Offset;

								writeBranchCellHDD(branchCell, branchOffset);

								currBlockCell.Type = MIN_BRANCH_TYPE1;
								currBlockCell.Offset = branchOffset;

								writeBlockCellHDD(currBlockCell, currBlockOffset);
							}
							else if(currBlockCell.Type < MAX_BRANCH_TYPE1)
							{
								BranchCell branchCell;

								readBranchCellHDD(branchCell, currBlockCell.Offset);

								branchCell.Values[currBlockCell.Type] = blockCells[i].ValueOrOffset;
								branchCell.Offsets[currBlockCell.Type] = blockCells[i].Offset;

								writeBranchCellHDD(branchCell, currBlockCell.Offset);

								currBlockCell.Type++;

								writeBlockCellHDD(currBlockCell, currBlockOffset);
							}
							else if(currBlockCell.Type == MAX_BRANCH_TYPE1)
							{
								//get free branch cell
								uint32 branchOffset;

								if(pActivePartition->countFreeBranchCell)
								{
									branchOffset = pActivePartition->freeBranchCells[--pActivePartition->countFreeBranchCell];
								}
								else
								{
									branchOffset = pActivePartition->lastBranchOffset++;
								}
																
								BranchCell branchCell;

								branchCell.Values[0] = blockCells[i].ValueOrOffset;
								branchCell.Offsets[0] = blockCells[i].Offset;

								writeBranchCellHDD(branchCell, branchOffset);

								currBlockCell.Type = MIN_BRANCH_TYPE2;
								currBlockCell.ValueOrOffset = branchOffset;

								writeBlockCellHDD(currBlockCell, currBlockOffset);
							}
							else
							{
								BranchCell branchCell;

								readBranchCellHDD(branchCell, currBlockCell.ValueOrOffset);

								uint32 countValues = currBlockCell.Type - MAX_BRANCH_TYPE1;
								branchCell.Values[countValues] = blockCells[i].ValueOrOffset;
								branchCell.Offsets[countValues] = blockCells[i].Offset;
								
								writeBranchCellHDD(branchCell, currBlockCell.ValueOrOffset);

								currBlockCell.Type++;

								writeBlockCellHDD(currBlockCell, currBlockOffset);
							}
						}
					}

					pActivePartition->lastBlockOffset += BLOCK_ENGINE_SIZE;

					#ifndef _RELEASE
					tempValues[4]++;
					#endif

					goto FILL_KEY;
				}
			}
			else if(blockCell.Type <= MAX_BLOCK_TYPE)
			{
				//go to block
				idxKeyValue = (blockCell.Type - MIN_BLOCK_TYPE) * BLOCK_ENGINE_STEP;
				startOffset = blockCell.Offset;

				//level++;

				goto NEXT_BLOCK;
			}
		}
	}
	else if(contentCellType == CURRENT_VALUE_TYPE) //PART OF KEY =========================================================================
	{
		#ifndef _RELEASE
		tempValues[12]++;
		#endif

		if(contentCellValueOrOffset == keyValue)
		{
			contentOffset++;
			keyOffset++;

			goto NEXT_KEY_PART;
		}
		else //create branch
		{
			//get free branch cell
			uint32 branchOffset;

			if(pActivePartition->countFreeBranchCell)
			{
				branchOffset = pActivePartition->freeBranchCells[--pActivePartition->countFreeBranchCell];
			}
			else
			{
				branchOffset = pActivePartition->lastBranchOffset++;
			}

			BranchCell branchCell;
			
			branchCell.Values[0] = contentCellValueOrOffset;
			branchCell.Offsets[0] = contentOffset + 1;

			branchCell.Values[1] = keyValue;
			branchCell.Offsets[1] = pActivePartition->lastContentOffset;

			writeBranchCellHDD(branchCell, branchOffset);

			contentCellType = MIN_BRANCH_TYPE1 + 1;
			contentCellValueOrOffset = branchOffset;

			writeContentCellHDD(contentCell, contentOffset);

			goto FILL_KEY;
		}
	}
	else if(contentCellType >= VALUE_MIN_TYPE) //update existing value
	{
		contentCell.Type = VALUE_MIN_TYPE + (value >> 32);
		*(uint32*)&contentCell = value & 0xFFFFFFFF;

		writeContentCellHDD(contentCell, contentOffset);

		return;
	}

FILL_KEY:

	//fill key
	keyOffset++;

	ContentCell contentCells[MAX_ROW_LEN];

	contentCells[0].Type = ONLY_CONTENT_TYPE + KeyLen - keyOffset;

	uint32 i = 0;

	for(; keyOffset < KeyLen; i++, keyOffset++)
	{
		*(uint32*)&contentCells[i] = key[keyOffset];
	}

	contentCells[i].Type = VALUE_MIN_TYPE + (value >> 32);
	*(uint32*)&contentCells[i] = value & 0xFFFFFFFF;

	writeContentCellsHDD(contentCells, pActivePartition->lastContentOffset, (i + 1));

	pActivePartition->lastContentOffset += (i + 1);

	return;
}