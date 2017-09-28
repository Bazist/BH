#pragma once

#include <direct.h>
#include "HArrayFixBase.h"
#include "HArrayVisitor.h"
#include "BinaryFile.h"
#include "stdafx.h"

const uint32 HARRAY_TEXT_FILE_BLOCK_SIZE = 4096;

class HArrayTextFile
{
private:
	BinaryFile* pFile;
	char templateWord[256];
	ulong64 fileSize;
	uint32 maxKeySegments;
	uint32 autoStemmingOn;

public:

	HArrayTextFile()
	{
		memset(this, 0, sizeof(HArrayTextFile));
	}

	void init(char* path,
		char* name,
		uint32 maxKeySegments,
		uint32 autoStemmingOn,
		bool isWritable,
		bool isOverwrite)
	{
		char fullPath[1024];

		sprintf(fullPath, "%s\\%s.ha", path, name);

		this->pFile = new BinaryFile(fullPath, isWritable, isOverwrite);

		this->maxKeySegments = maxKeySegments;
		this->autoStemmingOn = autoStemmingOn;
	}

	void open()
	{
		this->pFile->open();

		this->fileSize = pFile->getFileSize();
	}

	void insert(const uint32* key,
		ulong64 value)
	{
		char word[256];

		HArrayVisitor::getWord(word, key, autoStemmingOn);

		uint32 pos = 0;

		//find position of two equals string
		while (templateWord[pos] == word[pos])
		{
			pos++;
		}

		//fill rest of string by current word
		uint32 startPos = pos;

		for (; word[pos] != 0; pos++)
		{
			templateWord[pos] = word[pos];
		}

		uint32 wordLen = pos;

		//fill zero of tail
		while (templateWord[pos])
		{
			templateWord[pos] = 0;
			pos++;
		}

		uint32 currLen = wordLen - startPos;

		//can be writed in current block ?
		uint32 blockPos = fileSize % HARRAY_TEXT_FILE_BLOCK_SIZE;

		if (blockPos + currLen + 6 > HARRAY_TEXT_FILE_BLOCK_SIZE) //No, start from new block
		{
			currLen = wordLen;
			startPos = 0;

			uint32 len = HARRAY_TEXT_FILE_BLOCK_SIZE - blockPos;

			pFile->writeZero(len);

			fileSize += len;

			templateWord[0] = 0;
		}
		else if (blockPos == 0) //new block will be started
		{
			currLen = wordLen;
			startPos = 0;

			templateWord[0] = 0;
		}

		//if (pFile->getPosition() == 3563520)
		//{
		//	startPos = startPos;
		//}

		//1. Write header
		//4 bits for position of prefix + 4 bits for rest of word
		uchar8 header = (startPos << 4) | currLen;

		pFile->writeByte(&header);

		//write word
		pFile->write(word + startPos, currLen);

		uint32 valueLen = writeValue(value, pFile);

		//header + word + value
		fileSize += (1 + currLen + valueLen);
	}

	ulong64 getValueByKey(const uint32* key)
	{
		HArrayVisitor::getWord(templateWord, key, autoStemmingOn);

		ulong64 endPos = (fileSize & 0xFFFFFFFFFFFFF000) + HARRAY_TEXT_FILE_BLOCK_SIZE; //rounded to 4096

		return getValueByKey(templateWord, 0, endPos);
	}

	uint32 getValue(char* data, ulong64& value)
	{
		if (data[0] >> 7) //5 bytes
		{
			uchar8 val1 = (uchar8)data[0];

			val1 &= 0x7F; //reset first bit

			value = ((ulong64)val1 << 32) | *(uint32*)&data[1];

			return 5;
		}
		else //4 bytes
		{
			value = *(uint32*)data;

			return 4;
		}
	}

	uint32 writeValue(ulong64 value, BinaryFile* pFile)
	{
		if (value >> 31) //5 bytes
		{
			//write value
			uchar8 val1 = (value >> 32) | 0x80; //set first bit
			pFile->writeByte(&val1);

			uint32 val2 = value & 0xFFFFFFFF;
			pFile->writeInt(&val2);

			return 5;
		}
		else //4 bytes
		{
			uint32 val2 = (uint32)value;
			
			pFile->writeInt(&val2);

			return 4;
		}
	}

