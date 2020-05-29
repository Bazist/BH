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
#include "HArrayFixRAM.h"
#include "HArrayVisitor.h"

bool HArrayFixRAM::getKeysAndValuesByRangeFromBlock(HArrayVisitor* pVisitor,
													uint32* key,
													uint32 contentOffset,
													uint32 keyOffset,
													uint32 blockOffset,
													uint32* startKey,
													uint32* endKey)
{
	//printf("getValuesByRangeFromBlock count=%d size=%d contentOffset=%d keyOffset=%d blockOffset=%d\n", count, size, contentOffset, keyOffset, blockOffset);

	uint32 maxOffset = blockOffset + BLOCK_ENGINE_SIZE;

	for(uint32 offset = blockOffset; offset < maxOffset; offset++)
	{
		BlockPage* pBlockPage = pBlockPages[offset >> 16];
		BlockCell& blockCell = pBlockPage->pBlock[offset & 0xFFFF];

		uchar8& blockCellType = blockCell.Type;

		if(blockCellType == EMPTY_TYPE)
		{
			continue;
		}
		else if(blockCellType == CURRENT_VALUE_TYPE) //current value
		{
			uint32& keyValue = blockCell.ValueOrOffset;

			key[keyOffset] = keyValue;

			uint32* subMinKey = 0;
			uint32* subMaxKey = 0;
			
			if (startKey)
			{
				if(keyValue < startKey[keyOffset])
					continue;
				else if(keyValue == startKey[keyOffset])
					subMinKey = startKey;
			}

			if (endKey)
			{
				if(keyValue > endKey[keyOffset])
					continue;
				else if(keyValue == endKey[keyOffset])
					subMaxKey = endKey;
			}

			if(!getKeysAndValuesByRange(pVisitor, 
										key, 
										keyOffset + 1,  
										blockCell.Offset, 
										subMinKey, 
										subMaxKey))
			{
				return false;
			}
		}
		else if(blockCellType <= MAX_BRANCH_TYPE1) //branch cell
		{
			BranchPage* pBranchPage = pBranchPages[blockCell.Offset >> 16];
			BranchCell& branchCell1 = pBranchPage->pBranch[blockCell.Offset & 0xFFFF];

			//try find value in the list
			for(uint32 i=0; i<blockCellType; i++)
			{
				uint32& keyValue = branchCell1.Values[i];

				key[keyOffset] = keyValue;

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
			
				if (startKey)
				{
					if(keyValue < startKey[keyOffset])
						continue;
					else if(keyValue == startKey[keyOffset])
						subMinKey = endKey;
				}

				if (endKey)
				{
					if(keyValue > endKey[keyOffset])
						continue;
					else if(keyValue == endKey[keyOffset])
						subMaxKey = endKey;
				}

				if(!getKeysAndValuesByRange(pVisitor, 
											key, 
											keyOffset + 1, 
											branchCell1.Offsets[i], 
											subMinKey, 
											subMaxKey))
				{
					return false;
				}
			}
		}
		else if(blockCellType <= MAX_BRANCH_TYPE2) //branch cell
		{
			BranchPage* pBranchPage1 = pBranchPages[blockCell.Offset >> 16];
			BranchCell branchCell1 = pBranchPage1->pBranch[blockCell.Offset & 0xFFFF];

			//try find value in the list
			for(uint32 i=0; i < BRANCH_ENGINE_SIZE; i++)
			{
				uint32& keyValue = branchCell1.Values[i];

				key[keyOffset] = keyValue;

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
			
				if (startKey)
				{
					if(keyValue < startKey[keyOffset])
						continue;
					else if(keyValue == startKey[keyOffset])
						subMinKey = startKey;
				}

				if (endKey)
				{
					if(keyValue > endKey[keyOffset])
						continue;
					else if(keyValue == endKey[keyOffset])
						subMaxKey = endKey;
				}

				if(!getKeysAndValuesByRange(pVisitor, 
											key, 
											keyOffset + 1, 
											branchCell1.Offsets[i], 
											subMinKey, 
											subMaxKey))
				{
					return false;
				}
			}

			BranchPage* pBranchPage2 = pBranchPages[blockCell.ValueOrOffset >> 16];
			BranchCell branchCell2 = pBranchPage2->pBranch[blockCell.ValueOrOffset & 0xFFFF];

			//try find value in the list
			uint32 countValues = blockCellType - MAX_BRANCH_TYPE1;

			for(uint32 i=0; i<countValues; i++)
			{
				uint32& keyValue = branchCell2.Values[i];

				key[keyOffset] = keyValue;

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
			
				if (startKey)
				{
					if(keyValue < startKey[keyOffset])
						continue;
					else if(keyValue == startKey[keyOffset])
						subMinKey = startKey;
				}

				if (endKey)
				{
					if(keyValue > endKey[keyOffset])
						continue;
					else if(keyValue == endKey[keyOffset])
						subMaxKey = endKey;
				}

				if(!getKeysAndValuesByRange(pVisitor, 
											key, 
											keyOffset + 1, 
											branchCell2.Offsets[i], 
											subMinKey, 
											subMaxKey))
				{
					return false;
				}
			}
		}
		else if(blockCell.Type <= MAX_BLOCK_TYPE)
		{
			//go to block
			if(!getKeysAndValuesByRangeFromBlock(pVisitor,
												key,
												contentOffset,
												keyOffset,
												blockCell.Offset,
												startKey, 
												endKey))
			{
				return false;
			}
		}
	}

	return true;
}

