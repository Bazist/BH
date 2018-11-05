#pragma once

#ifndef DOCUMENTS_BLOCK_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTS_BLOCK_HEADER 3	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "BlockMemoryPool.h"
#include "BinaryFile.h"
#include "PostSelector.h"
#include "PostSelectorPool.h"

class DocumentsBlock
{
public:
	DocumentsBlock()
	{
		pHeadBlockMemory = 0;
		pTailBlockMemory = 0;

		CountPosition = 0;
		CurrentSize = 0;

		PrevDocNumber = 0;
		PrevPosNumber = 0;

		//FilePosition = 0;
		//ImportFilePosition = 0;

		CountDocuments = 0;
	}

	inline void incrementCurrentSize()
	{
		CurrentSize++;

		if (CurrentSize == BLOCK_SIZE)
		{
			BlockMemory* pNewMemoryBlock = pBlockMemoryPool->newObject();
			pTailBlockMemory->pNextBlockMemory = pNewMemoryBlock;
			pTailBlockMemory = pNewMemoryBlock;

			CurrentSize = 0;
		}
	}

	void clear()
	{
		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (pBlockMemory)
		{
			BlockMemory* pNextBlockMemory = pBlockMemory->pNextBlockMemory;
			pBlockMemoryPool->releaseObject(pBlockMemory);
			pBlockMemory = pNextBlockMemory;
		}

		pHeadBlockMemory = 0;
		pTailBlockMemory = 0;

		CountPosition = 0;
		CurrentSize = 0;

		PrevDocNumber = 0;
		PrevPosNumber = 0;

		CountDocuments = 0;
	}

	//format {[2bits size of docNumber][6 bits amount of docNumbers with increment 1 or if header is 2 docNumber]}{docNumber}
	void addWord(uint32 docNumber)
	{
		if (PrevDocNumber == docNumber)
		{
			return;
		}

		CountDocuments++;

		if (PrevDocNumber
			&& PrevDocNumber + 1 == docNumber
			&& CountPosition < 63)
		{
			uint32 header = *pCurrentHeader;
			if ((header >> 6) == 2)
			{
				pTailBlockMemory->pMemory[CurrentSize] = (header & 0x3F);
				incrementCurrentSize();

				*pCurrentHeader = 2; //header: 0, countpositon: 2
				CountPosition = 2;
			}
			else
			{
				(*pCurrentHeader)++; //next
				CountPosition++;
			}

			PrevDocNumber = docNumber;
		}
		else
		{
			if (!PrevDocNumber) //set count position in the previous header
			{
				pHeadBlockMemory = pBlockMemoryPool->newObject();
				pTailBlockMemory = pHeadBlockMemory;
			}

			if (CurrentSize < SAFE_BLOCK_SIZE)
			{
				pCurrentHeader = &pTailBlockMemory->pMemory[CurrentSize++];

				uint32 deltaDocNumber = docNumber - PrevDocNumber;

				if (deltaDocNumber < 64)
				{
					*pCurrentHeader = (2 << 6) | deltaDocNumber;
				}
				else if (deltaDocNumber < MAX_CHAR)
				{
					*pCurrentHeader = 1;

					pTailBlockMemory->pMemory[CurrentSize++] = deltaDocNumber;
				}
				else if (deltaDocNumber < MAX_SHORT)
				{
					*pCurrentHeader = 65; //(1<<6) + 1;

					pTailBlockMemory->pMemory[CurrentSize++] = (deltaDocNumber >> 8);
					pTailBlockMemory->pMemory[CurrentSize++] = (deltaDocNumber & 0xFF);
				}
				else
				{
					*pCurrentHeader = 193; //(3<<6) + 1;

					pTailBlockMemory->pMemory[CurrentSize++] = (deltaDocNumber >> 24);
					pTailBlockMemory->pMemory[CurrentSize++] = (deltaDocNumber << 8 >> 24);
					pTailBlockMemory->pMemory[CurrentSize++] = (deltaDocNumber << 16 >> 24); //((uchar8*)&deltaDocNumber)[2];
					pTailBlockMemory->pMemory[CurrentSize++] = (deltaDocNumber & 0xFF);
				}

				CountPosition = 1;
				PrevDocNumber = docNumber;

				return;
			}
			else
			{
				pCurrentHeader = &pTailBlockMemory->pMemory[CurrentSize];
				incrementCurrentSize();

				uint32 deltaDocNumber = docNumber - PrevDocNumber;
				if (deltaDocNumber < 64)
				{
					*pCurrentHeader = (2 << 6) | deltaDocNumber;
				}
				else if (deltaDocNumber < MAX_CHAR)
				{
					*pCurrentHeader = 1;

					pTailBlockMemory->pMemory[CurrentSize] = deltaDocNumber;
					incrementCurrentSize();
				}
				else if (deltaDocNumber < MAX_SHORT)
				{
					*pCurrentHeader = 65; //(1<<6) + 1;

					pTailBlockMemory->pMemory[CurrentSize] = (deltaDocNumber >> 8);
					incrementCurrentSize();

					pTailBlockMemory->pMemory[CurrentSize] = (deltaDocNumber & 0xFF);
					incrementCurrentSize();
				}
				else
				{
					*pCurrentHeader = 193; //(3<<6) + 1;

					pTailBlockMemory->pMemory[CurrentSize] = (deltaDocNumber >> 24);
					incrementCurrentSize();

					pTailBlockMemory->pMemory[CurrentSize] = (deltaDocNumber << 8 >> 24);
					incrementCurrentSize();

					pTailBlockMemory->pMemory[CurrentSize] = (deltaDocNumber << 16 >> 24); //((uchar8*)&deltaDocNumber)[2];
					incrementCurrentSize();

					pTailBlockMemory->pMemory[CurrentSize] = (deltaDocNumber & 0xFF);
					incrementCurrentSize();
				}

				CountPosition = 1;
				PrevDocNumber = docNumber;

				return;
			}
		}
	}

