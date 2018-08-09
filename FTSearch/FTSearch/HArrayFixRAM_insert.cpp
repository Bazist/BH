#include "StdAfx.h"
#include "HArrayFixRAM.h"

bool HArrayFixRAM::insert(uint32* key, ulong64 value)
{
	/*if((9350397>>16) < ContentPagesCount && pContentPages[9350397>>16]->pContent[9350397&0xFFFF].Type == 130)
	{
		uint32 bla = 0;
	}*/

	//create new page =======================================================================================
	if(!pContentPages[(lastContentOffset + RowLen)>>16])
	{
		pContentPages[ContentPagesCount++] = new ContentPage();

		if(ContentPagesCount == ContentPagesSize)
		{
			reallocateContentPages();
		}
	}

	//insert value ==========================================================================================
	uint32 keyOffset = 0;
	uint32 contentOffset = 0;

	uint32 headerOffset = key[0]>>HeaderBits;
	contentOffset = *(uint32*)&pHeader[headerOffset];

	if(!contentOffset)
	{
		*(uint32*)&pHeader[headerOffset] = lastContentOffset;
		
		ContentPage* pContentPage = pContentPages[lastContentOffset>>16];
		uint32 contentIndex = lastContentOffset&0xFFFF;

		pContentPage->pContent[contentIndex].Type = (ONLY_CONTENT_TYPE + KeyLen);

		if(contentIndex < MAX_SAFE_SHORT) //content in one page
		{
			//fill key
			for(; keyOffset < KeyLen; contentIndex++, keyOffset++, lastContentOffset++)
			{
				*(uint32*)&pContentPage->pContent[contentIndex] = key[keyOffset];
			}

			pContentPage->pContent[contentIndex].Type = VALUE_MIN_TYPE + (value >> 32);
			*(uint32*)&pContentPage->pContent[contentIndex] = value & 0xFFFFFFFF;

			lastContentOffset++;
		}
		else
		{
			for(; keyOffset < KeyLen; lastContentOffset++, keyOffset++)
			{
				pContentPage = pContentPages[lastContentOffset>>16];
				*(uint32*)&pContentPage->pContent[lastContentOffset&0xFFFF] = key[keyOffset];
			}

			pContentPage = pContentPages[lastContentOffset>>16];
			
			pContentPage->pContent[lastContentOffset & 0xFFFF].Type = VALUE_MIN_TYPE + (value >> 32);
			*(uint32*)&pContentPage->pContent[lastContentOffset & 0xFFFF] = value & 0xFFFFFFFF;

			lastContentOffset++;
		}

		#ifndef _RELEASE
		tempValues[10]++;
		#endif

		return true;
	}
	
	#ifndef _RELEASE
	tempValues[11]++;
	#endif

	//TWO KEYS =============================================================================================
NEXT_KEY_PART:

	ContentPage* pContentPage = pContentPages[contentOffset>>16];
	uint32 contentIndex = contentOffset & 0xFFFF;

	uchar8& contentCellType = pContentPage->pContent[contentIndex].Type;

	if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================
	{
		#ifndef _RELEASE
		tempValues[13]++;
		#endif

		if(contentIndex < MAX_SAFE_SHORT) //content in one page
		{
			for(; keyOffset < KeyLen; contentOffset++, contentIndex++, keyOffset++)
			{
				if(*(uint32*)&pContentPage->pContent[contentIndex] != key[keyOffset])
				{
					//create branch
					pContentPage->pContent[contentIndex].Type = MIN_BRANCH_TYPE1 + 1;

					//get free branch cell
					BranchCell* pBranchCell;
					if(countFreeBranchCell)
					{
						uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
						pBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];

						pBranchCell->Values[0] = *(uint32*)&pContentPage->pContent[contentIndex];
						*(uint32*)&pContentPage->pContent[contentIndex] = branchOffset;
					}
					else
					{
						BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
						if(!pBranchPage)
						{
							pBranchPage = new BranchPage();
							pBranchPages[BranchPagesCount++] = pBranchPage;

							if(BranchPagesCount == BranchPagesSize)
							{
								reallocateBranchPages();
							}
						}

						pBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];

						pBranchCell->Values[0] = *(uint32*)&pContentPage->pContent[contentIndex];
						*(uint32*)&pContentPage->pContent[contentIndex] = lastBranchOffset++;
					}
			
					pBranchCell->Offsets[0] = contentOffset + 1;

					pBranchCell->Values[1] = key[keyOffset];
					pBranchCell->Offsets[1] = lastContentOffset;

					uchar8& type = pContentPage->pContent[contentIndex + 1].Type;
					
					if (VALUE_MIN_TYPE <= type && type <= VALUE_MAX_TYPE)
					{
						//keep value cell
					}
					else
					{
						type = (ONLY_CONTENT_TYPE + KeyLen - keyOffset - 1);
					}
					
					goto FILL_KEY;
				}
				else
				{
					pContentPage->pContent[contentIndex].Type = CURRENT_VALUE_TYPE; //reset to current value	
				}
			}

			pContentPage->pContent[contentIndex].Type = (value >> 32) + VALUE_MIN_TYPE;
			*(uint32*)&pContentPage->pContent[contentIndex] = value & 0xFFFFFFFF; //key is exists, update

			return false;
		}
		else  //content in two pages
		{
			for(; keyOffset < KeyLen; contentOffset++, keyOffset++)
			{
				pContentPage = pContentPages[contentOffset>>16];
				contentIndex = contentOffset & 0xFFFF;

				if(*(uint32*)&pContentPage->pContent[contentIndex] != key[keyOffset])
				{
					//create branch
					pContentPage->pContent[contentIndex].Type = MIN_BRANCH_TYPE1 + 1;
								
					//get free branch cell
					BranchCell* pBranchCell;
					if(countFreeBranchCell)
					{
						uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
						pBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];

						pBranchCell->Values[0] = *(uint32*)&pContentPage->pContent[contentIndex];
						*(uint32*)&pContentPage->pContent[contentIndex] = branchOffset;
					}
					else
					{
						BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
						if(!pBranchPage)
						{
							pBranchPage = new BranchPage();
							pBranchPages[BranchPagesCount++] = pBranchPage;

							if(BranchPagesCount == BranchPagesSize)
							{
								reallocateBranchPages();
							}
						}

						pBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];

						pBranchCell->Values[0] = *(uint32*)&pContentPage->pContent[contentIndex];
						*(uint32*)&pContentPage->pContent[contentIndex] = lastBranchOffset++;
					}
									
					pBranchCell->Offsets[0] = contentOffset + 1;

					pBranchCell->Values[1] = key[keyOffset];
					pBranchCell->Offsets[1] = lastContentOffset;

					//set rest of key
					contentOffset++;

					pContentPage = pContentPages[contentOffset>>16];
					contentIndex = contentOffset & 0xFFFF;
					
					uchar8& type = pContentPage->pContent[contentIndex].Type;

					if (VALUE_MIN_TYPE <= type && type <= VALUE_MAX_TYPE)
					{
						//keep value cell
					}
					else
					{
						type = (ONLY_CONTENT_TYPE + KeyLen - keyOffset - 1);
					}
					
					goto FILL_KEY;
				}
				else
				{
					pContentPage->pContent[contentIndex].Type = CURRENT_VALUE_TYPE; //reset to current value	
				}
			}

			pContentPage = pContentPages[contentOffset>>16];
			contentIndex = contentOffset & 0xFFFF;

			pContentPage->pContent[contentIndex].Type = (value >> 32) + VALUE_MIN_TYPE;
			*(uint32*)&pContentPage->pContent[contentIndex] = value & 0xFFFFFFFF; //key is exists, update
		}

		return false; 
	}

	uint32& keyValue = key[keyOffset];
	uint32& contentCellValueOrOffset = *(uint32*)&pContentPage->pContent[contentIndex];
	
	if(contentCellType <= MAX_BRANCH_TYPE1) //BRANCH =====================================================================
	{
		#ifndef _RELEASE
		tempValues[14]++;
		#endif

		BranchPage* pBranchPage = pBranchPages[contentCellValueOrOffset >> 16];
		BranchCell& branchCell = pBranchPage->pBranch[contentCellValueOrOffset & 0xFFFF];

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
			branchCell.Offsets[contentCellType] = lastContentOffset;

			contentCellType++;

			goto FILL_KEY;
		}
		else
		{
			//EXTRACT BRANCH AND CREATE BLOCK =========================================================
			uchar8 idxKeyValue = 0;
			uchar8 currContentCellType = MIN_BLOCK_TYPE; 

			if(countFreeBranchCell < MAX_SHORT)
			{
				freeBranchCells[countFreeBranchCell++] = contentCellValueOrOffset;
			}

			const ushort16 countCell = BRANCH_ENGINE_SIZE + 1;

EXTRACT_BRANCH:
			
			BlockCell blockCells[countCell];
			uchar8 indexes[countCell];

			for(uint32 i=0; i < MAX_BRANCH_TYPE1; i++)
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
			currBlockCell4.Offset = lastContentOffset;
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

			//allocate page
			uint32 maxLastBlockOffset = lastBlockOffset + BLOCK_ENGINE_SIZE*2;
			if(!pBlockPages[maxLastBlockOffset>>16])
			{
				pBlockPages[BlockPagesCount++] = new BlockPage();

				if(BlockPagesCount == BlockPagesSize)
				{
					reallocateBlockPages();
				}
			}

			//fill block
			contentCellType = currContentCellType;
			contentCellValueOrOffset = lastBlockOffset;

			for(uint32 i = 0; i < countCell; i++)
			{
				uchar8 idx = indexes[i];
				uint32 offset = lastBlockOffset + idx;

				BlockPage* pBlockPage = pBlockPages[offset >> 16];
				BlockCell& currBlockCell = pBlockPage->pBlock[offset & 0xFFFF];
				
				uchar8 count = mapIndexes[idx];
				if(count == 1) //one value in block cell
				{
					currBlockCell = blockCells[i];
				}
				else if(count <= BRANCH_ENGINE_SIZE) //create branch cell
				{
					if(currBlockCell.Type == 0) //create branch
					{
						//get free branch cell
						BranchCell* pCurrBranchCell;
						if(countFreeBranchCell)
						{
							uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
							pCurrBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];
							currBlockCell.Offset = branchOffset;
						}
						else
						{
							BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
							if(!pBranchPage)
							{
								pBranchPage = new BranchPage();
								pBranchPages[BranchPagesCount++] = pBranchPage;

								if(BranchPagesCount == BranchPagesSize)
								{
									reallocateBranchPages();
								}
							}

							pCurrBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];
							currBlockCell.Offset = lastBranchOffset++;
						}

						currBlockCell.Type = MIN_BRANCH_TYPE1;
						
						pCurrBranchCell->Values[0] = blockCells[i].ValueOrOffset;
						pCurrBranchCell->Offsets[0] = blockCells[i].Offset;
					}
					else //types 1..4
					{
						BranchPage* pBranchPage = pBranchPages[currBlockCell.Offset >> 16];
						BranchCell& currBranchCell = pBranchPage->pBranch[currBlockCell.Offset & 0xFFFF];

						currBranchCell.Values[currBlockCell.Type] = blockCells[i].ValueOrOffset;
						currBranchCell.Offsets[currBlockCell.Type] = blockCells[i].Offset;
						currBlockCell.Type++;
					}
				}
				else
				{
					printf("FAIL STATE.");
				}
			}

			lastBlockOffset += BLOCK_ENGINE_SIZE;

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
		
		BlockPage* pBlockPage = pBlockPages[blockOffset >> 16];
		if(!pBlockPage)
		{
			pBlockPage = new BlockPage();
			pBlockPages[BlockPagesCount++] = pBlockPage;

			if(BlockPagesCount == BlockPagesSize)
			{
				reallocateBlockPages();
			}
		}

		BlockCell& blockCell = pBlockPage->pBlock[blockOffset & 0xFFFF];

		uchar8& blockCellType = blockCell.Type;

		if(blockCellType == EMPTY_TYPE) //free cell, fill
		{
			blockCellType = CURRENT_VALUE_TYPE;
			blockCell.ValueOrOffset = keyValue;
			blockCell.Offset = lastContentOffset;
			
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
					blockCellType = MIN_BRANCH_TYPE1 + 1;
			
					//get free branch cell
					BranchCell* pBranchCell;
					if(countFreeBranchCell)
					{
						uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
						pBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];

						pBranchCell->Offsets[0] = blockCell.Offset;
						blockCell.Offset = branchOffset;
					}
					else
					{
						BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
						if(!pBranchPage)
						{
							pBranchPage = new BranchPage();
							pBranchPages[BranchPagesCount++] = pBranchPage;

							if(BranchPagesCount == BranchPagesSize)
							{
								reallocateBranchPages();
							}
						}

						pBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];

						pBranchCell->Offsets[0] = blockCell.Offset;
						blockCell.Offset = lastBranchOffset++;
					}
					
					pBranchCell->Values[0] = blockCell.ValueOrOffset;
					
					pBranchCell->Values[1] = keyValue;
					pBranchCell->Offsets[1] = lastContentOffset;

					goto FILL_KEY;
				}
			}
			else if(blockCellType <= MAX_BRANCH_TYPE1) //branch cell 1
			{
				BranchPage* pBranchPage1 = pBranchPages[blockCell.Offset >> 16];
				BranchCell& branchCell1 = pBranchPage1->pBranch[blockCell.Offset & 0xFFFF];
				
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
					branchCell1.Offsets[blockCellType] = lastContentOffset;

					blockCellType++;

					goto FILL_KEY;
				}

				//create second branch
				blockCellType = MIN_BRANCH_TYPE2;
			
				//get free branch cell
				BranchCell* pBranchCell;
				if(countFreeBranchCell)
				{
					uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
					pBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];

					blockCell.ValueOrOffset = branchOffset;
				}
				else
				{
					BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
					if(!pBranchPage)
					{
						pBranchPage = new BranchPage();
						pBranchPages[BranchPagesCount++] = pBranchPage;

						if(BranchPagesCount == BranchPagesSize)
						{
							reallocateBranchPages();
						}
					}

					pBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];

					blockCell.ValueOrOffset = lastBranchOffset++;
				}
					
				pBranchCell->Values[0] = keyValue;
				pBranchCell->Offsets[0] = lastContentOffset;
				
				goto FILL_KEY;
			}
			else if(blockCellType <= MAX_BRANCH_TYPE2) //branch cell 2
			{
				BranchPage* pBranchPage1 = pBranchPages[blockCell.Offset >> 16];
				BranchCell& branchCell1 = pBranchPage1->pBranch[blockCell.Offset & 0xFFFF];
				
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
				BranchPage* pBranchPage2 = pBranchPages[blockCell.ValueOrOffset >> 16];
				BranchCell& branchCell2 = pBranchPage2->pBranch[blockCell.ValueOrOffset & 0xFFFF];

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
					branchCell2.Offsets[countValues] = lastContentOffset;

					blockCellType++;

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

					if(countFreeBranchCell < MAX_SHORT)
					{
						freeBranchCells[countFreeBranchCell++] = blockCell.Offset;
					}

					if(countFreeBranchCell < MAX_SHORT)
					{
						freeBranchCells[countFreeBranchCell++] = blockCell.ValueOrOffset;
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
					currBlockCell8.Offset = lastContentOffset;
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

					//allocate page
					uint32 maxLastBlockOffset = lastBlockOffset + BLOCK_ENGINE_SIZE*2;
					if(!pBlockPages[maxLastBlockOffset>>16])
					{
						pBlockPages[BlockPagesCount++] = new BlockPage();

						if(BlockPagesCount == BlockPagesSize)
						{
							reallocateBlockPages();
						}
					}

					//fill block
					blockCellType = MIN_BLOCK_TYPE + (idxKeyValue / BLOCK_ENGINE_STEP);
					blockCell.Offset = lastBlockOffset;

					for(uint32 i = 0; i < countCell; i++)
					{
						uchar8 idx = indexes[i];
						uint32 offset = lastBlockOffset + idx;

						BlockPage* pBlockPage = pBlockPages[offset >> 16];
						BlockCell& currBlockCell = pBlockPage->pBlock[offset & 0xFFFF];

						uchar8 count = mapIndexes[idx];
						if(count == 1) //one value in block cell
						{
							currBlockCell = blockCells[i];
						}
						else
						{
							if(currBlockCell.Type == 0) //create branch
							{
								//get free branch cell
								BranchCell* pCurrBranchCell;
								if(countFreeBranchCell)
								{
									uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
									pCurrBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];

									currBlockCell.Offset = branchOffset;
								}
								else
								{
									BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
									if(!pBranchPage)
									{
										pBranchPage = new BranchPage();
										pBranchPages[BranchPagesCount++] = pBranchPage;

										if(BranchPagesCount == BranchPagesSize)
										{
											reallocateBranchPages();
										}
									}

									pCurrBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];

									currBlockCell.Offset = lastBranchOffset++;
								}
																	
								currBlockCell.Type = MIN_BRANCH_TYPE1;
								
								pCurrBranchCell->Values[0] = blockCells[i].ValueOrOffset;
								pCurrBranchCell->Offsets[0] = blockCells[i].Offset;
							}
							else if(currBlockCell.Type < MAX_BRANCH_TYPE1)
							{
								BranchPage* pBranchPage = pBranchPages[currBlockCell.Offset >> 16];
								BranchCell& currBranchCell = pBranchPage->pBranch[currBlockCell.Offset & 0xFFFF];

								currBranchCell.Values[currBlockCell.Type] = blockCells[i].ValueOrOffset;
								currBranchCell.Offsets[currBlockCell.Type] = blockCells[i].Offset;

								currBlockCell.Type++;
							}
							else if(currBlockCell.Type == MAX_BRANCH_TYPE1)
							{
								//get free branch cell
								BranchCell* pCurrBranchCell;
								if(countFreeBranchCell)
								{
									uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
									pCurrBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];

									currBlockCell.ValueOrOffset = branchOffset;
								}
								else
								{
									BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
									if(!pBranchPage)
									{
										pBranchPage = new BranchPage();
										pBranchPages[BranchPagesCount++] = pBranchPage;

										if(BranchPagesCount == BranchPagesSize)
										{
											reallocateBranchPages();
										}
									}

									pCurrBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];

									currBlockCell.ValueOrOffset = lastBranchOffset++;
								}

								currBlockCell.Type = MIN_BRANCH_TYPE2;
								
								pCurrBranchCell->Values[0] = blockCells[i].ValueOrOffset;
								pCurrBranchCell->Offsets[0] = blockCells[i].Offset;
							}
							else
							{
								uint32 countValues = currBlockCell.Type - MAX_BRANCH_TYPE1;

								BranchPage* pBranchPage = pBranchPages[currBlockCell.ValueOrOffset >> 16];
								BranchCell& currBranchCell = pBranchPage->pBranch[currBlockCell.ValueOrOffset & 0xFFFF];

								currBranchCell.Values[countValues] = blockCells[i].ValueOrOffset;
								currBranchCell.Offsets[countValues] = blockCells[i].Offset;
								currBlockCell.Type++;
							}
						}
					}

					lastBlockOffset += BLOCK_ENGINE_SIZE;

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
			contentCellType = MIN_BRANCH_TYPE1 + 1;
			
			//get free branch cell
			BranchCell* pBranchCell;
			if(countFreeBranchCell)
			{
				uint32 branchOffset = freeBranchCells[--countFreeBranchCell];
				pBranchCell = &pBranchPages[branchOffset >> 16]->pBranch[branchOffset & 0xFFFF];

				pBranchCell->Values[0] = contentCellValueOrOffset;
				contentCellValueOrOffset = branchOffset;
			}
			else
			{
				BranchPage* pBranchPage = pBranchPages[lastBranchOffset >> 16];
				if(!pBranchPage)
				{
					pBranchPage = new BranchPage();
					pBranchPages[BranchPagesCount++] = pBranchPage;

					if(BranchPagesCount == BranchPagesSize)
					{
						reallocateBranchPages();
					}
				}

				pBranchCell = &pBranchPage->pBranch[lastBranchOffset & 0xFFFF];

				pBranchCell->Values[0] = contentCellValueOrOffset;
				contentCellValueOrOffset = lastBranchOffset++;
			}

			pBranchCell->Offsets[0] = contentOffset + 1;

			pBranchCell->Values[1] = keyValue;
			pBranchCell->Offsets[1] = lastContentOffset;

			goto FILL_KEY;
		}
	}
	else if(contentCellType >= VALUE_MIN_TYPE) //update existing value
	{
		contentCellType = VALUE_MIN_TYPE + (value >> 32);
		*(uint32*)&pContentPage->pContent[contentIndex] = (value & 0xFFFFFFFF);
		
		return false;
	}

