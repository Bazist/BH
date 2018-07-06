#include "StdAfx.h"
#include "HArrayFixRAM.h"

void HArrayFixRAM::sortItems(HArrayFixPair* pairs,
							 int startIndex,
							 int endIndex)
{
	int i = startIndex, j = endIndex;
    
	HArrayFixPair pivot = pairs[(startIndex + endIndex) / 2];
 
      /* partition */
    
	while (i <= j)
	{
		while (pairs[i].compareKeys(pivot, KeyLen) == -1)
			i++;
            
		while (pairs[j].compareKeys(pivot, KeyLen) == 1)
            j--;
            
		if (i <= j)
		{
			HArrayFixPair tmp = pairs[i];
            pairs[i] = pairs[j];
            pairs[j] = tmp;
            
			i++;
            j--;
        }
    };
 
      /* recursion */
      if (startIndex < j)
            sortItems(pairs, startIndex, j);
      
	  if (i < endIndex)
            sortItems(pairs, i, endIndex);

	/*for(uint32 i = startIndex; i <= endIndex; i++)
	{
		for(uint32 j = i + 1; j <= endIndex; j++)
		{
			if(pairs[i].compareKeys(pairs[j], KeyLen) == 1)
			{
				HArrayFixPair tempPair = pairs[i];
				pairs[i] = pairs[j];
				pairs[j] = tempPair;
			}
		}
	}*/

	/*if(1==2)
	{
		printf("========================\n");
		for(uint32 j=0; j < endIndex; j++)
		{
			printf("%u %u\n", pairs[j].Key[0], pairs[j].Key[1]);
		}
	}*/

	return;
}

void HArrayFixRAM::getKeysAndValuesByRangeFromBlock(HArrayFixPair* pairs, 
													uint32& count,
													uint32 size,
													uint32 contentOffset,
													uint32 keyOffset,
													uint32 blockOffset,
													uint32* minKey,
													uint32* maxKey)
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

			pairs[count].Key[keyOffset] = keyValue;

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

			getKeysAndValuesByRange(pairs, count, size, keyOffset + 1,  blockCell.Offset, subMinKey, subMaxKey);

			if(count == size && pairs)
				return;
		}
		else if(blockCellType <= MAX_BRANCH_TYPE1) //branch cell
		{
			BranchPage* pBranchPage = pBranchPages[blockCell.Offset >> 16];
			BranchCell& branchCell1 = pBranchPage->pBranch[blockCell.Offset & 0xFFFF];

			//try find value in the list
			for(uint32 i=0; i<blockCellType; i++)
			{
				uint32& keyValue = branchCell1.Values[i];

				pairs[count].Key[keyOffset] = keyValue;

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

				getKeysAndValuesByRange(pairs, count, size, keyOffset + 1, branchCell1.Offsets[i], subMinKey, subMaxKey);

				if(count == size && pairs)
					return;
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

				pairs[count].Key[keyOffset] = keyValue;

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

				getKeysAndValuesByRange(pairs, count, size, keyOffset + 1, branchCell1.Offsets[i], subMinKey, subMaxKey);

				if(count == size && pairs)
					return;
			}

			BranchPage* pBranchPage2 = pBranchPages[blockCell.ValueOrOffset >> 16];
			BranchCell branchCell2 = pBranchPage2->pBranch[blockCell.ValueOrOffset & 0xFFFF];

			//try find value in the list
			uint32 countValues = blockCellType - MAX_BRANCH_TYPE1;

			for(uint32 i=0; i<countValues; i++)
			{
				uint32& keyValue = branchCell2.Values[i];

				pairs[count].Key[keyOffset] = keyValue;

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

				getKeysAndValuesByRange(pairs, count, size, keyOffset + 1, branchCell2.Offsets[i], subMinKey, subMaxKey);

				if(count == size && pairs)
					return;
			}
		}
		else if(blockCell.Type <= MAX_BLOCK_TYPE)
		{
			//go to block
			getKeysAndValuesByRangeFromBlock(pairs, 
											 count,
											 size,
											 contentOffset,
											 keyOffset,
											 blockCell.Offset,
											 minKey, 
											 maxKey);

			if(count == size && pairs)
				return;
		}
	}
}

