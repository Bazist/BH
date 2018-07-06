#pragma once

#ifndef DOCUMENT_CELL_BLOCK_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENT_CELL_BLOCK_POOL_HEADER 15	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "DocumentCell.h"

class DocumentCellBlockPool
{
public:
	DocumentCellBlockPool(char** pLastErrorMessage)
	{
		Count = 0;
		Size = 0;

		for(uint32 i=0; i<MAX_FOUND_DOCUMENTS_IN_QUERY; i++)
		{
			pDocumentCellBlockLists[i] = 0;
		}

		this->pLastErrorMessage = pLastErrorMessage;
	}

	DocumentCell** pDocumentCellBlockLists[MAX_FOUND_DOCUMENTS_IN_QUERY];
	
	uint32 Count;
	uint32 Size;
	char** pLastErrorMessage;
	
	DocumentCell** newObject()
	{
		if(Count >= Size)
		{
			pDocumentCellBlockLists[Size] = new DocumentCell*[MAX_SHORT];

			for(uint32 i=0; i<MAX_SHORT; i++)
			{
				pDocumentCellBlockLists[Size][i] = 0;
			}

			Size++;
		}
		
		return pDocumentCellBlockLists[Count++];
	}

	void releaseObjects()
	{
		Count = 0;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * MAX_SHORT * 4 + sizeof(DocumentCellBlockPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * MAX_SHORT * 4 + sizeof(DocumentCellBlockPool);
	}

	~DocumentCellBlockPool()
	{
		for(uint32 i=0; i<MAX_FOUND_DOCUMENTS_IN_QUERY; i++)
		{
			delete[] pDocumentCellBlockLists[i];
		}
	}
};