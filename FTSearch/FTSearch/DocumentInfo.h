#pragma once

#ifndef DOCUMENT_INFO_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENT_INFO_HEADER 30	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "BlockMemory.h"

struct DocumentInfo
{
public:
	DocumentInfo()
	{
		Number = 0;
		FilePosition = 0;
		pHeadBlockMemory = 0;
		MemoryPosition = 0;
		CountPosition = 0;
	}

	uint32 Number;
	uint32 FilePosition;
	BlockMemory* pHeadBlockMemory;
	uint32 MemoryPosition;
	uchar8 CountPosition;
};