	//void addByte(uchar8 byte)
	//{
	//	if(!pHeadBlockMemory) //create first
	//	{
	//		pHeadBlockMemory = pBlockMemoryPool->newObject();
	//		pTailBlockMemory = pHeadBlockMemory;
	//	}

	//	pTailBlockMemory->pMemory[CurrentSize] = byte;
	//	incrementCurrentSize();
	//}

	void writeBlocksToFile(BinaryFile* pBinaryFile,
		uchar8* pBuffer,
		uint32 maxLength,
		uint32& buffFilledLength,
		uint32 baseDocNumber,
		uint32 lastDocNumber)
	{
		uchar8 currPosition = 0;
		uint32 maxSafeLength = maxLength - 128;

		//repack header
		BlockMemory* pBlockMemory = pHeadBlockMemory;
		if (lastDocNumber)
		{
			uchar8 header = pBlockMemory->pMemory[0];

			//read old header
			uint32 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			uint32 deltaDocNumber = 0;

			if (leftHeaderPart == 2)
			{
				currPosition = 1;

				deltaDocNumber = baseDocNumber + rightHeaderPart - lastDocNumber;

				rightHeaderPart = 1; //reset, because is coded one number
			}
			else
			{
				currPosition = 1;

				uint32 sizeNumber = leftHeaderPart + 2;

				uint32 firstDocNumber = 0;
				while (currPosition < sizeNumber)
				{
					firstDocNumber = (firstDocNumber << 8) | pBlockMemory->pMemory[currPosition++];
				}

				deltaDocNumber = baseDocNumber + firstDocNumber - lastDocNumber;
			}

			//save new header
			if (deltaDocNumber < 64)
			{
				pBuffer[buffFilledLength++] = (2 << 6) + deltaDocNumber;
			}
			else if (deltaDocNumber < MAX_CHAR)
			{
				pBuffer[buffFilledLength++] = rightHeaderPart;
				pBuffer[buffFilledLength++] = deltaDocNumber;
			}
			else if (deltaDocNumber < MAX_SHORT)
			{
				pBuffer[buffFilledLength++] = (1 << 6) | rightHeaderPart;
				pBuffer[buffFilledLength++] = (deltaDocNumber >> 8);
				pBuffer[buffFilledLength++] = (deltaDocNumber & 0xFF);
			}
			else
			{
				pBuffer[buffFilledLength++] = (3 << 6) | rightHeaderPart;
				pBuffer[buffFilledLength++] = (deltaDocNumber >> 24);
				pBuffer[buffFilledLength++] = (deltaDocNumber << 8 >> 24);
				pBuffer[buffFilledLength++] = (deltaDocNumber << 16 >> 24);
				pBuffer[buffFilledLength++] = (deltaDocNumber & 0xFF);
			}
		}

		while (true)
		{
			//flush buffer
			if (buffFilledLength >= maxSafeLength)
			{
				pBinaryFile->write(pBuffer, buffFilledLength);
				buffFilledLength = 0;
			}

			//save to buffer
			if (pBlockMemory->pNextBlockMemory)
			{
				for (uchar8 i = currPosition; i < BLOCK_SIZE; i++)
				{
					pBuffer[buffFilledLength++] = pBlockMemory->pMemory[i];
				}

				pBlockMemory = pBlockMemory->pNextBlockMemory;
			}
			else
			{
				for (uchar8 i = currPosition; i < CurrentSize; i++)
				{
					pBuffer[buffFilledLength++] = pBlockMemory->pMemory[i];
				}

				break;
			}

			currPosition = 0;
		}

		//pBinaryFile->write(pBuffer, length);
	}