FILL_KEY:

	//fill key
	pContentPage = pContentPages[lastContentOffset>>16];
	contentIndex = lastContentOffset&0xFFFF;

	keyOffset++;

	pContentPage->pContent[contentIndex].Type = ONLY_CONTENT_TYPE + KeyLen - keyOffset;

	if(contentIndex < MAX_SAFE_SHORT) //content in one page
	{
		for(; keyOffset < KeyLen; contentIndex++, keyOffset++, lastContentOffset++)
		{
			*(uint32*)&pContentPage->pContent[contentIndex] = key[keyOffset];
		}

		pContentPage->pContent[contentIndex].Type = (value >> 32) + VALUE_MIN_TYPE;
		*(uint32*)&pContentPage->pContent[contentIndex] = value & 0xFFFFFFFF; //key is exists, update
		
		lastContentOffset++;
	}
	else //content in two pages
	{
		for(; keyOffset < KeyLen; lastContentOffset++, keyOffset++)
		{
			pContentPage = pContentPages[lastContentOffset>>16];
			contentIndex = lastContentOffset&0xFFFF;

			*(uint32*)&pContentPage->pContent[contentIndex] = key[keyOffset];
		}

		pContentPage = pContentPages[lastContentOffset>>16];
		
		pContentPage->pContent[lastContentOffset & 0xFFFF].Type = (value >> 32) + VALUE_MIN_TYPE;
		*(uint32*)&pContentPage->pContent[lastContentOffset & 0xFFFF] = value & 0xFFFFFFFF; //key is exists, update
		
		lastContentOffset++;
	}
	
	return true;
}