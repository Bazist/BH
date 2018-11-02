#pragma once

#ifndef SELECTOR_HEADER		// Allow use of features specific to Windows XP or later.                   
#define SELECTOR_HEADER 1450	// Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdlib.h> 
#include "stdafx.h"
#include "Dictionary.h"
#include "QueryRow.h"
#include "HArrayVisitor.h"
#include "DocumentsBlock.h"
#include "SearchRelPreCalcInfo.h"

class FilterWordsByDocumentsCount : public HArrayVisitor
{

public:
	FilterWordsByDocumentsCount(DocumentsBlockPool* pDocumentsBlockPool,
		SearchRelPreCalcInfo* pSearchRelPreCalcInfo,
		uchar8* pLevelBuffer,
		uint32 minAllowedCount,
		uint32 maxAllowedCount,
		bool isOnlyCalcAmountWords,
		uint32 countKeySegments)
	{
		this->pSearchRelPreCalcInfo = pSearchRelPreCalcInfo;
		this->pDocumentsBlockPool = pDocumentsBlockPool;
		this->MinAllowedCount = minAllowedCount;
		this->MaxAllowedCount = maxAllowedCount;
		this->isOnlyCalcAmountWords = isOnlyCalcAmountWords;
		this->CountKeySegments = countKeySegments;
		this->pLevelBuffer = pLevelBuffer;
		this->CountWords = 0;
	}

	SearchRelPreCalcInfo* pSearchRelPreCalcInfo;
	DocumentsBlockPool* pDocumentsBlockPool;
	uchar8* pLevelBuffer;
	uint32 MinAllowedCount;
	uint32 MaxAllowedCount;
	uint32 CountKeySegments;
	uint32 CountWords;
	bool isOnlyCalcAmountWords;

	virtual void onStartScan()
	{
	}

	virtual bool onVisit(uint32* key, ulong64 value)
	{
		DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(value);
		uint32 fullCountDocuments = pDocumentsBlock->CountDocuments;
		uint32 countDocuments = fullCountDocuments;
		
		if (MinAllowedCount <= fullCountDocuments && fullCountDocuments <= MaxAllowedCount)
		{
			if (pLevelBuffer)
			{
				uint32 equals = 0;
				uint32 notEquals = 0;

				pDocumentsBlock->calcMatchDocuments(pLevelBuffer, 1, equals, notEquals, 0, MAX_INT);

				countDocuments = equals;
			}

			if (MinAllowedCount <= countDocuments && countDocuments <= MaxAllowedCount)
			{
				if (!isOnlyCalcAmountWords)
				{
					pSearchRelPreCalcInfo->Words[this->CountWords].copyFrom(key, value, CountKeySegments);
					pSearchRelPreCalcInfo->CountDocuments[this->CountWords] = countDocuments;
				}

				this->CountWords++;
			}
		}
		return true;
	}

	virtual void onEndScan()
	{
	}
};