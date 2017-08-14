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

	char DocName[64];
	char Text[128];

	uint32 CountFilterSelectors;
	uint32 CountColumnSelectors;
	uint32 CountSortSelectors;

	uint32 Weight;
	bool IsWeightCalcByGroupedSubjects;

	bool IsExcluded;
};