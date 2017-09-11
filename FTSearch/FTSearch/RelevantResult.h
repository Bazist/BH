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
		MatchWords = new char[256]; //max 25 words
	}

	char* MatchWords;

	char Matches[MAX_FOUND_DOCUMENTS_IN_QUERY][256];

	uint32 CountMatches;
	//uint32 FullCountMatches;
	
	~RelevantResult()
	{
		delete[] MatchWords;
	}
};