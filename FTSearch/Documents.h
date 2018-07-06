#pragma once

#ifndef DOCUMENTS_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTS_HEADER 15	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "Document.h"

class Documents
{
public:
	Documents()
	{
		Size = 2;
		Count = 0;
		pDocuments = new Document*[Size];
	}

	void addWord(uint32 docNumber, uint32 position)
	{
		if(pDocuments[Count]->Number != docNumber)
		{
			Document* pDocument = new Document();
			pDocument->Number = docNumber;
			addDocument(pDocument);
		}

		if(pDocuments[Count]->Positions[pDocuments[Count]->Count-1] != position)
		{
			pDocuments[Count]->Positions[pDocuments[Count]->Count++] = position;
		}
	}

	void addDocument(Document* pDocument)
	{
		pDocuments[Count++] = pDocument;

		if(Count == Size)
		{
			Size*=2;
			
			Document** pNewDocuments = new Document*[Size];
			
			for(uint32 i=0; i<Count; i++)
			{
				pNewDocuments[i] = pDocuments[i];
			}

			delete[] pDocuments;
			pDocuments = pNewDocuments;
		}
	}

	void addRange(Documents* pDocuments)
	{
		for(uint32 i=0; pDocuments->Count; i++)
		{
			addDocument(pDocuments->pDocuments[i]);
		}
	}

	Document** pDocuments;
	uint32 Count;
	uint32 Size;

	~Documents()
	{
		delete[] pDocuments;
	}
};
