#pragma once

#include <direct.h>
#include "HArrayFixBase.h"
#include "HArrayVisitor.h"
#include "BinaryFile.h"
#include "DocumentsBlock.h"
#include "stdafx.h"

const uint32 HARRAY_TEXT_FILE_BLOCK_SIZE = 4096;
const uint32 HARRAY_TEXT_FILE_MAX_WORD_LEN = 64;
const uint32 HARRAY_TEXT_FILE_MAX_VALUE_BLOCK_LEN = 15;

struct HArrayTextFilePair
{
private:
	HArrayTextFilePair()
	{
	}

public:
	
	uint32* Key; //use additional memory
	ulong64 Value;

	uchar8 ValueBlock[HARRAY_TEXT_FILE_MAX_VALUE_BLOCK_LEN];
	uchar8 ValueBlockLen;

	static uint32 calcArrayMemory(uint32 count, uint32 countKeySegments)
	{
		return (sizeof(HArrayTextFilePair) + (countKeySegments << 2)) * count;
	}

	static HArrayTextFilePair* CreateArray(uint32 count, uint32 countKeySegments)
	{
		char* pArray = new char[calcArrayMemory(count, countKeySegments)];

		HArrayTextFilePair* pCurrentPair = (HArrayTextFilePair*)pArray;
		uint32* pCurrentKey = (uint32*)(pArray + sizeof(HArrayTextFilePair) * count);

		for (uint32 i = 0; i < count; i++,
			pCurrentPair++,
			pCurrentKey += countKeySegments)
		{
			pCurrentPair->Key = pCurrentKey;
			pCurrentPair->Value = 0;
		}

		return (HArrayTextFilePair*)pArray;
	}

	static void DeleteArray(HArrayTextFilePair* pArray)
	{
		delete[](char*)pArray;
	}

	//1 segment = 4 bytes
	int compareKeys(uint32* key, uint32 countKeySegments)
	{
		for (uint32 i = 0; i < countKeySegments; i++)
		{
			if (Key[i] < key[i])
				return -1;

			if (Key[i] > key[i])
				return 1;
		}

		return 0;
	}

	~HArrayTextFilePair()
	{
	}
};


class HArrayTextFile
{
private:
	BinaryFile* pFile;
	char templateWord[256];
	ulong64 fileSize;
	uint32 maxKeySegments;

public:

	HArrayTextFile()
	{
		memset(this, 0, sizeof(HArrayTextFile));
	}

	char FullPath[1024];
	char Path[1024];
	char TableName[256];

	void init(const char* path,
			  const char* name,
			  uint32 maxKeySegments)
	{
		if (!name[0])
		{
			sprintf(FullPath, "%s\\dictionary.ha", path);
		}
		else
		{
			sprintf(FullPath, "%s\\dictionary_%s.ha", path, name);
		}

		strcpy(Path, path);
		strcpy(TableName, name);

		this->maxKeySegments = maxKeySegments;
	}

	bool open()
	{
		if (!pFile)
		{
			this->pFile = new BinaryFile(FullPath, true, false);

			if (this->pFile->open())
			{
				this->fileSize = pFile->getFileSize();

				return true;
			}
		}

		return false;
	}

	bool create()
	{
		this->pFile = new BinaryFile(FullPath, true, true);

		if (this->pFile->open())
		{
			this->fileSize = 0;

			return true;
		}

		return false;
	}

	void openOrCreate()
	{
		if (BinaryFile::existsFile(FullPath))
		{
			open();
		}
		else
		{
			create();
		}
	}

	bool insert(const uint32* key,
				ulong64 value,
				DocumentsBlock* pDocumentsBlock)
	{
		if (pDocumentsBlock)
		{
			uchar8 valueBlock[HARRAY_TEXT_FILE_MAX_VALUE_BLOCK_LEN];

			uint32 valueBlockLen = 0;

			pDocumentsBlock->writeBlocksToBuffer(valueBlock, HARRAY_TEXT_FILE_MAX_VALUE_BLOCK_LEN, valueBlockLen);

			if (valueBlockLen < HARRAY_TEXT_FILE_MAX_VALUE_BLOCK_LEN) //max 16 bytes
			{
				insertBlock(key, valueBlock, valueBlockLen);

				return true;
			}
			else
			{
				insertValue(key, value);

				return false;
			}
		}
		else
		{
			insertValue(key, value);

			return false;
		}
	}

