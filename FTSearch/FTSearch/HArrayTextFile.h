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
	uint32 maxKeyLen;
	uint32 autoStemmingOn;

public:

	HArrayTextFile()
	{
		memset(this, 0, sizeof(HArrayTextFile));
	}
	
	void init(char* fileName,
			  uint32 maxKeyLen,
			  uint32 autoStemmingOn,
			  bool isWritable,
			  bool isOverwrite)
	{
		this->pFile = new BinaryFile(fileName, isWritable, isOverwrite);

		this->fileSize = pFile->getFileSize();
		this->maxKeyLen = maxKeyLen;
		this->autoStemmingOn = autoStemmingOn;
	}

	void insert(const char* word, ulong64 value, uint32 wordLen)
	{
		uint32 pos = 0;

		//find position of two equals string
		for (; pos < wordLen; pos++)
		{
			if (templateWord[pos] != word[pos])
			{
				break;
			}

			templateWord[pos] = word[pos];
		}

		//fill rest of string by current word
		uint32 rest = pos;
		for (; rest < pos; rest++)
		{
			templateWord[pos] = word[pos];
		}

		//fill zero of tail
		while (templateWord[rest])
		{
			templateWord[rest] = 0;
			rest++;
		}

		uint32 currLen = wordLen - pos;

		//can be writed in current block ?
		if (fileSize % HARRAY_TEXT_FILE_BLOCK_SIZE + currLen + 6 > HARRAY_TEXT_FILE_BLOCK_SIZE) //No, start from new block
		{
			currLen = wordLen;
			pos = 0;

			pFile->writeZero(HARRAY_TEXT_FILE_BLOCK_SIZE - (fileSize % HARRAY_TEXT_FILE_BLOCK_SIZE + 6));
		}

		//1. Write header
		//4 bits for position of prefix + 4 bits for rest of word
		uchar8 header = (pos << 4) | currLen;

		pFile->writeByte(&header);

		//write word
		pFile->write(word + pos, currLen);

		//write value
		uchar8 val1 = value >> 32;
		pFile->writeByte(&val1);

		uint32 val2 = value & 0xFFFFFFFF;
		pFile->writeInt(&val2);

		fileSize += (currLen + 1);
	}

	ulong64 getValueByKey(const uint32* key, uint32 keyLen)
	{
		HArrayVisitor::getWord(templateWord, key, autoStemmingOn);

		return getValueByKey(templateWord, 0, fileSize);
	}

	ulong64 getValue(char* data)
	{
		return ((ulong64)(uchar8)data[0] << 32) | *(uint32*)&data[1];
	}

	int readBlock(ulong64 pos,
				  const char* findWord,
				  ulong64& value)
	{
		char currWord[256];

		char data[HARRAY_TEXT_FILE_BLOCK_SIZE];

		uint32 len = pFile->read(data, HARRAY_TEXT_FILE_BLOCK_SIZE);

		bool bFirst = true;

		for (uint32 i = 0; i < len;)
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
						i += 5; //skip value						
					}
				}
				else //found !
				{
					value = getValue(data + i);

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
			ulong64 midPos = (endPos - startPos) / 2 / HARRAY_TEXT_FILE_BLOCK_SIZE * HARRAY_TEXT_FILE_BLOCK_SIZE;
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
									  uint32& blockNumber,
									  uint32& wordInBlock)
	{
		uint32 count = 0;

		uint32 blocks = fileSize / HARRAY_TEXT_FILE_BLOCK_SIZE;

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

					if (currWordInBlock > wordInBlock)
					{
						//get word
						HArrayVisitor::getPartWords(currWord,
													startPos + wordLen,
													maxKeyLen,
													pairs[count++].Key,
													autoStemmingOn);
						

						//get value
						pairs[i].Value = getValue(data + i);

						i += 5;

						wordInBlock = currWordInBlock;

						//check overflow
						if (count >= size)
						{
							return count;
						}
					}

					bFirst = false;
				}
				else //value in next block
				{
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

