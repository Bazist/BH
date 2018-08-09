#pragma once

#ifndef DOCUMENTCELL_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTCELL_HEADER 11	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "DocumentInfo.h"

struct DocumentCell
{
	DocumentCell** pRefDocumentCell;
	uint32 Number;
	DocumentInfo* pDocInfos[MAX_WORDS_IN_QUERY];
	uchar8 Count;
};