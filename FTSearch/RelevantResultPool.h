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
		pRelevantResult->FullCountMatches = 0;

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