#pragma once

#ifndef DOCUMENTS_NAME_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTS_NAME_HEADER 333	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "BlockMemoryPool.h"
#include "BinaryFile.h"

class DocumentsName
{
public:
	DocumentsName()
	{
		//FilePosition = 0;

		CurrentPosition = 0;

		for(uint32 i=0; i < MAX_SHORT; i++)
		{
			pPages[i] = 0;
		}
	}

	char* pPages[MAX_SHORT];

	void clear()
	{
		for(uint32 i=0; i < MAX_SHORT; i++)
		{
			if(pPages[i])
			{
				delete[] pPages[i];

				pPages[i] = 0;
			}
		}
		
		//FilePosition = 0;
		CurrentPosition = 0;
	}
	
	//format {[2bits size of docNumber][6 bits amount of positions]}{docNumber}{position1}...{positionN}
	char partName[2048];

	void addName(const char* name,
			     uint32 nameLen,
				 uint32& id, 
				 uint32 nameSize)
	{
		uint32 page = CurrentPosition >> 16;
		uint32 index = CurrentPosition & 0xFFFF;
		
		//create page
		char* pPage = pPages[page];
		if(!pPage)
		{
			pPage = new char[MAX_SHORT];
			pPages[page] = pPage;
		}

		//copy
		uint32 i;
		for(i = 0; i < nameLen; i++, index++, CurrentPosition++)
		{
			pPage[index] = name[i];
		}
		
		for(; i < nameSize; i++, index++, CurrentPosition++)
		{
			pPage[index] = 0;
		}

		id++;
	}

	void addBytes(uchar8* pBuffer, uint32 buffLength)
	{
		uint32 page = 0;
		uint32 index = 0;

		for(uint32 i = 0; i < buffLength; i++, CurrentPosition++)
		{
			page = CurrentPosition >> 16;
			index = CurrentPosition & 0xFFFF;
		
			//create page
			char* pPage = pPages[page];
			if(!pPage)
			{
				pPage = new char[MAX_SHORT];
				pPages[page] = pPage;
			}

			pPage[index] = pBuffer[i];
		}
	}

	void getDocumentNameByOffset(char* name, uint32 offset, uint32 sizeName)
	{
		//find start block
		uint32 page = offset >> 16;
		uint32 index = offset & 0xFFFF;

		char* pPage = pPages[page];

		for(uint32 i = 0; i < sizeName; i++, index++)
		{
			name[i] = pPage[index];
		}
	}

	bool hasDocumentName(const char* name, uint32 len, uint32 sizeName)
	{
		//find start block
		uint32 offset = 0;
		while(offset < CurrentPosition)
		{
			uint32 page = offset >> 16;
			uint32 index = offset & 0xFFFF;

			char* pPage = pPages[page];

			//compare string
			uint32 currIdx = 0;
			for(; currIdx < len; currIdx++, index++)
			{
				if(name[currIdx] != pPage[index])
				{
					goto EXIT;
				}
			}

			//compare zeros
			for(; currIdx < sizeName; currIdx++, index++)
			{
				if(pPage[index])
				{
					goto EXIT;
				}
			}

			return true;

			EXIT:

			offset += sizeName;
		}

		return false;
	}

	ulong64 getUsedMemory()
	{
		return CurrentPosition;
	}

	ulong64 getTotalMemory()
	{
		return CurrentPosition;
	}

	void writeBlocksToFile(BinaryFile* pBinaryFile,
						   uchar8* pBuffer, 
						   uint32 maxLength,
						   uint32& buffFilledLength)
	{

		//find start block
		uint32 page = CurrentPosition >> 16;
		uint32 index = CurrentPosition & 0xFFFF;

		uint32 maxSafeLength = maxLength - MAX_SHORT;
		
		//previous pages
		uint32 currPage;
		for(currPage = 0; currPage < page; currPage++)
		{
			if(buffFilledLength >= maxSafeLength) //flush buffer
			{
				pBinaryFile->write(pBuffer, buffFilledLength);
				buffFilledLength = 0;
			}

			for(uint32 j = 0; j < MAX_SHORT; j++, buffFilledLength++)
			{
				pBuffer[buffFilledLength] = pPages[currPage][j];
			}
		}

		if(buffFilledLength >= maxSafeLength) //flush buffer
		{
			pBinaryFile->write(pBuffer, buffFilledLength);
			buffFilledLength = 0;
		}

		//last page
		for(uint32 j = 0; j < index; j++)
		{
			pBuffer[buffFilledLength++] = pPages[currPage][j];
		}

		pBinaryFile->write(pBuffer, buffFilledLength);
		buffFilledLength = 0;
	}

	//uint32 FilePosition;
	uint32 CurrentPosition;

	~DocumentsName()
	{
		clear();
	}
};