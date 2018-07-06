#pragma once

#ifndef POSITIONCELL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define POSITIONCELL_HEADER 12	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"

struct PositionCell
{
	uint QueryID;
	uchar Level;
	uchar Position;
};