	int readBlock(ulong64 pos,
		const char* findWord,
		ulong64& value)
	{
		char currWord[256];

		char data[HARRAY_TEXT_FILE_BLOCK_SIZE];

		uint32 len = pFile->read(data, pos, HARRAY_TEXT_FILE_BLOCK_SIZE);

		bool bFirst = true;

		for (uint32 i = 0; i < len;)
		{
			//heaer
			uchar8 header = data[i];

			uchar8 startPos = header >> 4;
			uchar8 wordLen = startPos + (header & 0xF);

			if (bFirst && startPos)
			{
				printf("corrupted data");
				return 0;
			}

			if (wordLen)
			{
				i++;

				//word
				for (; startPos < wordLen; startPos++, i++)
				{
					currWord[startPos] = data[i];
				}

				currWord[startPos] = 0;

				//check word
				int res = strcmp(findWord, currWord);

				if (res)
				{
					if (res < 0) //word in left part
					{
						if (bFirst) //first word in block
						{
							return -1;
						}
						else //word is not found in block
						{
							value = 0;

							return 0;
						}
					}
					else //if(res > 0) 
					{
						i += getValue(data + i, value); //skip value						
					}
				}
				else //found !
				{
					getValue(data + i, value);

					return 0;
				}

				bFirst = false;
			}
			else //value in right block
			{
				return 1;
			}
		}
	}

	ulong64 getValueByKey(const char* word,
		ulong64 startPos,
		ulong64 endPos)
	{
		if (endPos - startPos > HARRAY_TEXT_FILE_BLOCK_SIZE)
		{
			//ulong64 midPos = (startPos + (endPos - startPos)) / 2 / HARRAY_TEXT_FILE_BLOCK_SIZE * HARRAY_TEXT_FILE_BLOCK_SIZE;

			ulong64 midPos = startPos + ((endPos - startPos) >> 1 & 0xFFFFFFFFFFFFF000);

			ulong64 value;

			int res = readBlock(midPos, word, value);

			if (res)
			{
				//left part
				if (res < 0)
				{
					return getValueByKey(word, startPos, midPos);
				}

				//right part
				if (res > 0)
				{
					return getValueByKey(word, midPos, endPos);
				}
			}
			else //stop searching
			{
				return value;
			}
		}
		else //last block
		{
			ulong64 value;

			int res = readBlock(startPos, word, value);

			if (!res) //stop searching
			{
				return value;
			}
			else
			{
				return 0;
			}
		}
	}

	uint32 getKeysAndValuesByPortions(HArrayFixPair* pairs,
		uint32 size,
		ulong64& blockNumber,
		uint32& wordInBlock)
	{
		uint32 count = 0;

		ulong64 blocks = fileSize / HARRAY_TEXT_FILE_BLOCK_SIZE;

		for (; blockNumber <= blocks; blockNumber++)
		{
			char currWord[256];

			char data[HARRAY_TEXT_FILE_BLOCK_SIZE];

			uint32 len = pFile->read(data,
									blockNumber * HARRAY_TEXT_FILE_BLOCK_SIZE,
									HARRAY_TEXT_FILE_BLOCK_SIZE);

			bool bFirst = true;

			uint32 currWordInBlock = 0;

			for (uint32 i = 0; i < len; currWordInBlock++)
			{
				//heaer
				uchar8 header = data[i];

				uchar8 startPos = header >> 4;
				uchar8 wordLen = startPos + (header & 0xF);

				if (wordLen)
				{
					i++;

					//word
					for (; startPos < wordLen; startPos++, i++)
					{
						currWord[startPos] = data[i];
					}

					currWord[startPos] = 0;

					if (currWordInBlock >= wordInBlock)
					{
						//get word
						HArrayVisitor::getPartWords(currWord,
													wordLen,
													maxKeySegments,
													pairs[count].Key,
													autoStemmingOn);


						//get value
						count++;

						i += getValue(data + i, pairs[count].Value);

						wordInBlock = currWordInBlock + 1;

						//check overflow
						if (count >= size)
						{
							return count;
						}
					}
					else
					{
						ulong64 value;

						i += getValue(data + i, value);
					}

					bFirst = false;
				}
				else //value in next block
				{
					wordInBlock = 0;

					break;
				}
			}
		}

		return count;
	}

	void close()
	{
		pFile->close();
	}

	void flush()
	{
		pFile->flush();
	}

	void destroy()
	{
		if (pFile)
		{
			pFile->close();

			delete pFile;

			pFile = 0;
		}
	}

	~HArrayTextFile()
	{
		destroy();
	}
};

