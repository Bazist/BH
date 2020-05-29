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

#ifndef QUERY_ROW_HEADER		// Allow use of features specific to Windows XP or later.                   
#define QUERY_ROW_HEADER 1455	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"

class QueryRow
{

public:
	QueryRow()
	{
		
	}

	uint32 DocNumber;

	char DocName[256];
	char Text[128];

	uint32 CountFilterSelectors;
	uint32 CountColumnSelectors;
	uint32 CountSortSelectors;

	uint32 Weight;
	bool IsWeightCalcByGroupedSubjects;

	bool IsExcluded;
};