void HArrayFixRAM::getKeysAndValuesByRange(HArrayFixPair* pairs,
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
		if(count == size && pairs)
			return;

		ContentPage* pContentPage = pContentPages[contentOffset>>16];
		ushort16 contentIndex = contentOffset&0xFFFF;
		
		uint32& contentCellValueOrOffset = *(uint32*)&pContentPage->pContent[contentIndex];
		uchar8& contentCellType = pContentPage->pContent[contentIndex].Type; //move to type part

		if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================================
		{
			for(; keyOffset < KeyLen; keyOffset++, contentOffset++)
			{
				uint32& keyValue = *(uint32*)&pContentPages[contentOffset>>16]->pContent[contentOffset&0xFFFF];
					
				pairs[count].Key[keyOffset] = keyValue;
				
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

			contentOffset += (KeyLen - keyOffset);

			if(pairs)
			{
				ContentCell& valContentCell = pContentPages[contentOffset >> 16]->pContent[contentOffset & 0xFFFF];

				ulong64 currValue = ((ulong64)(valContentCell.Type - VALUE_MIN_TYPE) << 32) | *(uint32*)&valContentCell;

				//printf("===> ADD VALUE\n");
				pairs[count++].Value = currValue;
				
				if(count < size)
				{
					pairs[count-1].copyTo(pairs[count], KeyLen); //copy prev key
				}
			}
			else
			{
				count++;
			}

			return;
		}
		else if(contentCellType <= MAX_BRANCH_TYPE1) //BRANCH =====================================================================
		{
			BranchPage* pBranchPage = pBranchPages[contentCellValueOrOffset >> 16];
			BranchCell& branchCell = pBranchPage->pBranch[contentCellValueOrOffset & 0xFFFF];

			//check other
			for(uint32 i = 0; i<contentCellType; i++) //from 1
			{
				uint32& keyValue = branchCell.Values[i];

				pairs[count].Key[keyOffset] = keyValue;

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

				getKeysAndValuesByRange(pairs, count, size, keyOffset + 1, branchCell.Offsets[i], subMinKey, subMaxKey);

				if(count == size && pairs)
					return;
			}

			return;
		}
		else if(contentCellType >= VALUE_MIN_TYPE)
		{
			if(pairs)
			{
				//printf("===> ADD VALUE\n");
				ulong64 currValue = ((ulong64)(contentCellType - VALUE_MIN_TYPE) << 32) | contentCellValueOrOffset;

				pairs[count++].Value = currValue;

				if(count < size)
				{
					pairs[count-1].copyTo(pairs[count], KeyLen); //copy prev key
				}
			}
			else
			{
				count++;
			}

			return;
		}
		else if(contentCellType <= MAX_BLOCK_TYPE) //VALUE IN BLOCK ===================================================================
		{
			getKeysAndValuesByRangeFromBlock(pairs, 
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
			
			pairs[count].Key[keyOffset] = keyValue;

			if (minKey && keyValue < minKey[keyOffset])
				return;

			if (maxKey && keyValue > maxKey[keyOffset])
				return;
		}
	}
}


uint32 HArrayFixRAM::getKeysAndValuesByRange(	HArrayFixPair* pairs,
											uint32 size, 
											uint32* minKey, 
											uint32* maxKey)
{
	uint32 count = 0;

	uint32 startHeader;
	uint32 endHeader;

	if(minKey && maxKey)
	{
		startHeader = minKey[0] >> HeaderBits;
		endHeader = maxKey[0] >> HeaderBits;
	}
	else
	{
		startHeader = 0;
		endHeader = MAX_INT >> HeaderBits;
	}

	//start range
	uint32 prevCount = 0;

	if(startHeader < endHeader)
	{
		uint32 contentOffset = *(uint32*)&pHeader[startHeader];
		if(contentOffset)
		{
			prevCount = count;

			getKeysAndValuesByRange(pairs, count, size, 0, contentOffset, minKey, 0);

			sortItems(pairs, prevCount, count - 1);
		}

		//middle range
		for(uint32 currKey = startHeader + 1; currKey < endHeader; currKey++)
		{
			if(count == size)
				return count;

			contentOffset = *(uint32*)&pHeader[currKey];
		
			if(contentOffset)
			{
				prevCount = count;

				getKeysAndValuesByRange(pairs, count, size, 0, contentOffset, 0, 0);

				sortItems(pairs, prevCount, count - 1);
			}
		}

		//end range
		contentOffset = *(uint32*)&pHeader[endHeader];
		if(contentOffset)
		{
			prevCount = count;

			getKeysAndValuesByRange(pairs, count, size, 0, contentOffset, 0, maxKey);

			sortItems(pairs, prevCount, count - 1);
		}
	}
	else
	{
		uint32 contentOffset = *(uint32*)&pHeader[startHeader];
		if(contentOffset)
		{
			prevCount = count;

			getKeysAndValuesByRange(pairs, count, size, 0, contentOffset, minKey, maxKey);

			sortItems(pairs, prevCount, count - 1);
		}
	}
	
	return count;
}