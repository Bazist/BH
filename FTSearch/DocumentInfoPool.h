#pragma once

#ifndef DOCUMENT_INFO_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENT_INFO_POOL_HEADER 15	// Change this to the appropriate value to target other versions of Windows.
#endif


#include "stdafx.h"
#include "DocumentInfo.h"

class DocumentInfoPool
{
public:
	DocumentInfoPool(char* pLastErrorMessage)
	{
		Count = 0;
		Size = 0;

		this->pLastErrorMessage = pLastErrorMessage;
	}

	DocumentInfo** pFreeDocumentInfoLists[MAX_POOL_PAGES];
	DocumentInfo* pDocumentInfoLists[MAX_POOL_PAGES];

	uint32 Count;
	uint32 Size;
	char* pLastErrorMessage;
	
	DocumentInfo* newObject()
	{
		uint32 page = Count>>16;
		uint32 index = Count&0xFFFF;

		if(Count >= Size)
		{
			if(page >= MAX_POOL_PAGES)
			{
				sprintf(pLastErrorMessage, "DocumentInfoPool is full.");
				return 0;
			}

			DocumentInfo* pDocumentInfos = new DocumentInfo[MAX_SHORT];
			pDocumentInfoLists[page] = pDocumentInfos;

			pFreeDocumentInfoLists[page] = new DocumentInfo*[MAX_SHORT];

			for(uint32 i=0; i<MAX_SHORT; i++)
			{
				pFreeDocumentInfoLists[page][i] = pDocumentInfos + i;
			}
			
			Size += MAX_SHORT;
		}
		
		DocumentInfo* pDocumentInfo = pFreeDocumentInfoLists[page][index];
		
		Count++;

		return pDocumentInfo;
	}

	void releaseObject(DocumentInfo* pDocumentInfo)
	{
		Count--;

		pFreeDocumentInfoLists[Count>>16][Count&0xFFFF] = pDocumentInfo;
	}

	void releaseObjects()
	{
		Count = 0;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * sizeof(DocumentInfo) + sizeof(DocumentInfoPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * sizeof(DocumentInfo) + sizeof(DocumentInfoPool);
	}

	void clear()
	{
		uint32 page = Size>>16;

		for(uint32 i=0; i<page; i++)
		{
			delete[] pFreeDocumentInfoLists[i];
		}

		for(uint32 i=0; i<page; i++)
		{
			delete[] pDocumentInfoLists[i];
		}

		Count = 0;
		Size = 0;
	}

	~DocumentInfoPool()
	{
		clear();
	}
};