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

#include "HArrayFixBase.h"

#pragma once

struct SearchRelPreCalcInfo
{
public:
	SearchRelPreCalcInfo()
	{
		Words = 0;
		CountDocuments = 0;
		Zooms = 0;
		CountWords = 0;
	}

	void init(const char* sourceName, uint32 countWords, uint32 countKeySegments)
	{
		strcpy(SourceName, sourceName);
		
		Words = HArrayFixPair::CreateArray(countWords, countKeySegments);
		CountDocuments = new uint32[countWords];
		Zooms = new DocumentsBlock*[countWords];

		CountWords = countWords;
	}

	char SourceName[256];

	HArrayFixPair* Words;
	uint32* CountDocuments;
	DocumentsBlock** Zooms;

	uint32 CountWords;

	~SearchRelPreCalcInfo()
	{
		if(Words)
			delete[] Words;

		if(CountDocuments)
			delete[] CountDocuments;

		if (Zooms)
			delete[] Zooms;
	}
};