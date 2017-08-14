#pragma once

#ifndef RELEVANT_RESULT_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define RELEVANT_RESULT_POOL_HEADER 20	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "RelevantResult.h"

class RelevantResultPool
{
public:
	RelevantResultPool(char* pLastErrorMessage)
	{
		Count = 0;
		Size = 0;

		this->pLastErrorMessage = pLastErrorMessage;
	}

	RelevantResult** pFreeRelevantResultLists[MAX_POOL_PAGES];
	RelevantResult* pRelevantResultLists[MAX_POOL_PAGES];

	uint32 Count;
	uint32 Size;
	char* pLastErrorMessage;
	
	RelevantResult* newObject()
	{
		uint32 page = Count>>8;
		uint32 index = Count&0xFF;

		if(Count >= Size)
		{
			if(page >= MAX_POOL_PAGES)
			{
				sprintf(pLastErrorMessage, "RelevantResultPool is full.");
				return 0;
			}

			RelevantResult* pRelevantResults = new RelevantResult[MAX_CHAR];
			pRelevantResultLists[page] = pRelevantResults;

			pFreeRelevantResultLists[page] = new RelevantResult*[MAX_CHAR];

			for(uint32 i=0; i<MAX_CHAR; i++)
			{
				pFreeRelevantResultLists[page][i] = pRelevantResults + i;
			}
			
			Size += MAX_CHAR;
		}
		
		RelevantResult* pRelevantResult = pFreeRelevantResultLists[page][index];
		pRelevantResult->MatchWords[0] = 0;
		pRelevantResult->CountMatches = 0;
		
		Count++;

		return pRelevantResult;
	}

	void releaseObject(RelevantResult* pRelevantResult)
	{
		Count--;

		pFreeRelevantResultLists[Count>>8][Count&0xFF] = pRelevantResult;
	}

	void releaseObjects()
	{
		Count = 0;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * sizeof(RelevantResult) + sizeof(RelevantResultPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * sizeof(RelevantResult) + sizeof(RelevantResultPool);
	}

	void clear()
	{
		uint32 page = Size>>8;

		for(uint32 i=0; i<page; i++)
		{
			delete[] pFreeRelevantResultLists[i];
		}

		for(uint32 i=0; i<page; i++)
		{
			delete[] pRelevantResultLists[i];
		}

		Count = 0;
		Size = 0;
	}

	~RelevantResultPool()
	{
		clear();
	}
};