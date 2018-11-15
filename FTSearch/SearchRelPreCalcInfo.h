#include "HArrayFixBase.h"

#pragma once

struct SearchRelPreCalcInfo
{
public:
	SearchRelPreCalcInfo()
	{
		Words = 0;
		CountDocuments = 0;
		Zooms = 0;
		CountWords = 0;
	}

	void init(const char* sourceName, uint32 countWords, uint32 countKeySegments)
	{
		strcpy(SourceName, sourceName);
		
		Words = HArrayFixPair::CreateArray(countWords, countKeySegments);
		CountDocuments = new uint32[countWords];
		Zooms = new DocumentsBlock*[countWords];

		CountWords = countWords;
	}

	char SourceName[256];

	HArrayFixPair* Words;
	uint32* CountDocuments;
	DocumentsBlock** Zooms;

	uint32 CountWords;

	~SearchRelPreCalcInfo()
	{
		if(Words)
			delete[] Words;

		if(CountDocuments)
			delete[] CountDocuments;

		if (Zooms)
			delete[] Zooms;
	}
};