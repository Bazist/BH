// FTSearch.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FTSServer.h"

FTSInstance* instances[256];
uint32 countInstances = 0;

void startInstance(FTSConfiguration configuration, bool onlyCheckIndex)
{
	instances[countInstances] = new FTSInstance();
	instances[countInstances]->startInstance(configuration, onlyCheckIndex);

	countInstances++;
}

//void startDefaultInstance(const char* path,
//						  uint32 instanceNumber,
//						  bool inMemoryMode,
//						  uint32 autoStemmingOn,
//						  uint32 minLenWord,
//						  uint32 maxLenWord,
//						  uint32 documentNameSize,
//						  uint32 countWordInPhrase,
//						  bool isUseNumberAlphabet)
//{
//	FTSConfiguration configuration;
//	
//	configuration.setIndexPath(path, instanceNumber);
//
//	configuration.InstanceNumber = instanceNumber;
//	configuration.InMemoryMode = inMemoryMode;
//	configuration.AutoStemmingOn = autoStemmingOn;
//	configuration.MinLenWord = minLenWord;
//	configuration.MaxLenWord = maxLenWord;
//	configuration.WordsHeaderBase = documentNameSize;
//	configuration.CountWordInPhrase = countWordInPhrase;
//	configuration.IsUseNumberAlphabet = isUseNumberAlphabet;
//
//	startInstance(configuration, false);
//}

RelevantResult* searchPhrase(uint32 instanceNumber,
							 const char* phrase,
							 uint32 phraseLen,
							 uint32 minPage,
							 uint32 maxPage)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->searchPhrase(phrase,
				phraseLen,
				minPage,
				maxPage);
		}
	}
}

RelevantResult* searchPhraseRel(uint32 instanceNumber,
								const char* phrase,
								uint32 phraseLen,
								uint32 minPage,
								uint32 maxPage)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->searchPhraseRel(phrase, phraseLen, minPage, maxPage);
		}
	}
}

QueryResult* searchQuery(uint32 instanceNumber, 
						 Selector** selectors,
						 uint32 countSelectors,
						 uint32 minPage,
						 uint32 maxPage,
						 uint32 skip,
						 bool agregateBySubject)
{
	/*
	QueryResult* res = new QueryResult();
	res->minWeight = 111;
	res->minWeightIndex = 333;

	QueryRow row;
	row.DocNumber = 555;
	memset(row.Text, 0, 128);
	strcpy(row.Text, "Булка");

	res->addQueryRow(row);

	return res;
	*/

	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->searchQuery(selectors,
				countSelectors,
				minPage,
				maxPage,
				skip,
				agregateBySubject);
		}
	}
}

RelevantResult* searchNewMems(uint32 instanceNumber, 
							   uint32 startDate1Year,
							   uint32 startDate1Month,
							   uint32 startDate2Year,
							   uint32 startDate2Month,
							   uint32 startDate2Day,
							   uint32 endDate2Year,
							   uint32 endDate2Month,
							   uint32 endDate2Day,
							   uint32 periodInDays)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->searchNewMems(startDate1Year,
				startDate1Month,
				startDate2Year,
				startDate2Month,
				startDate2Day,
				endDate2Year,
				endDate2Month,
				endDate2Day,
				periodInDays);
		}
	}
}

void calculateTrend(uint32 instanceNumber, 
					const char* phrase,
					uint32 phraseLen,
					uint32* points,
					uint32 count,
					uint32 minPage,
					uint32 maxPage)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->calculateTrend(phrase,
				phraseLen,
				points,
				count,
				minPage,
				maxPage);
		}
	}
}

RelevantResult* searchHtmlSeldomWords(uint32 instanceNumber, char* text, uint32 textLen)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->searchHtmlSeldomWords(text, textLen);
		}
	}
}

void releaseRelevantResult(uint32 instanceNumber, RelevantResult* pRelevantResult)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->releaseRelevantResult(pRelevantResult);
		}
	}
}

void releaseQueryResult(uint32 instanceNumber, QueryResult* pQueryResult)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->releaseQueryResult(pQueryResult);
		}
	}
}

FTSInstanceInfo* getInfo(uint32 instanceNumber)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->getInfo();
		}
	}
}

bool indexText(uint32 instanceNumber, const char* name, uint32 nameLen, char* text, uint32 textLen)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->indexText(name, nameLen, text, textLen);
		}
	}
}

bool indexHtml(uint32 instanceNumber, const char* name, uint32 nameLen, char* text, uint32 textLen)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->indexHtml(name, nameLen, text, textLen);
		}
	}
}

void getDocumentNameByID(uint32 instanceNumber, uint32 id, char* name, uint32 sizeName)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			instances[i]->getDocumentNameByID(id, name, sizeName);

			break;
		}
	}
}

void clearInstance()
{
	//instance.clearInstance("c:\\fts");
}

void saveIndex(uint32 instanceNumber)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			instances[i]->saveIndex();

			break;
		}
	}
}

void importIndex(uint32 instanceNumber,
				 const char* importPath,
				 const bool isDeleteImportedIndex)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			instances[i]->importIndex(importPath);

			break;
		}
	}
}

void stopInstance(uint32 instanceNumber)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			instances[i]->stopInstance();
			
			delete instances[i];

			instances[i] = instances[countInstances - 1];

			countInstances--;

			break;
		}
	}
}