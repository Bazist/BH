#pragma once

#ifndef DOCUMENT_INFO_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENT_INFO_HEADER 30	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "DocumentsBlock.h"

struct DocumentsInfo
{
public:
	DocumentsInfo()
	{
	
	}

	uint32 CountDocuments;
	ulong64 FilePosition;
	DocumentsBlock* pDocumentsBlock;
};
