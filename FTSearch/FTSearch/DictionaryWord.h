#pragma once

#ifndef DICTIONARYWORD_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DICTIONARYWORD_HEADER 144	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"

class DictionaryWord
{

public:
	char Word[64];
	uint32 DocID;
	int Weight;
};