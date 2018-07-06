#pragma once

#ifndef MATCH_POSITIONS_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define MATCH_POSITIONS_POOL_HEADER 2223	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "MatchPositions.h"

class MatchPositionsPool
{
public:
	MatchPositionsPool(char** pLastErrorMessage)
	{
		Count = 0;
		Size = 0;
		this->pLastErrorMessage = pLastErrorMessage;
	}

	MatchPositions** pFreeBlockLists[MAX_POOL_PAGES];
	MatchPositions* pBlockLists[MAX_POOL_PAGES];
	
	uint Count;
	uint Size;
	char** pLastErrorMessage;
	
	MatchPositions* newObject()
	{
		uint page = Count>>16;
		uint index = Count&0xFFFF;

		if(Count >= Size)
		{
			if(page >= MAX_POOL_PAGES)
			{
				sprintf(*pLastErrorMessage, "MatchPositionsPool is full.");
				return 0;
			}

			MatchPositions* pMatchPositions = new MatchPositions[MAX_SHORT2];
			pBlockLists[page] = pMatchPositions;

			pFreeBlockLists[page] = new MatchPositions*[MAX_SHORT2];

			for(uint i=0; i<MAX_SHORT2; i++)
			{
				pFreeBlockLists[page][i] = pMatchPositions + i;
			}
			
			Size += MAX_SHORT2;
		}
		
		Count++;

		return pFreeBlockLists[page][index];
	}

	void releaseObject(MatchPositions* pMatchPositions)
	{
		Count--;

		pFreeBlockLists[Count>>16][Count&0xFFFF] = pMatchPositions;
	}

	void releaseObjects()
	{
		Count = 0;
	}

	uint getUsedMemory()
	{
		return Count * sizeof(MatchPositions) + sizeof(MatchPositionsPool);
	}

	uint getTotalMemory()
	{
		return Size * sizeof(MatchPositions) + sizeof(MatchPositionsPool);
	}

	~MatchPositionsPool()
	{
		uint page = Size>>16;

		for(uint i=0; i<page; i++)
		{
			delete[] pFreeBlockLists[i];
		}

		for(uint i=0; i<page; i++)
		{
			delete[] pBlockLists[i];
		}
	}
};