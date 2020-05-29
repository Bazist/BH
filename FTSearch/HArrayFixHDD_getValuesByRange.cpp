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

void HArrayFixHDD::getValuesByRangeFromBlock(ulong64* values,
										uint32& count,
										uint32 size,
										uint32 contentOffset,
										uint32 keyOffset,
										uint32 blockOffset,
										uint32* minKey,
										uint32* maxKey)
{
	////printf("getValuesByRangeFromBlock count=%d size=%d contentOffset=%d keyOffset=%d blockOffset=%d\n", count, size, contentOffset, keyOffset, blockOffset);

	uint32 maxOffset = blockOffset + BLOCK_ENGINE_SIZE;
	for(uint32 offset = blockOffset; offset < maxOffset; offset++)
	{
		if(count == size && values)
			return;

		BlockCell blockCell;

		readBlockCellHDD(blockCell, offset);

		uchar8& blockCellType = blockCell.Type;

		if(blockCellType == EMPTY_TYPE)
		{
			continue;
		}
		else if(blockCellType == CURRENT_VALUE_TYPE) //current value
		{
			uint32& keyValue = blockCell.ValueOrOffset;

			uint32* subMinKey = 0;
			uint32* subMaxKey = 0;
			
			if (minKey)
			{
				if(keyValue < minKey[keyOffset])
					continue;
				else if(keyValue == minKey[keyOffset])
					subMinKey = minKey;
			}

			if (maxKey)
			{
				if(keyValue > maxKey[keyOffset])
					continue;
				else if(keyValue == maxKey[keyOffset])
					subMaxKey = maxKey;
			}

			getValuesByRange(values, count, size, keyOffset + 1,  blockCell.Offset, subMinKey, subMaxKey);
		}
		else if(blockCellType <= MAX_BRANCH_TYPE1) //branch cell
		{
			BranchCell branchCell1;

			readBranchCellHDD(branchCell1, blockCell.Offset);

			//try find value in the list
			for(uint32 i=0; i<blockCellType; i++)
			{
				uint32& keyValue = branchCell1.Values[i];

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
			
				if (minKey)
				{
					if(keyValue < minKey[keyOffset])
						continue;
					else if(keyValue == minKey[keyOffset])
						subMinKey = minKey;
				}

				if (maxKey)
				{
					if(keyValue > maxKey[keyOffset])
						continue;
					else if(keyValue == maxKey[keyOffset])
						subMaxKey = maxKey;
				}

				getValuesByRange(values, count, size, keyOffset + 1, branchCell1.Offsets[i], subMinKey, subMaxKey);
			}
		}
		else if(blockCellType <= MAX_BRANCH_TYPE2) //branch cell
		{
			BranchCell branchCell1;

			readBranchCellHDD(branchCell1, blockCell.Offset);

			//try find value in the list
			for(uint32 i=0; i < BRANCH_ENGINE_SIZE; i++)
			{
				uint32& keyValue = branchCell1.Values[i];

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
			
				if (minKey)
				{
					if(keyValue < minKey[keyOffset])
						continue;
					else if(keyValue == minKey[keyOffset])
						subMinKey = minKey;
				}

				if (maxKey)
				{
					if(keyValue > maxKey[keyOffset])
						continue;
					else if(keyValue == maxKey[keyOffset])
						subMaxKey = maxKey;
				}

				getValuesByRange(values, count, size, keyOffset + 1, branchCell1.Offsets[i], subMinKey, subMaxKey);
			}

			BranchCell branchCell2;

			readBranchCellHDD(branchCell2, blockCell.ValueOrOffset);

			//try find value in the list
			uint32 countValues = blockCellType - MAX_BRANCH_TYPE1;

			for(uint32 i=0; i<countValues; i++)
			{
				uint32& keyValue = branchCell2.Values[i];

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
			
				if (minKey)
				{
					if(keyValue < minKey[keyOffset])
						continue;
					else if(keyValue == minKey[keyOffset])
						subMinKey = minKey;
				}

				if (maxKey)
				{
					if(keyValue > maxKey[keyOffset])
						continue;
					else if(keyValue == maxKey[keyOffset])
						subMaxKey = maxKey;
				}

				getValuesByRange(values, count, size, keyOffset + 1, branchCell2.Offsets[i], subMinKey, subMaxKey);
			}
		}
		else if(blockCell.Type <= MAX_BLOCK_TYPE)
		{
			//go to block
			getValuesByRangeFromBlock(values, 
									  count,
									  size,
									  contentOffset,
									  keyOffset,
									  blockCell.Offset,
									  minKey, 
									  maxKey);
		}
	}
}

