#pragma once

#ifndef BLOCK_MEMORY_HEADER		// Allow use of features specific to Windows XP or later.                   
#define BLOCK_MEMORY_HEADER 1	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"

struct BlockMemory
{
	uchar8 pMemory[BLOCK_SIZE];
	BlockMemory* pNextBlockMemory;
};