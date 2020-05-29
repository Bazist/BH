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

#ifndef DOCUMENT_CELL_POOL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENT_CELL_POOL_HEADER 15	// Change this to the appropriate value to target other versions of Windows.
#endif


#include "stdafx.h"
#include "DocumentCell.h"

class DocumentCellPool
{
public:
	DocumentCellPool(char** pLastErrorMessage)
	{
		Count = 0;
		Size = 0;

		this->pLastErrorMessage = pLastErrorMessage;
	}

	DocumentCell** pFreeDocumentCellLists[MAX_POOL_PAGES];
	DocumentCell* pDocumentCellLists[MAX_POOL_PAGES];

	uint32 Count;
	uint32 Size;
	char** pLastErrorMessage;
	
	DocumentCell* newObject()
	{
		uint32 page = Count>>16;
		uint32 index = Count&0xFFFF;

		if(Count >= Size)
		{
			if(page >= MAX_POOL_PAGES)
			{
				sprintf(*pLastErrorMessage, "DocumentCellPool is full.");
				return 0;
			}

			DocumentCell* pDocumentCells = new DocumentCell[MAX_SHORT2];
			pDocumentCellLists[page] = pDocumentCells;

			pFreeDocumentCellLists[page] = new DocumentCell*[MAX_SHORT2];

			for(uint32 i=0; i<MAX_SHORT2; i++)
			{
				pFreeDocumentCellLists[page][i] = pDocumentCells + i;
			}
			
			Size += MAX_SHORT2;
		}
		
		DocumentCell* pDocumentCell = pFreeDocumentCellLists[page][index];
		
		Count++;

		return pDocumentCell;
	}

	void releaseObject(DocumentCell* pDocumentCell)
	{
		Count--;

		pFreeDocumentCellLists[Count>>16][Count&0xFFFF] = pDocumentCell;
	}

	void releaseObjects()
	{
		Count = 0;
	}

	ulong64 getUsedMemory()
	{
		return (ulong64)Count * sizeof(DocumentCell) + sizeof(DocumentCellPool);
	}

	ulong64 getTotalMemory()
	{
		return (ulong64)Size * sizeof(DocumentCell) + sizeof(DocumentCellPool);
	}

	~DocumentCellPool()
	{
		uint32 page = Size>>16;

		for(uint32 i=0; i < page; i++)
		{
			delete[] pFreeDocumentCellLists[i];
		}

		for(uint32 i=0; i<page; i++)
		{
			delete[] pDocumentCellLists[i];
		}
	}
};