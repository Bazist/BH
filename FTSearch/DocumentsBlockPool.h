/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "BlockMemoryPool.h"
#include "PostSelectorPool.h"

class DocumentsBlockPool
{
public:
	DocumentsBlockPool(BlockMemoryPool* pBlockMemoryPool,
					   PostSelectorPool* pPostSelectorPool,
					   char* pLastErrorMessage)
	{
		Count = 1;
		Size = 0;

		this->pBlockMemoryPool = pBlockMemoryPool;
		this->pPostSelectorPool = pPostSelectorPool;
		this->pLastErrorMessage = pLastErrorMessage;
	}

	DocumentsBlock* pDocumentsBlockLists[MAX_POOL_PAGES];

	BlockMemoryPool* pBlockMemoryPool;
	PostSelectorPool* pPostSelectorPool;

	uint32 Count;
	uint32 Size;
	char* pLastErrorMessage;
	
	DocumentsBlock* newObject(uint32& id)
	{
		uint32 page = Count>>16;
		uint32 index = Count&0xFFFF;

		if (!checkSize(page))
			return 0;
		
		DocumentsBlock* pDocumentsBlock = &pDocumentsBlockLists[page][index];
		
		id = Count++;

		return pDocumentsBlock;
	}

	bool checkSize(int page)
	{
		if (Count >= Size)
		{
			if (page >= MAX_POOL_PAGES)
			{
				sprintf(pLastErrorMessage, "DocumentsBlockPool is full.");

				return false;
			}

			DocumentsBlock* pDocumentsBlocks = new DocumentsBlock[MAX_SHORT];
			pDocumentsBlockLists[page] = pDocumentsBlocks;

			for (uint32 i = 0; i<MAX_SHORT; i++)
			{
				pDocumentsBlockLists[page][i].pBlockMemoryPool = pBlockMemoryPool;
				pDocumentsBlockLists[page][i].pPostSelectorPool = pPostSelectorPool;
			}

			Size += MAX_SHORT;
		}

		return true;
	}

	DocumentsBlock* getTempObject()
	{
		//pDocumentsBlockLists[0][0].clear();

		if (!checkSize(0))
			return 0;

		return &pDocumentsBlockLists[0][0];
	}

	DocumentsBlock* getObject(uint32 id)
	{
		uint32 page = id >> 16;
		uint32 index = id & 0xFFFF;

		//pDocumentsBlockLists[page][index].clear();

		return &pDocumentsBlockLists[page][index];
	}

	/*void releaseObject(uint32 id)
	{
		Count--;

		DocumentsBlock* pDocumentsBlock = getObject(id);

		pDocumentsBlock->clear();

		pFreeDocumentsBlockLists[Count>>16][Count&0xFFFF] = id;
	}*/

	void releaseObjects()
	{
		for(uint32 i=1; i<Count; i++)
		{
			DocumentsBlock* pDocumentsBlock = &pDocumentsBlockLists[i>>16][i&0xFFFF];

			if(pDocumentsBlock)
			{
				pDocumentsBlock->clear();
			}
		}

		Count = 1;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * sizeof(DocumentsBlock) + sizeof(DocumentsBlockPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * sizeof(DocumentsBlock) + sizeof(DocumentsBlockPool);
	}

	void clear()
	{
		uint32 page = Size>>16;

		for(uint32 i=0; i<page; i++)
		{
			delete[] pDocumentsBlockLists[i];
		}

		Count = 1;
		Size = 0;
	}

	~DocumentsBlockPool()
	{
		clear();
	}
};