void HArrayFixHDD::getValuesByRange(ulong64* values,
							uint32& count,
							uint32 size,
							uint32 keyOffset, 
							uint32 contentOffset,
							uint32* minKey,
							uint32* maxKey)
{
	//printf("getValuesByRange count=%d size=%d contentOffset=%d keyOffset=%d\n", count, size, contentOffset, keyOffset);

	for(; keyOffset <= KeyLen; keyOffset++, contentOffset++)
	{
		if(count == size && values)
			return;

		ContentCell contentCell;
		readContentCellHDD(contentCell, contentOffset);
		
		uint32& contentCellValueOrOffset = *(uint32*)&contentCell;
		uchar8& contentCellType = contentCell.Type; //move to type part

		if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================================
		{
			if(minKey || maxKey)
			{
				for(; keyOffset < KeyLen; keyOffset++, contentOffset++)
				{
					readContentCellHDD(contentCell, contentOffset);

					uint32& keyValue = *(uint32*)&contentCell;
				
					if (minKey)
					{
						if(keyValue > minKey[keyOffset])
							minKey = 0;
						else if(keyValue < minKey[keyOffset])
							return;
					}

					if (maxKey)
					{
						if(keyValue < maxKey[keyOffset])
							maxKey = 0;
						else if(keyValue > maxKey[keyOffset])
							return;
					}
				}
			}

			contentOffset += (KeyLen - keyOffset);

			if(values)
			{
				//printf("===> ADD VALUE\n");

				readContentCellHDD(contentCell, contentOffset);

				ulong64 currValue = ((ulong64)(contentCell.Type - VALUE_MIN_TYPE) << 32) | *(uint32*)&contentCell;

				values[count++] = currValue;
			}
			else
			{
				count++;
			}

			return;
		}
		else if(contentCellType <= MAX_BRANCH_TYPE1) //BRANCH =====================================================================
		{
			BranchCell branchCell;
			
			readBranchCellHDD(branchCell, contentCellValueOrOffset);

			//check other
			for(uint32 i = 0; i<contentCellType; i++) //from 1
			{
				uint32& keyValue = branchCell.Values[i];

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
				
				if (minKey)
				{
					if(keyValue < minKey[keyOffset])
						continue;
					else if(keyValue == minKey[keyOffset])
						subMinKey = minKey;
				}

				if (maxKey)
				{
					if(keyValue > maxKey[keyOffset])
						continue;
					else if(keyValue == maxKey[keyOffset])
						subMaxKey = maxKey;
				}

				getValuesByRange(values, count, size, keyOffset + 1, branchCell.Offsets[i], subMinKey, subMaxKey);
			}

			return;
		}
		else if(contentCellType >= VALUE_MIN_TYPE)
		{
			if(values)
			{
				ulong64 currValue = ((ulong64)(contentCellType - VALUE_MIN_TYPE) << 32) | contentCellValueOrOffset;

				//printf("===> ADD VALUE\n");
				values[count++] = currValue;
			}
			else
			{
				count++;
			}

			return;
		}
		else if(contentCellType <= MAX_BLOCK_TYPE) //VALUE IN BLOCK ===================================================================
		{
			getValuesByRangeFromBlock(values, 
									  count,
									  size,
									  contentOffset,
									  keyOffset,
									  contentCellValueOrOffset,
									  minKey,
									  maxKey);

			return;
		}
		else if(contentCellType == CURRENT_VALUE_TYPE)
		{
			uint32& keyValue = contentCellValueOrOffset;

			if (minKey && keyValue < minKey[keyOffset])
				return;

			if (maxKey && keyValue > maxKey[keyOffset])
				return;
		}
	}
}


uint32 HArrayFixHDD::getValuesByRange(ulong64* values,
								uint32 size, 
								uint32* minKey, 
								uint32* maxKey)
{
	uint32 count = 0;
	uint32 startHeader = minKey[0] >> HeaderBits;
	uint32 endHeader = maxKey[0] >> HeaderBits;

	HeaderCell headerCell;

	//start range
	if(startHeader < endHeader)
	{
		uint32 contentOffset;

		readHeaderCellHDD(headerCell, startHeader);

		contentOffset = *(uint32*)&headerCell;
		pActivePartition = &partitions[headerCell.Partition];

		if(contentOffset)
		{
			getValuesByRange(values, count, size, 0, contentOffset, minKey, 0);
		}

		//middle range
		for(uint32 currKey = startHeader + 1; currKey < endHeader; currKey++)
		{
			if(count == size)
				return count;

			readHeaderCellHDD(headerCell, currKey);
		
			contentOffset = *(uint32*)&headerCell;
			pActivePartition = &partitions[headerCell.Partition];

			if(contentOffset)
			{
				getValuesByRange(values, count, size, 0, contentOffset, 0, 0);
			}
		}

		//end range
		readHeaderCellHDD(headerCell, endHeader);

		contentOffset = *(uint32*)&headerCell;
		pActivePartition = &partitions[headerCell.Partition];

		if(contentOffset)
		{
			getValuesByRange(values, count, size, 0, contentOffset, 0, maxKey);
		}
	}
	else
	{
		uint32 contentOffset;

		readHeaderCellHDD(headerCell, startHeader);

		contentOffset = *(uint32*)&headerCell;
		pActivePartition = &partitions[headerCell.Partition];

		if(contentOffset)
		{
			getValuesByRange(values, count, size, 0, contentOffset, minKey, maxKey);
		}
	}
	
	return count;
}