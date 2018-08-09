#include "StdAfx.h"
#include "HArrayFixHDD.h"

void HArrayFixHDD::sortItems(HArrayFixPair* pairs,
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

void HArrayFixHDD::getKeysAndValuesByRangeFromBlock(HArrayFixPair* pairs, 
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

	//read from hdd
	BlockCell blockCells[BLOCK_ENGINE_SIZE];
	readBlockCellsHDD(blockCells, blockOffset, BLOCK_ENGINE_SIZE);
	
	for(uint32 i = 0; i < BLOCK_ENGINE_SIZE; i++)
	{
		BlockCell& blockCell = blockCells[i];

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
			BranchCell branchCell1;

			readBranchCellHDD(branchCell1, blockCell.Offset);

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
			BranchCell branchCell1;

			readBranchCellHDD(branchCell1, blockCell.Offset);

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

			BranchCell branchCell2;

			readBranchCellHDD(branchCell2, blockCell.ValueOrOffset);

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

void HArrayFixHDD::getKeysAndValuesByRange(HArrayFixPair* pairs,
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
		ContentCell contentCells[MAX_ROW_LEN];
		readContentCellsHDD(contentCells, contentOffset, RowLen);
		
		uint32& contentCellValueOrOffset = *(uint32*)&contentCells[0];
		uchar8& contentCellType = contentCells[0].Type; //move to type part

		if(contentCellType >= ONLY_CONTENT_TYPE) //ONLY CONTENT =========================================================================================
		{
			uint32 i = 0;
			for(; keyOffset < KeyLen; keyOffset++, i++)
			{
				uint32 keyValue = *(uint32*)&contentCells[i];

				if(count < size)
				{
					pairs[count].Key[keyOffset] = keyValue;
				}
				
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
				//printf("===> ADD VALUE\n");
				ulong64 currValue = ((ulong64)(contentCells[i].Type - VALUE_MIN_TYPE) << 32) | *(uint32*)&contentCells[i];

				pairs[count++].Value = currValue;
				
				if(count < size)
				{
					pairs[count-1].copyTo(pairs[count], KeyLen); //copy prev key
				}

				printf("\rGet words from HDD: %u", count);

				/*if(pairs[count-1].Key[0] == 6906977)
					pairs[count-1].Key[0] = 6906977;*/
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
				ulong64 currValue = ((ulong64)(contentCellType - VALUE_MIN_TYPE) << 32) | contentCellValueOrOffset;

				//printf("===> ADD VALUE\n");
				pairs[count++].Value = currValue;

				if(count < size)
				{
					pairs[count-1].copyTo(pairs[count], KeyLen); //copy prev key
				}

				printf("\rGet words from HDD: %u", count);

				/*if(pairs[count-1].Key[0] == 6906977)
					pairs[count-1].Key[0] = 6906977;*/
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


uint32 HArrayFixHDD::getKeysAndValuesByRange(HArrayFixPair* pairs,
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

	HeaderCell headerCell;

	//start range
	if(startHeader < endHeader)
	{
		HeaderCell* pHeaderCells = new HeaderCell[endHeader - startHeader];
		uint32 currKey = 0;
		uint32 prevCount;

		readHeaderCellsHDD(pHeaderCells, startHeader, endHeader - startHeader);

		if(*(uint32*)&pHeaderCells[currKey])
		{
			prevCount = count;

			pActivePartition = &partitions[pHeaderCells[currKey].Partition];
			
			getKeysAndValuesByRange(pairs, count, size, 0, *(uint32*)&pHeaderCells[currKey], minKey, 0);

			sortItems(pairs, prevCount, count - 1);
		}

		currKey++;

		//middle range
		for(; currKey <= endHeader; currKey++)
		{
			if(count == size)
				return count;

			if(*(uint32*)&pHeaderCells[currKey])
			{
				prevCount = count;

				pActivePartition = &partitions[pHeaderCells[currKey].Partition];

				getKeysAndValuesByRange(pairs, count, size, 0, *(uint32*)&pHeaderCells[currKey], 0, 0);

				sortItems(pairs, prevCount, count - 1);
			}
		}

		//end range
		if(*(uint32*)&pHeaderCells[currKey])
		{
			prevCount = count;

			pActivePartition = &partitions[pHeaderCells[currKey].Partition];

			getKeysAndValuesByRange(pairs, count, size, 0, *(uint32*)&pHeaderCells[currKey], 0, maxKey);

			sortItems(pairs, prevCount, count - 1);
		}

		currKey++;

		delete[] pHeaderCells;
	}
	else
	{
		HeaderCell headerCell;

		uint32 contentOffset;

		readHeaderCellHDD(headerCell, startHeader);

		contentOffset = *(uint32*)&headerCell;
		pActivePartition = &partitions[headerCell.Partition];

		if(contentOffset)
		{
			getKeysAndValuesByRange(pairs, count, size, 0, contentOffset, minKey, maxKey);

			sortItems(pairs, 0, count - 1);
		}
	}
	
	return count;
}

uint32 HArrayFixHDD::getKeysAndValuesByPortions(HArrayFixPair* pairs,
											  uint32 size, 
											  uint32& startHeader,
											  bool& isBufferNotEnough)
{
	uint32 count = 0;
	uint32 prevCount = 0;

	uint32 endHeader = MAX_INT >> HeaderBits;

	//get portions
	const uint32 HEADER_PORTION = 1024;
	HeaderCell pHeaderCells[HEADER_PORTION];

	printf("\n");

	uint32 prevStartHeader = startHeader;
	
	for(uint32 currKey = 0; startHeader <= endHeader; startHeader++, currKey++)
	{
		if(!(currKey % HEADER_PORTION)) //each HEADER_PORTION load portion of header
		{
			readHeaderCellsHDD(pHeaderCells, startHeader, HEADER_PORTION);

			currKey = 0;
		}

		if(*(uint32*)&pHeaderCells[currKey])
		{
			prevCount = count;

			pActivePartition = &partitions[pHeaderCells[currKey].Partition];

			getKeysAndValuesByRange(pairs, count, size, 0, *(uint32*)&pHeaderCells[currKey], 0, 0);

			if(count == size)
			{
				count = prevCount; //do not fetch last header sub tree

				break;
			}

			sortItems(pairs, prevCount, count - 1);
		}
	}

	if(prevStartHeader == startHeader && startHeader <= endHeader)
	{
		isBufferNotEnough = true;
	}
	else
	{
		isBufferNotEnough = false;
	}

	printf("\n");

	return count;
}