	void writeBlocksToBuffer(uchar8* pBuffer, uint32 bufferSize)
	{
		uint32 buffFilledLength = 0;
		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (buffFilledLength < bufferSize)
		{
			if (pBlockMemory->pNextBlockMemory)
			{
				for (uchar8 i = 0; i < BLOCK_SIZE; i++)
				{
					pBuffer[buffFilledLength++] = pBlockMemory->pMemory[i];
				}

				pBlockMemory = pBlockMemory->pNextBlockMemory;
			}
			else
			{
				for (uchar8 i = 0; i < CurrentSize; i++)
				{
					pBuffer[buffFilledLength++] = pBlockMemory->pMemory[i];
				}

				break;
			}
		}

		pBuffer[buffFilledLength++] = 0; //null terminated
	}

	void readBlocksFromFile(BinaryFile* pSourceDocFile,
		uchar8* pSourceBuffer,

		ulong64& sourceFilePosition,
		uint32& sourceBuffPosition,
		uint32& sourceBuffLength,

		uint32 maxSizeBuffer,
		uint32 minLastNameID,
		uint32 maxLastNameID,

		bool& isFormatCorrupted)
	{
		bool isFirstIteration = true;

		uint32 docNumber = 0;

		while (true)
		{
			//1. Read data from file to buffer
			if (sourceBuffLength == 0 || sourceBuffPosition + 128 >= maxSizeBuffer)
			{
				//read new data from file
				sourceFilePosition += sourceBuffPosition;
				sourceBuffLength = pSourceDocFile->read(pSourceBuffer, sourceFilePosition, maxSizeBuffer);

				sourceBuffPosition = 0;

				if (!sourceBuffLength)
				{
					return;
				}
			}

			//read document number
			uchar8 header = pSourceBuffer[sourceBuffPosition++];
			uint32 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			uint32 deltaDocNumber = 0;

			if (leftHeaderPart == 2)
			{
				docNumber += rightHeaderPart;

				//catch errors
				if (docNumber < minLastNameID || docNumber > maxLastNameID)
				{
					isFormatCorrupted = true;

					return;
				}

				addWord(docNumber);
			}
			else
			{
				uint32 sizeNumber = sourceBuffPosition + leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				while (sourceBuffPosition < sizeNumber)
				{
					uchar8 byte = pSourceBuffer[sourceBuffPosition++];
					deltaDocNumber = (deltaDocNumber << 8) | byte;
				}

				docNumber += deltaDocNumber;

				//add next
				for (uint32 i = 0; i < rightHeaderPart; i++, docNumber++)
				{
					//catch errors
					if (docNumber > maxLastNameID)
					{
						isFormatCorrupted = true;

						return;
					}

					addWord(docNumber);
				}

				docNumber--;
			}

			if (!pSourceBuffer[sourceBuffPosition]) //is last
			{
				sourceBuffPosition++; //with null terminated
				return;
			}
		}

		return;
	}

