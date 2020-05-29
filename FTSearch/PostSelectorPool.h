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

#pragma once

#ifndef POST_SELECTOR_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define POST_SELECTOR_POOL_HEADER 205	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "PostSelector.h"

class PostSelectorPool
{
public:
	PostSelectorPool(char* pLastErrorMessage)
	{
		Count = 0;
		Size = 0;

		this->pLastErrorMessage = pLastErrorMessage;
	}

	PostSelector** pFreePostSelectorLists[MAX_POOL_PAGES];
	PostSelector* pPostSelectorLists[MAX_POOL_PAGES];

	uint32 Count;
	uint32 Size;
	char* pLastErrorMessage;
	
	PostSelector* newObject()
	{
		uint32 page = Count>>16;
		uint32 index = Count&0xFFFF;

		if(Count >= Size)
		{
			if(page >= MAX_POOL_PAGES)
			{
				sprintf(pLastErrorMessage, "PostSelectorPool is full.");
				return 0;
			}

			PostSelector* pPostSelectors = new PostSelector[MAX_SHORT];
			pPostSelectorLists[page] = pPostSelectors;

			pFreePostSelectorLists[page] = new PostSelector*[MAX_SHORT];

			for(uint32 i=0; i<MAX_SHORT; i++)
			{
				pFreePostSelectorLists[page][i] = pPostSelectors + i;
			}
			
			Size += MAX_SHORT;
		}
		
		PostSelector* pPostSelector = pFreePostSelectorLists[page][index];
		pPostSelector->WordID[0] = 0;
		pPostSelector->WordID[1] = 0;
		pPostSelector->pNextPostSelector = 0;
		
		Count++;

		return pPostSelector;
	}

	void releaseObject(PostSelector* pPostSelector)
	{
		Count--;

		pFreePostSelectorLists[Count>>8][Count&0xFF] = pPostSelector;
	}

	void releasePostSelectorList(PostSelector& postSelector)
	{
		PostSelector* pPostSelector = postSelector.pNextPostSelector;
		
		while(pPostSelector)
		{
			PostSelector* pNextPostSelector = pPostSelector->pNextPostSelector;

			PostSelectorPool::releaseObject(pPostSelector);

			pPostSelector = pNextPostSelector;
		}
	}

	void releaseObjects()
	{
		Count = 0;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * sizeof(PostSelector) + sizeof(PostSelectorPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * sizeof(PostSelector) + sizeof(PostSelectorPool);
	}

	void clear()
	{
		uint32 page = Size>>8;

		for(uint32 i=0; i<page; i++)
		{
			delete[] pFreePostSelectorLists[i];
		}

		for(uint32 i=0; i<page; i++)
		{
			delete[] pPostSelectorLists[i];
		}

		Count = 0;
		Size = 0;
	}

	~PostSelectorPool()
	{
		clear();
	}
};