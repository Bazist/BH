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

#ifndef DOCUMENT_INFO_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENT_INFO_POOL_HEADER 15	// Change this to the appropriate value to target other versions of Windows.
#endif


#include "stdafx.h"
#include "DocumentsInfo.h"

class DocumentsInfoPool
{
public:
	DocumentsInfoPool(char* pLastErrorMessage)
	{
		Count = 0;
		Size = 0;

		this->pLastErrorMessage = pLastErrorMessage;
	}

	DocumentsInfo** pFreeDocumentsInfoLists[MAX_POOL_PAGES];
	DocumentsInfo* pDocumentsInfoLists[MAX_POOL_PAGES];

	uint32 Count;
	uint32 Size;
	char* pLastErrorMessage;

	DocumentsInfo* newObject()
	{
		uint32 page = Count >> 16;
		uint32 index = Count & 0xFFFF;

		if (Count >= Size)
		{
			if (page >= MAX_POOL_PAGES)
			{
				sprintf(pLastErrorMessage, "DocumentInfoPool is full.");
				return 0;
			}

			DocumentsInfo* pDocumentInfos = new DocumentsInfo[MAX_SHORT];
			pDocumentsInfoLists[page] = pDocumentInfos;

			pFreeDocumentsInfoLists[page] = new DocumentsInfo*[MAX_SHORT];

			for (uint32 i = 0; i<MAX_SHORT; i++)
			{
				pFreeDocumentsInfoLists[page][i] = pDocumentInfos + i;
			}

			Size += MAX_SHORT;
		}

		DocumentsInfo* pDocumentsInfo = pFreeDocumentsInfoLists[page][index];

		Count++;

		return pDocumentsInfo;
	}

	void releaseObject(DocumentsInfo* pDocumentInfo)
	{
		Count--;

		pFreeDocumentsInfoLists[Count >> 16][Count & 0xFFFF] = pDocumentInfo;
	}

	void releaseObjects()
	{
		Count = 0;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * sizeof(DocumentsInfo) + sizeof(DocumentsInfoPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * sizeof(DocumentsInfo) + sizeof(DocumentsInfoPool);
	}

	void clear()
	{
		uint32 page = Size >> 16;

		for (uint32 i = 0; i<page; i++)
		{
			delete[] pFreeDocumentsInfoLists[i];
		}

		for (uint32 i = 0; i<page; i++)
		{
			delete[] pDocumentsInfoLists[i];
		}

		Count = 0;
		Size = 0;
	}

	~DocumentsInfoPool()
	{
		clear();
	}
};