	inline void markOneDocuments(uint32* pResultDocNumbers,
		uint32& countResultDocNumbers,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					pResultDocNumbers[countResultDocNumbers++] = prevDocNumber;
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						pResultDocNumbers[countResultDocNumbers++] = prevDocNumber;
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void makeZoom(DocumentsBlock* pZoom,
		uchar8* pLevelBuffer,
		uchar8 level,
		uint32 step,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currStep = 0;

		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				//mark documents
				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					if (!pLevelBuffer || pLevelBuffer[prevDocNumber] == level)
					{
						if (currStep == step)
						{
							pZoom->addWord(prevDocNumber);

							currStep = 0;
						}
						else
						{
							currStep++;
						}
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						if (!pLevelBuffer || pLevelBuffer[prevDocNumber] == level)
						{
							if (currStep == step)
							{
								pZoom->addWord(prevDocNumber);

								currStep = 0;
							}
							else
							{
								currStep++;
							}
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void markFirstDocuments(uchar8* pMergeBuffer,
		uint32* pUsedDocNumbers,
		uint32& countUsedDocNumbers,
		uint32& minDocNumber,
		uint32& maxDocNumber,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					//mark documents
					pMergeBuffer[prevDocNumber] = 1;
					pUsedDocNumbers[countUsedDocNumbers++] = prevDocNumber;

					if (!minDocNumber)
					{
						minDocNumber = prevDocNumber;
					}

					maxDocNumber = prevDocNumber;
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						pMergeBuffer[prevDocNumber] = 1;
						pUsedDocNumbers[countUsedDocNumbers++] = prevDocNumber;

						if (!minDocNumber)
						{
							minDocNumber = prevDocNumber;
						}

						maxDocNumber = prevDocNumber;
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void markNextDocuments(uchar8* pMergeBuffer,
		uint32& minDocNumber,
		uint32& maxDocNumber,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					//mark documents
					if (prevDocNumber >= minDocNumber && pMergeBuffer[prevDocNumber])
					{
						pMergeBuffer[prevDocNumber]++;
					}

					if (prevDocNumber > maxDocNumber) //exit
					{
						return;
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						//mark documents
						if (prevDocNumber >= minDocNumber && pMergeBuffer[prevDocNumber])
						{
							pMergeBuffer[prevDocNumber]++;
						}

						if (prevDocNumber > maxDocNumber) //exit
						{
							return;
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void markLastDocuments(uchar8* pMergeBuffer,
		uint32* pResultDocNumbers,
		uint32& countResultDocNumbers,
		uint32& minDocNumber,
		uint32& maxDocNumber,
		uint32 countWords,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					//mark documents
					if (prevDocNumber >= minDocNumber && pMergeBuffer[prevDocNumber])
					{
						pMergeBuffer[prevDocNumber]++;

						if (pMergeBuffer[prevDocNumber] == countWords)
						{
							pResultDocNumbers[countResultDocNumbers++] = prevDocNumber;
						}
					}

					if (prevDocNumber > maxDocNumber) //exit
					{
						return;
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						//mark documents
						if (prevDocNumber >= minDocNumber && pMergeBuffer[prevDocNumber])
						{
							pMergeBuffer[prevDocNumber]++;

							if (pMergeBuffer[prevDocNumber] == countWords)
							{
								pResultDocNumbers[countResultDocNumbers++] = prevDocNumber;
							}
						}

						if (prevDocNumber > maxDocNumber) //exit
						{
							return;
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void markMatchDocuments(int* pWeightBuffer,
		uchar8* pLevelBuffer,
		uint32* pUsedDocNumbers,
		uint32& countUsedDocNumbers,
		int weight,
		uchar8 level,
		bool isMarkLevel,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					if (pLevelBuffer[prevDocNumber] == level)
					{
						//mark documents
						if (!pWeightBuffer[prevDocNumber])
						{
							pUsedDocNumbers[countUsedDocNumbers++] = prevDocNumber;
						}

						pWeightBuffer[prevDocNumber] += weight;


						if (isMarkLevel)
						{
							pLevelBuffer[prevDocNumber]++;
						}
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						//mark documents
						if (pLevelBuffer[prevDocNumber] == level)
						{
							if (!pWeightBuffer[prevDocNumber])
							{
								pUsedDocNumbers[countUsedDocNumbers++] = prevDocNumber;
							}

							pWeightBuffer[prevDocNumber] += weight;

							if (isMarkLevel)
							{
								pLevelBuffer[prevDocNumber]++;
							}
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void markMatchDocuments(uchar8* pLevelBuffer,
		uchar8 level,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					//mark documents
					if (pLevelBuffer[prevDocNumber] == level)
					{
						pLevelBuffer[prevDocNumber]++;
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						//mark documents
						if (pLevelBuffer[prevDocNumber] == level)
						{
							pLevelBuffer[prevDocNumber]++;
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void markSelectorDocuments(PostSelector* pPostSelectorBuffer,
		uint32* pUsedDocNumbers,
		uint32& countUsedDocNumbers,
		uint32 wordId,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					//mark documents
					PostSelector* pPostSelector = &pPostSelectorBuffer[prevDocNumber];

					if (!pPostSelector->WordID[0])
					{
						pUsedDocNumbers[countUsedDocNumbers++] = prevDocNumber;
						pPostSelector->WordID[0] = wordId;
					}
					else
					{
					NEXT_SELECTOR1:

						if (!pPostSelector->WordID[1])
						{
							pPostSelector->WordID[1] = wordId;
						}
						else if (!pPostSelector->pNextPostSelector)
						{
							pPostSelector->pNextPostSelector = pPostSelectorPool->newObject();
							pPostSelector->pNextPostSelector->WordID[0] = wordId;
						}
						else //find
						{
							pPostSelector = pPostSelector->pNextPostSelector;

							goto NEXT_SELECTOR1;
						}
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						//mark documents
						PostSelector* pPostSelector = &pPostSelectorBuffer[prevDocNumber];

						if (!pPostSelector->WordID[0])
						{
							pUsedDocNumbers[countUsedDocNumbers++] = prevDocNumber;
							pPostSelector->WordID[0] = wordId;
						}
						else
						{
						NEXT_SELECTOR2:

							if (!pPostSelector->WordID[1])
							{
								pPostSelector->WordID[1] = wordId;
							}
							else if (!pPostSelector->pNextPostSelector)
							{
								pPostSelector->pNextPostSelector = pPostSelectorPool->newObject();
								pPostSelector->pNextPostSelector->WordID[0] = wordId;
							}
							else //find
							{
								pPostSelector = pPostSelector->pNextPostSelector;

								goto NEXT_SELECTOR2;
							}
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void calcMatchDocuments(uchar8* pLevelBuffer,
		uchar8 level,
		uint32& equals,
		uint32& notEquals,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				//mark documents
				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					if (pLevelBuffer[prevDocNumber] == level)
					{
						equals++;
					}
					else
					{
						notEquals++;
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						if (pLevelBuffer[prevDocNumber] == level)
						{
							equals++;
						}
						else
						{
							notEquals++;
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void markTrend(uint32* points,
		uint32 zoom,
		uint32 minPage,
		uint32 maxPage)
	{
		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				//mark documents
				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					points[(prevDocNumber - minPage) / zoom]++;
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						points[(prevDocNumber - minPage) / zoom]++;
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline void calcCountInRanges(uint32 minRange1,
		uint32 maxRange1,
		uint32 minRange2,
		uint32 maxRange2,
		uint32& countRange1,
		uint32& countRange2)
	{
		uint32 minPage = minRange1;
		uint32 maxPage = maxRange2;

		uint32 currPosition = 0;

		uint32 prevDocNumber = 0;

		BlockMemory* pBlockMemory = pHeadBlockMemory;

		while (true)
		{
			uchar8 header = pBlockMemory->pMemory[currPosition];

			//read document number
			uchar8 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			if (leftHeaderPart == 2)
			{
				prevDocNumber += rightHeaderPart;

				//mark documents
				if (prevDocNumber >= minPage)
				{
					if (prevDocNumber > maxPage)
					{
						return;
					}

					if (minRange1 <= prevDocNumber && prevDocNumber <= maxRange1)
					{
						countRange1++;
					}
					else if (minRange2 <= prevDocNumber && prevDocNumber <= maxRange2)
					{
						countRange2++;
					}
				}

				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}
			}
			else
			{
				//increment currPosition
				currPosition++;

				if (currPosition == BLOCK_SIZE)
				{
					pBlockMemory = pBlockMemory->pNextBlockMemory;

					currPosition = 0;
				}

				uint32 sizeNumber = leftHeaderPart + 1;
				uint32 deltaDocNumber = 0;

				for (uint32 i = 0; i < sizeNumber; i++)
				{
					deltaDocNumber = (deltaDocNumber << 8) | (uchar8)pBlockMemory->pMemory[currPosition];

					//increment currPosition
					currPosition++;

					if (currPosition == BLOCK_SIZE)
					{
						pBlockMemory = pBlockMemory->pNextBlockMemory;

						currPosition = 0;
					}
				}

				prevDocNumber += deltaDocNumber;

				//mark documents
				for (uint32 i = 0; i < rightHeaderPart; i++, prevDocNumber++)
				{
					if (prevDocNumber >= minPage)
					{
						if (prevDocNumber > maxPage)
						{
							return;
						}

						if (minRange1 <= prevDocNumber && prevDocNumber <= maxRange1)
						{
							countRange1++;
						}
						else if (minRange2 <= prevDocNumber && prevDocNumber <= maxRange2)
						{
							countRange2++;
						}
					}
				}

				prevDocNumber--;
			}

			if (pBlockMemory->pMemory[currPosition] == 0
				|| (pBlockMemory->pNextBlockMemory == 0 && currPosition == CurrentSize))
			{
				return; //is last document
			}
		}
	}

	inline bool hasDocNumber(uint32 docNumber)
	{
		return false;
	}

	BlockMemoryPool* pBlockMemoryPool;
	PostSelectorPool* pPostSelectorPool;

	BlockMemory* pHeadBlockMemory;
	BlockMemory* pTailBlockMemory;
	uchar8 CurrentSize;

	uint32 PrevDocNumber;
	uchar8 PrevPosNumber;

	uchar8 CountPosition;
	uchar8* pCurrentHeader;

	//uint32 FilePosition; //offset in index file
	//uint32 ImportFilePosition; //offset in import index file

	uint32 CountDocuments;

	~DocumentsBlock()
	{

	}
};