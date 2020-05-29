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