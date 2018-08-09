#pragma once

#ifndef MATCH_POSITIONS_HEADER		// Allow use of features specific to Windows XP or later.                   
#define MATCH_POSITIONS_HEADER 1288	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"

struct MatchPositions
{
	MatchPositions()
	{
		Positions = new uchar[128];
	}

	uchar* Positions;
	uchar CountPositions;

	~MatchPositions()
	{
		delete[] Positions;
	}
};

