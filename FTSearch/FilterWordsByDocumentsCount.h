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
		this->CountWords = 0;
	}

	SearchRelPreCalcInfo* pSearchRelPreCalcInfo;
	DocumentsBlockPool* pDocumentsBlockPool;
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
		uint32 countDocuments = pDocumentsBlock->CountDocuments;
		
		if (MinAllowedCount <= countDocuments && countDocuments <= MaxAllowedCount)
		{
			if (!isOnlyCalcAmountWords)
			{
				pSearchRelPreCalcInfo->Words[pSearchRelPreCalcInfo->CountWords].copyFrom(key, value, CountKeySegments);
				pSearchRelPreCalcInfo->CountDocuments[pSearchRelPreCalcInfo->CountWords] = countDocuments;

				pSearchRelPreCalcInfo->CountWords++;
			}

			this->CountWords++;
		}
		
		return true;
	}

	virtual void onEndScan()
	{
	}
};