	void insertValue(const uint32* key,
					 ulong64 value)
	{
		char word[256];

		HArrayVisitor::getWord(word, key, maxKeySegments * 4);

		uint32 pos = 0;

		//find position of two equals string
		while (templateWord[pos] == word[pos] && pos < 8)
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

			//templateWord[0] = 0;
		}
		else if (blockPos == 0) //new block will be started
		{
			currLen = wordLen;
			startPos = 0;

			//templateWord[0] = 0;
		}

		//if (pFile->getPosition() == 3563520)
		//{
		//	startPos = startPos;
		//}

		//1. Write header
		ushort16 header;
		uchar8 blockLen = 0; //flag value 5 bytes

		packHeader(header, startPos, currLen, blockLen);

		pFile->writeShort(&header);

		//write word
		pFile->write(word + startPos, currLen);

		//write value
		char bytes[5];

		uint32 valueLen = setValue(value, bytes);

		pFile->write(bytes, valueLen);

		//header + word + value
		fileSize += (1 + currLen + valueLen);
	}

	void insertBlock(const uint32* key,
					 uchar8* valueBlock,
					 uint32 valueBlockLen)
	{
		char word[HARRAY_TEXT_FILE_MAX_WORD_LEN];

		HArrayVisitor::getWord(word, key, maxKeySegments * 4);

		uint32 pos = 0;

		//find position of two equals string
		while (templateWord[pos] == word[pos] && pos < 8) //max 8 symbols could be skipped
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

		if (blockPos + (1 + currLen + valueBlockLen) > HARRAY_TEXT_FILE_BLOCK_SIZE) //No, start from new block
		{
			currLen = wordLen;
			startPos = 0;

			uint32 len = HARRAY_TEXT_FILE_BLOCK_SIZE - blockPos;

			pFile->writeZero(len);

			fileSize += len;

			//templateWord[0] = 0;
		}
		else if (blockPos == 0) //new block will be started
		{
			currLen = wordLen;
			startPos = 0;

			//templateWord[0] = 0;
		}

		//if (pFile->getPosition() == 3563520)
		//{
		//	startPos = startPos;
		//}

		//1. Write header
		ushort16 header;
		
		packHeader(header, startPos, currLen, valueBlockLen);

		pFile->writeShort(&header);

		//write word
		pFile->write(word + startPos, currLen);

		//write block
		pFile->write(valueBlock, valueBlockLen);
		
		//header + word + block
		fileSize += (1 + currLen + valueBlockLen);
	}

	bool getValueByKey(const uint32* key,
					   ulong64& value,
					   uchar8* valueBlock,
					   uchar8& valueBlockLen)
	{
		//HArrayVisitor::getWord(templateWord, key, maxKeySegments * 4);

		ulong64 endPos = (fileSize & 0xFFFFFFFFFFFFF000) + HARRAY_TEXT_FILE_BLOCK_SIZE; //rounded to 4096

		return getValueByKey(key,
							 0,
							 endPos,
							 value,
							 valueBlock,
							 valueBlockLen);
	}

	void packHeader(ushort16& header,
					uchar8 prefixLen,
					uchar8 wordLen,
					uchar8 blockLen)
	{
		//format: [6bits: Prefix Len][6bits: Word Len][4bits: Block Len, If Block Len = 0 then Position 5 bytes, otherwise len of block]
		header = (prefixLen << 10) | (wordLen << 4) | blockLen;
	}

	void unpackHeader(ushort16 header,
					  uchar8& prefixLen,
					  uchar8& wordLen,
					  uchar8& blockLen)
	{
		//format: [6bits: Prefix Len][6bits: Word Len][4bits: Block Len, If Block Len = 0 then Position 5 bytes, otherwise len of block]
		prefixLen = header >> 10;
		wordLen = header << 6 >> 10;
		blockLen = header & 0xF;
	}

	uint32 getValue(char* bytes, ulong64& value)
	{
		ulong64 val1 = bytes[0];
		ulong64 val2 = *(uint32*)&bytes[1];

		value = (val1 << 32 | val2);

		return 5;

		//if (val >> 7) //5 bytes
		//{
		//	value = (((ulong64)(uchar8)bytes[0] & 0x7F) << 32 ) | //reset first byte
		//			((ulong64)(uchar8)bytes[1] << 24) |
		//			((ulong64)(uchar8)bytes[2] << 16) |
		//			((ulong64)(uchar8)bytes[3] << 8) |
		//			((ulong64)(uchar8)bytes[4]);

		//	return 5;
		//}
		//else //4 bytes
		//{
		//	value = ((ulong64)(uchar8)bytes[0] << 24) |
		//			((ulong64)(uchar8)bytes[1] << 16) |
		//			((ulong64)(uchar8)bytes[2] << 8) |
		//			((ulong64)(uchar8)bytes[3]);

		//	return 4;
		//}
	}

	uint32 setValue(ulong64 value, char* bytes)
	{
		bytes[0] = value >> 32;
		*(uint32*)&bytes[1] = (value & 0xFFFFFFFF);

		return 5;

		//if (value >> 31) //5 bytes
		//{
		//	bytes[0] = value << 24 >> 56 | 0x80; //set first bit
		//	bytes[1] = value << 32 >> 56;
		//	bytes[2] = value << 40 >> 56;
		//	bytes[3] = value << 48 >> 56;
		//	bytes[4] = value << 56 >> 56;

		//	return 5;
		//}
		//else //4 bytes
		//{
		//	bytes[0] = value << 32 >> 56;
		//	bytes[1] = value << 40 >> 56;
		//	bytes[2] = value << 48 >> 56;
		//	bytes[3] = value << 56 >> 56;

		//	return 4;
		//}
	}

	int compareKeys(const uint32* key1,
					const uint32* key2,
					uint32 countKeySegments)
	{
		for (uint32 i = 0; i < countKeySegments; i++)
		{
			if (key1[i] < key2[i])
				return -1;

			if (key1[i] > key2[i])
				return 1;

			if (!key1[i] && !key2[i])
				return 0;
		}

		return 0;
	}

	int readBlock(ulong64 pos,
				  const uint32* findKey,
				  ulong64& value,
				  uchar8* valueBlock,
				  uchar8& valueBlockLen)
	{
		valueBlockLen = 0;

		char currWord[256];

		char data[HARRAY_TEXT_FILE_BLOCK_SIZE];

		uint32 len = pFile->read(data, pos, HARRAY_TEXT_FILE_BLOCK_SIZE);

		bool bFirst = true;

		for (uint32 i = 0; i < len;)
		{
			//heaer
			ushort16 header = *(ushort16*)&data[i];
			uchar8 startPos;
			uchar8 wordLen;
			
			unpackHeader(header, startPos, wordLen, valueBlockLen);
			
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
								
				uint32 currKey[8];
				
				HArrayVisitor::getPartWords(currWord, startPos, currKey, maxKeySegments * 4);

				//check word
				int res = compareKeys(findKey, currKey, maxKeySegments);

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
						if (valueBlockLen)
						{
							i += valueBlockLen; //skip block
						}
						else
						{
							i += 5; //skip value						
						}
					}
				}
				else //found !
				{
					if (valueBlockLen)
					{
						for(uint32 j = 0; j < valueBlockLen; i++, j++)  //block
						{
							valueBlock[j] = data[i];							
						}
					}
					else
					{
						getValue(data + i, value);
					}

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

	bool getValueByKey(const uint32* key,
					    ulong64 startPos,
						ulong64 endPos,
						ulong64& value,
						uchar8* valueBlock,
						uchar8& valueBlockLen)
	{
		if (endPos - startPos > HARRAY_TEXT_FILE_BLOCK_SIZE)
		{
			//ulong64 midPos = (startPos + (endPos - startPos)) / 2 / HARRAY_TEXT_FILE_BLOCK_SIZE * HARRAY_TEXT_FILE_BLOCK_SIZE;

			ulong64 midPos = startPos + ((endPos - startPos) >> 1 & 0xFFFFFFFFFFFFF000);

			int res = readBlock(midPos,
								key,
								value,
								valueBlock,
								valueBlockLen);

			if (res)
			{
				//left part
				if (res < 0)
				{
					return getValueByKey(key,
										 startPos,
										 midPos,
										 value,
										 valueBlock,
										 valueBlockLen);
				}

				//right part
				if (res > 0)
				{
					return getValueByKey(key,
										 midPos,
										 endPos,
										 value,
										 valueBlock,
										 valueBlockLen);
				}
			}
			else //stop searching
			{
				return true;
			}
		}
		else //last block
		{
			int res = readBlock(startPos,
								key,
								value,
								valueBlock,
								valueBlockLen);

			if (!res) //stop searching
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	uint32 getKeysAndValuesByPortions(HArrayTextFilePair* pairs,
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
				ushort16 header = *(ushort16*)&data[i];
				uchar8 startPos;
				uchar8 wordLen;
				
				unpackHeader(header,
							 startPos,
							 wordLen,
							 pairs[count].ValueBlockLen);

				if (wordLen)
				{
					if (wordLen > maxKeySegments * 4)
					{
						printf("data corrupted\n");

						return 0;
					}

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
													pairs[count].Key,
													maxKeySegments * 4);

						if (pairs[count].ValueBlockLen)
						{
							for (uint32 j = 0; j < pairs[count].ValueBlockLen; i++, j++)  //block
							{
								pairs[count].ValueBlock[j] = data[i];
							}
						}
						else
						{
							i += getValue(data + i, pairs[count].Value);
						}

						/*if (pairs[count].Value == 2147507280)
							i = i;*/

						//get value
						count++;

						wordInBlock = currWordInBlock + 1;

						//check overflow
						if (count >= size)
						{
							return count;
						}
					}
					else
					{
						if (pairs[count].ValueBlockLen)
						{
							i += pairs[count].ValueBlockLen; //skip block
						}
						else
						{
							i += 5; //skip value						
						}
					}

					bFirst = false;
				}
				else //value in next block
				{
					break;
				}
			}

			wordInBlock = 0;
		}

		return count;
	}

	static void deleteFiles(char* path, char* tableName)
	{
		char fullPath[1024];

		if (!tableName[0])
		{
			sprintf(fullPath, "%s\\dictionary.ha", path);
		}
		else
		{
			sprintf(fullPath, "%s\\dictionary_%s.ha", path, tableName);
		}

		BinaryFile::deleteFile(fullPath);
	}

	static void renameFiles(char* path, char* oldTableName, char* newTableName)
	{
		char oldFullPath[1024];

		if (!oldTableName[0])
		{
			sprintf(oldFullPath, "%s\\dictionary.ha", path);
		}
		else
		{
			sprintf(oldFullPath, "%s\\dictionary_%s.ha", path, oldTableName);
		}

		char newFullPath[1024];

		if (!newTableName[0])
		{
			sprintf(newFullPath, "%s\\dictionary.ha", path);
		}
		else
		{
			sprintf(newFullPath, "%s\\dictionary_%s.ha", path, newTableName);
		}

		BinaryFile::renameFile(oldFullPath, newFullPath);

		////clear files
		//char oldInfoPath[1024];
		//char oldHeaderPath[1024];

		//getPath(oldInfoPath, path, "ha_info", oldTableName, 0);
		//getPath(oldHeaderPath, path, "ha_header", oldTableName, 0);

		//char newInfoPath[1024];
		//char newHeaderPath[1024];

		//getPath(newInfoPath, path, "ha_info", newTableName, 0);
		//getPath(newHeaderPath, path, "ha_header", newTableName, 0);

		//BinaryFile::renameFile(oldInfoPath, newInfoPath);
		//BinaryFile::renameFile(oldHeaderPath, newHeaderPath);

		//for (uint32 i = 0; i < MAX_HARRAY_PARTITIONS; i++)
		//{
		//	char oldContentPath[1024];
		//	char oldBranchPath[1024];
		//	char oldBlockPath[1024];

		//	getPath(oldContentPath, path, "ha_content", oldTableName, i + 1);
		//	getPath(oldBranchPath, path, "ha_branch", oldTableName, i + 1);
		//	getPath(oldBlockPath, path, "ha_block", oldTableName, i + 1);

		//	char newContentPath[1024];
		//	char newBranchPath[1024];
		//	char newBlockPath[1024];

		//	getPath(newContentPath, path, "ha_content", newTableName, i + 1);
		//	getPath(newBranchPath, path, "ha_branch", newTableName, i + 1);
		//	getPath(newBlockPath, path, "ha_block", newTableName, i + 1);

		//	BinaryFile::renameFile(oldContentPath, newContentPath);
		//	BinaryFile::renameFile(oldBranchPath, newBranchPath);
		//	BinaryFile::renameFile(oldBlockPath, newBlockPath);
		//}
	}

	void close()
	{
		destroy();
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

//class InsertToTextFileVisitor : public HArrayVisitor
//{
//
//private:
//	HArrayTextFile* pTextFile;
//
//public:
//	InsertToTextFileVisitor(HArrayTextFile* pTextFile)
//	{
//		this->pTextFile = pTextFile;
//	}
//
//	virtual void onStartScan()
//	{
//	}
//
//	virtual bool onVisit(uint32* key, ulong64 value)
//	{
//		this->pTextFile->insertValue(key, value);
//
//		return true;
//	}
//
//	virtual void onEndScan()
//	{
//	}
//
//	~InsertToTextFileVisitor()
//	{
//	}
//};

