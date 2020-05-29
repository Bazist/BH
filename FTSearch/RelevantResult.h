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

#ifndef DOCUMENTCELL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTCELL_HEADER 14	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "Dictionary.h"

class RelevantResult
{
public:
	RelevantResult()
	{
		CountMatches = 0;
		FullCountMatches = 0;
		MatchWords = new char[256]; //max 25 words
	}

	char* MatchWords;

	char Matches[MAX_FOUND_DOCUMENTS_IN_QUERY][256];

	uint32 CountMatches;
	uint32 FullCountMatches;
	
	~RelevantResult()
	{
		delete[] MatchWords;
	}
};