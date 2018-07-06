#pragma once

#ifndef BLOCK_MEMORY_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define BLOCK_MEMORY_POOL_HEADER 2	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "BlockMemory.h"

class BlockMemoryPool
{
public:
	BlockMemoryPool(char* pLastErrorMessage)
	{
		Count = 0;
		Size = 0;
		this->pLastErrorMessage = pLastErrorMessage;
	}

	BlockMemory** pFreeBlockLists[MAX_POOL_PAGES];
	BlockMemory* pBlockLists[MAX_POOL_PAGES];
	
	uint32 Count;
	uint32 Size;
	char* pLastErrorMessage;
	
	BlockMemory* newObject()
	{
		uint32 page = Count>>16;
		uint32 index = Count&0xFFFF;

		if(Count >= Size)
		{
			if(page >= MAX_POOL_PAGES)
			{
				sprintf_s(pLastErrorMessage, 1024, "BlockMemoryPool is full.");
				return 0;
			}

			BlockMemory* pBlockMemories = new BlockMemory[MAX_SHORT];
			pBlockLists[page] = pBlockMemories;

			pFreeBlockLists[page] = new BlockMemory*[MAX_SHORT];

			for(uint32 i=0; i<MAX_SHORT; i++)
			{
				pFreeBlockLists[page][i] = pBlockMemories + i;
			}
			
			Size += MAX_SHORT;
		}
		
		BlockMemory* pBlockMemory = pFreeBlockLists[page][index];
		pBlockMemory->pNextBlockMemory = 0;

		Count++;

		return pBlockMemory;
	}

	void releaseObject(BlockMemory* pBlockMemory)
	{
		Count--;

		pFreeBlockLists[Count>>16][Count&0xFFFF] = pBlockMemory;
	}

	void releaseObjects()
	{
		Count = 0;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * sizeof(BlockMemory) + sizeof(BlockMemoryPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * sizeof(BlockMemory) + sizeof(BlockMemoryPool);
	}

	void clear()
	{
		uint32 page = Size>>16;

		for(uint32 i=0; i<page; i++)
		{
			delete[] pFreeBlockLists[i];
		}

		for(uint32 i=0; i<page; i++)
		{
			delete[] pBlockLists[i];
		}

		Count = 0;
		Size = 0;
	}

	~BlockMemoryPool()
	{
		clear();
	}
};