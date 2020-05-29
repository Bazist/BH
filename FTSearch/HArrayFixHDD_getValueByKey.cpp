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

ulong64 HArrayFixHDD::getValueByKey(uint32* key)
{
	HeaderCell headerCell;
	readHeaderCellHDD(headerCell, key[0]>>HeaderBits);

	uint32 contentOffset = *(uint32*)&headerCell;
	pActivePartition = &partitions[headerCell.Partition];
		
	if(contentOffset)
	{
		uint32 keyOffset = 0;
	
NEXT_KEY_PART:

		/*if(keyOffset > 2)
		{
			keyOffset = keyOffset;
		}*/

		ContentCell contentCell;
		readContentCellHDD(contentCell, contentOffset);

		uchar8& contentCellType = contentCell.Type; //move to type part
	
		if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================================
		{
			ContentCell contentCells[MAX_ROW_LEN]; //set size of content + value
			readContentCellsHDD(contentCells, contentOffset, (KeyLen - keyOffset + 1));

			uint32 i = 0;
			for(; keyOffset < KeyLen; i++, contentOffset++, keyOffset++)
			{
				if(*(uint32*)&contentCells[i] != key[keyOffset])
					return 0;
			}
			
			ulong64 currValue = ((ulong64)(contentCells[i].Type - VALUE_MIN_TYPE) << 32) | *(uint32*)&contentCells[i];

			return currValue; //return value
		}

		uint32& keyValue = key[keyOffset];
		uint32& contentCellValueOrOffset = *(uint32*)&contentCell;
	
		if(contentCellType <= MAX_BRANCH_TYPE1) //BRANCH =====================================================================
		{
			BranchCell branchCell;
			readBranchCellHDD(branchCell, contentCellValueOrOffset);

			//try find value in the list
			uint32* values = branchCell.Values;

			for(uint32 i=0; i<contentCellType; i++)
			{
				if(values[i] == keyValue)
				{
					contentOffset = branchCell.Offsets[i];
					keyOffset++;

					goto NEXT_KEY_PART;
				}
			}

			return 0;
		}
		else if(contentCellType >= VALUE_MIN_TYPE)
		{
			ulong64 currValue = ((ulong64)(contentCellType - VALUE_MIN_TYPE) << 32) | contentCellValueOrOffset;

			return currValue;
		}
		else if(contentCellType <= MAX_BLOCK_TYPE) //VALUE IN BLOCK ===================================================================
		{
			uchar8 idxKeyValue = (contentCellType - MIN_BLOCK_TYPE) * BLOCK_ENGINE_STEP;

			uint32 startOffset = contentCellValueOrOffset;

NEXT_BLOCK:
			uint32 subOffset = ((keyValue << idxKeyValue) >> BLOCK_ENGINE_SHIFT);
			uint32 blockOffset = startOffset + subOffset;
		
			BlockCell blockCell;
			readBlockCellHDD(blockCell, blockOffset);

			uchar8& blockCellType = blockCell.Type;

			if(blockCellType == EMPTY_TYPE)
			{
				return 0;
			}
			else if(blockCellType == CURRENT_VALUE_TYPE) //current value
			{
				if(blockCell.ValueOrOffset == keyValue) //value is exists
				{
					contentOffset = blockCell.Offset;
					keyOffset++;

					goto NEXT_KEY_PART;
				}
				else
				{
					return 0;
				}
			}
			else if(blockCellType <= MAX_BRANCH_TYPE1) //branch cell
			{
				BranchCell branchCell1;
				readBranchCellHDD(branchCell1, blockCell.Offset);

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

				return 0;
			}
			else if(blockCellType <= MAX_BRANCH_TYPE2) //branch cell
			{
				BranchCell branchCell1;
				readBranchCellHDD(branchCell1, blockCell.Offset);

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

				BranchCell branchCell2;
				readBranchCellHDD(branchCell2, blockCell.ValueOrOffset);

				//try find value in the list
				uint32 countValues = blockCellType - MAX_BRANCH_TYPE1;

				for(uint32 i=0; i<countValues; i++)
				{
					if(branchCell2.Values[i] == keyValue)
					{
						contentOffset = branchCell2.Offsets[i];
						keyOffset++;

						goto NEXT_KEY_PART;
					}
				}

				return 0;
			}
			else if(blockCell.Type <= MAX_BLOCK_TYPE)
			{
				//go to block
				idxKeyValue = (blockCell.Type - MIN_BLOCK_TYPE) * BLOCK_ENGINE_STEP;
				startOffset = blockCell.Offset;

				goto NEXT_BLOCK;
			}
			else
			{
				return 0;
			}
		}
		else if(contentCellType == CURRENT_VALUE_TYPE) //PART OF KEY =========================================================================
		{
			if(contentCellValueOrOffset == keyValue)
			{
				contentOffset++;
				keyOffset++;
			
				goto NEXT_KEY_PART;
			}
			else 
			{
				return 0;
			}
		}
	}

	return 0;
}