bool HArrayFixRAM::getKeysAndValuesByRange(HArrayVisitor* pVisitor,
										   uint32* key,
										   uint32 keyOffset, 
										   uint32 contentOffset,
										   uint32* startKey,
										   uint32* endKey)
{
	//printf("getValuesByRange count=%d size=%d contentOffset=%d keyOffset=%d\n", count, size, contentOffset, keyOffset);

	for(; keyOffset <= KeyLen; keyOffset++, contentOffset++)
	{
		ContentPage* pContentPage = pContentPages[contentOffset>>16];
		ushort16 contentIndex = contentOffset&0xFFFF;
		
		uint32& contentCellValueOrOffset = *(uint32*)&pContentPage->pContent[contentIndex];
		uchar8& contentCellType = pContentPage->pContent[contentIndex].Type; //move to type part

		if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================================
		{
			for(; keyOffset < KeyLen; keyOffset++, contentOffset++)
			{
				uint32& keyValue = *(uint32*)&pContentPages[contentOffset>>16]->pContent[contentOffset&0xFFFF];
					
				key[keyOffset] = keyValue;
				
				if (startKey)
				{
					if(keyValue > startKey[keyOffset])
						startKey = 0;
					else if(keyValue < startKey[keyOffset])
						return true;
				}

				if (endKey)
				{
					if(keyValue < endKey[keyOffset])
						endKey = 0;
					else if(keyValue > endKey[keyOffset])
						return true;
				}
			}

			contentOffset += (KeyLen - keyOffset);

			ContentCell& valContentCell = pContentPages[contentOffset >> 16]->pContent[contentOffset & 0xFFFF];

			ulong64 currValue = ((ulong64)(valContentCell.Type - VALUE_MIN_TYPE) << 32) | *(uint32*)&valContentCell;

			if(pVisitor->onVisit(key, currValue))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if(contentCellType <= MAX_BRANCH_TYPE1) //BRANCH =====================================================================
		{
			BranchPage* pBranchPage = pBranchPages[contentCellValueOrOffset >> 16];
			BranchCell& branchCell = pBranchPage->pBranch[contentCellValueOrOffset & 0xFFFF];

			//check other
			for(uint32 i = 0; i<contentCellType; i++) //from 1
			{
				uint32& keyValue = branchCell.Values[i];

				key[keyOffset] = keyValue;

				uint32* subMinKey = 0;
				uint32* subMaxKey = 0;
				
				if (startKey)
				{
					if(keyValue < startKey[keyOffset])
						continue;
					else if(keyValue == startKey[keyOffset])
						subMinKey = startKey;
				}

				if (endKey)
				{
					if(keyValue > endKey[keyOffset])
						continue;
					else if(keyValue == endKey[keyOffset])
						subMaxKey = endKey;
				}

				if(!getKeysAndValuesByRange(pVisitor, 
											key, 
											keyOffset + 1, 
											branchCell.Offsets[i], 
											subMinKey, 
											subMaxKey))
				{
					return false;
				}
			}

			return true;
		}
		else if(contentCellType >= VALUE_MIN_TYPE)
		{
			ulong64 currValue = ((ulong64)(contentCellType - VALUE_MIN_TYPE) << 32) | contentCellValueOrOffset;

			if(pVisitor->onVisit(key, currValue))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if(contentCellType <= MAX_BLOCK_TYPE) //VALUE IN BLOCK ===================================================================
		{
			return getKeysAndValuesByRangeFromBlock(pVisitor, 
													key,
													contentOffset,
													keyOffset,
													contentCellValueOrOffset,
													startKey,
													endKey);
		}
		else if(contentCellType == CURRENT_VALUE_TYPE)
		{
			uint32& keyValue = contentCellValueOrOffset;
			
			key[keyOffset] = keyValue;

			if (startKey && keyValue < startKey[keyOffset])
				return true;

			if (endKey && keyValue > endKey[keyOffset])
				return true;
		}
	}

	return true;
}

void HArrayFixRAM::scanByVisitor(HArrayVisitor* pVisitor)
{
	pVisitor->onStartScan();

	uint32* key = new uint32[KeyLen];

	uint32 startHeader;
	uint32 endHeader;

	//get header range
	startHeader = 0;
	endHeader = 0xFFFFFFFF >> HeaderBits;
	
	//range
	for(uint32 currKey = startHeader + 1; currKey < endHeader; currKey++)
	{
		uint32 contentOffset = *(uint32*)&pHeader[currKey];
	
		if(contentOffset)
		{
			if(!getKeysAndValuesByRange(pVisitor, 
										key, 
										0, 
										contentOffset, 
										0, 
										0))
			{
				break;
			}
		}
	}

	delete[] key;

	pVisitor->onEndScan();
}