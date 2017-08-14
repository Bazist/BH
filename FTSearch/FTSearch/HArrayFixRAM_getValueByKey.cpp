#include "StdAfx.h"
#include "HArrayFixRAM.h"

ulong64 HArrayFixRAM::getValueByKey(uint32* key)
{
	uint32 contentOffset = *(uint32*)&pHeader[key[0]>>HeaderBits];
	
	if(contentOffset)
	{
		uint32 keyOffset = 0;
	
NEXT_KEY_PART:
		ContentPage* pContentPage = pContentPages[contentOffset>>16];
		ushort16 contentIndex = contentOffset&0xFFFF;

		uchar8& contentCellType = pContentPage->pContent[contentIndex].Type; //move to type part
	
		if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================================
		{
			if(contentIndex < MAX_SAFE_SHORT) //content in one page
			{
				for(; keyOffset < KeyLen; contentIndex++, keyOffset++)
				{
					if(*(uint32*)&pContentPage->pContent[contentIndex] != key[keyOffset])
						return 0;
				}

				ContentCell& valContentCell = pContentPage->pContent[contentIndex];

				ulong64 currValue = ((ulong64)(valContentCell.Type - VALUE_MIN_TYPE) << 32) | *(uint32*)&valContentCell;

				return currValue; //return value
			}
			else //content in two pages
			{
				for(; keyOffset < KeyLen; contentOffset++, keyOffset++)
				{
					if(*(uint32*)&pContentPages[contentOffset>>16]->pContent[contentOffset&0xFFFF] != key[keyOffset])
						return 0;
				}

				ContentCell& valContentCell = pContentPages[contentOffset >> 16]->pContent[contentOffset & 0xFFFF];

				ulong64 currValue = ((ulong64)(valContentCell.Type - VALUE_MIN_TYPE) << 32) | *(uint32*)&valContentCell;

				return currValue; //return value
			}
		}

		uint32& keyValue = key[keyOffset];
		uint32& contentCellValueOrOffset = *(uint32*)&pContentPage->pContent[contentIndex];
	
		if(contentCellType <= MAX_BRANCH_TYPE1) //BRANCH =====================================================================
		{
			BranchPage* pBranchPage = pBranchPages[contentCellValueOrOffset >> 16];
			BranchCell& branchCell = pBranchPage->pBranch[contentCellValueOrOffset & 0xFFFF];

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
		else if(contentCellType == VALUE_MIN_TYPE)
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
		
			BlockPage* pBlockPage = pBlockPages[blockOffset >> 16];
			BlockCell& blockCell = pBlockPage->pBlock[blockOffset & 0xFFFF];

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
				BranchPage* pBranchPage = pBranchPages[blockCell.Offset >> 16];
				BranchCell& branchCell1 = pBranchPage->pBranch[blockCell.Offset & 0xFFFF];

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
				BranchPage* pBranchPage1 = pBranchPages[blockCell.Offset >> 16];
				BranchCell branchCell1 = pBranchPage1->pBranch[blockCell.Offset & 0xFFFF];

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

				BranchPage* pBranchPage2 = pBranchPages[blockCell.ValueOrOffset >> 16];
				BranchCell branchCell2 = pBranchPage2->pBranch[blockCell.ValueOrOffset & 0xFFFF];

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