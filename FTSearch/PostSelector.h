#pragma once

#ifndef POST_SELECTOR_HEADER		// Allow use of features specific to Windows XP or later.                   
#define POST_SELECTOR_HEADER 1451	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "Selector.h"

class PostSelector
{

public:
	PostSelector()
	{
		WordID[0] = 0;
		WordID[1] = 0;

		pNextPostSelector = 0;
	}

	static Selector* Selectors[MAX_SHORT];
	static char* Words[MAX_SHORT];
	static uint32 CountWords;

	static uint32 addSelectorWord(Selector* pSelector, char* word)
	{
		CountWords++;

		Selectors[CountWords] = pSelector;
		Words[CountWords] = word;

		return CountWords;
	}

	ushort16 WordID[2];
	
	PostSelector* pNextPostSelector;

	uint32 count()
	{
		uint32 count = 0;

		PostSelector* pPostSelector = this;

		while(pPostSelector)
		{
			if(!pPostSelector->WordID[0])
				break;

			count++;

			if(!pPostSelector->WordID[1])
				break;

			count++;

			pPostSelector = pPostSelector->pNextPostSelector;
		}

		return count;
	}
};