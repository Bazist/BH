#pragma once

#ifndef DOCUMENTS_ID_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTS_ID_HEADER 334	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "BlockMemoryPool.h"
#include "BinaryFile.h"

class DocumentsNameOffset
{
public:
	DocumentsNameOffset(BlockMemoryPool* pBlockMemoryPool)
	{
		this->pBlockMemoryPool = pBlockMemoryPool;

		pHeadBlockMemory = 0;
		pTailBlockMemory = 0;

		FilePosition = 0;
		CurrentSize = 0;
	}

	void clear()
	{
		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while(pBlockMemory)
		{
			pBlockMemoryPool->releaseObject(pBlockMemory);
			pBlockMemory = pBlockMemory->pNextBlockMemory;
		}

		pHeadBlockMemory = 0;
		pTailBlockMemory = 0;
		
		CurrentSize = 0;
	}
	
	//format {[2bits size of docNumber][6 bits amount of positions]}{docNumber}{position1}...{positionN}

	void addOffset(uint* offset, uint& filePosition)
	{
		if(!pHeadBlockMemory) //create first
		{
			pHeadBlockMemory = pBlockMemoryPool->newObject();
			pTailBlockMemory = pHeadBlockMemory;
		}

		for(uint i = 0; i < 4; i++)
		{
			pTailBlockMemory->pMemory[CurrentSize++] = offset[i];

			if(CurrentSize == BLOCK_SIZE)
			{
				BlockMemory* pNewMemoryBlock = pBlockMemoryPool->newObject();
				pTailBlockMemory->pNextBlockMemory = pNewMemoryBlock;
				pTailBlockMemory = pNewMemoryBlock;

				CurrentSize = 0;
			}

			filePosition++;
		}
	}

	void getDocumentOffsetByID(uint id, uint* offset)
	{
		//find start block
		uint blocks = id / BLOCK_SIZE;
		
		BlockMemory* pBlockMemory = pHeadBlockMemory;

		for(uint i=0; i<blocks; i++)
		{
			pBlockMemory = pBlockMemory->pNextBlockMemory;
		}

		//get string
		uint currPosition = id % BLOCK_SIZE;

		uint i = 0;
		while(i < 4)
		{
			offset[i] = pBlockMemory->pMemory[currPosition++];
			
			if(currPosition == BLOCK_SIZE)
			{
				pBlockMemory = pBlockMemory->pNextBlockMemory;
				currPosition = 0;
			}

			if(offset[i])
			{
				i++;
			}
			else
			{
				break;
			}
		}
	}

	void writeBlocksToFile(BinaryFile* pBinaryFile,
						   uchar* pBuffer, 
						   uint& maxLength,
						   uint& buffFilledLength)
	{
		uchar currPosition = 0;
		uint maxSafeLength = maxLength - 128;
		
		BlockMemory* pBlockMemory = pHeadBlockMemory;
		
		while(pBlockMemory)
		{
			if(pBlockMemory->pNextBlockMemory)
			{
				for(uchar i=currPosition; i<BLOCK_SIZE; i++)
				{
					pBuffer[buffFilledLength++] = pBlockMemory->pMemory[i];
				}
				
				pBlockMemory = pBlockMemory->pNextBlockMemory;
			}
			else
			{
				for(uchar i=currPosition; i<CurrentSize; i++)
				{
					pBuffer[buffFilledLength++] = pBlockMemory->pMemory[i];
				}

				break;
			}

			if(buffFilledLength >= maxSafeLength) //flush buffer
			{
				pBinaryFile->write(pBuffer, buffFilledLength);
				buffFilledLength = 0;
			}

			currPosition = 0;
		}

		pBinaryFile->write(pBuffer, buffFilledLength);
		buffFilledLength = 0;
	}

	BlockMemoryPool* pBlockMemoryPool;

	BlockMemory* pHeadBlockMemory;
	BlockMemory* pTailBlockMemory;
	uchar CurrentSize;

	uint FilePosition;
	
	~DocumentsNameOffset()
	{
		
	}
};