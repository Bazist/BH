#pragma once

#include "BinaryFile.h"
#include "stdafx.h"

const uint32 DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE = 4096;

class DocumentsNameTextFile
{
private:
	BinaryFile* pFile;
	char templateWord[1024];
	ulong64 fileSize;

public:

	DocumentsNameTextFile()
	{
		memset(this, 0, sizeof(DocumentsNameTextFile));
	}

	char FullPath[1024];
	char Path[1024];
	char TableName[256];

	void init(const char* path,
			  const char* name)
	{
		if (!name[0])
		{
			sprintf(FullPath, "%s\\documentNames.ha", path);
		}
		else
		{
			sprintf(FullPath, "%s\\documentNames_%s.ha", path, name);
		}

		strcpy(Path, path);
		strcpy(TableName, name);
	}

	bool open(uint32 bufferSize)
	{
		if (!pFile)
		{
			this->pFile = new BinaryFile(FullPath, true, false, bufferSize);

			if (this->pFile->open())
			{
				this->fileSize = pFile->getFileSize();

				return true;
			}
		}

		return false;
	}

	bool create(uint32 bufferSize)
	{
		this->pFile = new BinaryFile(FullPath, true, true, bufferSize);

		if (this->pFile->open())
		{
			this->fileSize = 0;

			return true;
		}

		return false;
	}

	void openOrCreate(uint32 bufferSize)
	{
		if (BinaryFile::existsFile(FullPath))
		{
			open(bufferSize);
		}
		else
		{
			create(bufferSize);
		}
	}

	void addDocumentName(const char* name,
						 uint32 nameLen,
						 uint32 id)
	{
		uint32 pos = 0;

		//find position of two equals string
		while (templateWord[pos] == name[pos])
		{
			pos++;
		}

		//fill rest of string by current word
		uint32 startPos = pos;

		for (; pos < nameLen; pos++)
		{
			templateWord[pos] = name[pos];
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
		uint32 blockPos = fileSize % DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE;

		if (blockPos + currLen + 6 > DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE) //No, start from new block
		{
			currLen = wordLen;
			startPos = 0;

			uint32 len = DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE - blockPos;

			pFile->writeZero(len);

			fileSize += len;

			//write id of document
			pFile->writeBuffered(&id, fileSize, 4);

			fileSize += 4;

			//templateWord[0] = 0;
		}
		else if (blockPos == 0) //new block will be started
		{
			currLen = wordLen;
			startPos = 0;

			//write id of document
			pFile->writeBuffered(&id, fileSize, 4);

			fileSize += 4;

			//templateWord[0] = 0;
		}

		//if (pFile->getPosition() == 3563520)
		//{
		//	startPos = startPos;
		//}

		//1. Write header
		//8 bits for position of prefix + 8 bits for rest of word
		ushort16 header = (startPos << 8) | currLen;

		pFile->writeBuffered(&header, fileSize, 2);

		fileSize += 2;

		//write name
		pFile->writeBuffered(name + startPos, fileSize, currLen);

		fileSize += currLen;
	}

	bool getDocumentName(uint32 id,
		char* name,
		uint32 sizeName)
	{
		//HArrayVisitor::getWord(templateWord, key, maxKeySegments * 4);

		ulong64 endPos = (fileSize & 0xFFFFFFFFFFFFF000) + DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE; //rounded to 4096

		return getDocumentName(id, name, sizeName, 0, endPos);
	}

	int readBlock(ulong64 pos,
		uint32 id,
		char* name,
		uint32 sizeName)
	{
		char data[DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE];

		uint32 len = pFile->readBuffered(data, pos, DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE);

		uint32 blockId = *(uint32*)data;

		bool bFirst = true;

		for (uint32 i = 4; i < len; blockId++)
		{
			//heaer
			ushort16 header = *(ushort16*)(data + i);

			ushort16 startPos = header >> 8;
			ushort16 wordLen = startPos + (header & 0xFF);

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
					name[startPos] = data[i];
				}

				name[startPos] = 0;

				//check id
				if (blockId != id)
				{
					if (blockId > id) //name in left part
					{
						if (bFirst) //first name in block
						{
							return -1;
						}
						else //name is not found in block
						{
							return -2;
						}
					}
				}
				else //found !
				{
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

	bool getDocumentName(uint32 id,
		char* name,
		uint32 sizeName,
		ulong64 startPos,
		ulong64 endPos)
	{
		if (endPos - startPos > DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE)
		{
			//ulong64 midPos = (startPos + (endPos - startPos)) / 2 / HARRAY_TEXT_FILE_BLOCK_SIZE * HARRAY_TEXT_FILE_BLOCK_SIZE;

			ulong64 midPos = startPos + ((endPos - startPos) >> 1 & 0xFFFFFFFFFFFFF000);

			ulong64 value;

			int res = readBlock(midPos, id, name, sizeName);

			switch (res)
			{
				case -2: //not found
					return false; 
				case -1: //in left part
					return getDocumentName(id, name, sizeName, startPos, midPos);
				case 0:  //found
					return true; 
				case 1:  //in right part
					return getDocumentName(id, name, sizeName, midPos, endPos);
				default:
					return false;
			}
		}
		else //last block
		{
			ulong64 value;

			int res = readBlock(startPos, id, name, sizeName);

			switch (res)
			{
				case -2: //not found
					return false;
				case -1: //in left part
					return false;
				case 0:  //found
					return true;
				case 1:  //in right part
					return false;
				default:
					return false;
			}
		}
	}

	/*
	uint32 getKeysAndValuesByPortions(HArrayFixPair* pairs,
		uint32 size,
		ulong64& blockNumber,
		uint32& wordInBlock)
	{
		uint32 count = 0;

		ulong64 blocks = fileSize / DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE;

		for (; blockNumber <= blocks; blockNumber++)
		{
			char currWord[256];

			char data[DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE];

			uint32 len = pFile->read(data,
									blockNumber * DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE,
									DOCUMENTS_NAME_TEXT_FILE_BLOCK_SIZE);

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

						i += getValue(data + i, pairs[count].Value);

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
						ulong64 value;

						i += getValue(data + i, value);
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
	*/

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
			pFile->flush();

			pFile->close();

			delete pFile;

			pFile = 0;
		}
	}

	void clear()
	{
		pFile->clear();
	}

	bool hasDocumentName(const char* name, uint32 len)
	{
		return false;
	}

	void loadIntoRAM()
	{
		pFile->loadIntoRAM();
	}

	void append(DocumentsNameTextFile* pDocumentsName)
	{
		this->flush();
		pDocumentsName->flush();

		this->pFile->append(pDocumentsName->pFile);
	}

	ulong64 getUsedMemory()
	{
		return sizeof(DocumentsNameTextFile) + fileSize;
	}

	ulong64 getTotalMemory()
	{
		return sizeof(DocumentsNameTextFile) + fileSize;
	}

	~DocumentsNameTextFile()
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
//		this->pTextFile->insert(key, value);
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

