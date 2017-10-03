// FTSearch.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FTSServer.h"
#include "HArrayTextFile.h"

FTSInstance* instances[256];
uint32 countInstances = 0;

void startInstance(FTSConfiguration configuration, bool onlyCheckIndex)
{
	if (!countInstances)
	{
		FTSInstance::initSharedResources();
	}

	instances[countInstances] = new FTSInstance();
	instances[countInstances]->startInstance(configuration, onlyCheckIndex);

	countInstances++;
}

//void startDefaultInstance(const char* path,
//						  uint32 instanceNumber,
//						  uint32 memoryMode,
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
//	configuration.MemoryMode = memoryMode;
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
	const char* templateName,
	uint32 templateNameLen,
	uint32 minPage,
	uint32 maxPage,
	uint32 skip)
{
	for (uint32 i = 0; i < countInstances; i++)
	{
		if (instances[i]->Configuration.InstanceNumber == instanceNumber)
		{
			return instances[i]->searchPhrase(phrase,
				phraseLen,
				templateName,
				templateNameLen,
				minPage,
				maxPage,
				skip);
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

void migrate(char* path)
{
	HArrayTextFile file;

	file.init(path, "", 3);

	HArrayFixHDD ha;
	ha.init(path, "", 12, 4, 24);

	HArrayFixPair* pairs = HArrayFixPair::CreateArray(10000000, 3);

	file.create();

	ha.open(BIN_FILE_BUFFER_SIZE);

	uint32 startHeader = 0;
	bool isBufferNotEnough = false;

	while (true)
	{
		uint32 count = ha.getKeysAndValuesByPortions(pairs, 10000000, startHeader, isBufferNotEnough);

		if (!count || isBufferNotEnough)
			break;

		for (uint32 i = 0; i < count; i++)
		{
			file.insert(pairs[i].Key, pairs[i].Value);
		}

		file.flush();
	}

	ha.close();

	file.close();

	HArrayFixPair::DeleteArray(pairs);
}

void clearInstance()
{
	/*HArrayTextFile file;

	for (ulong64 i = 2000000000; i < 0xFFFFFFFFFF; i++)
	{
		char bytes[5];

		file.setValue(i, bytes);

		ulong64 value = 0;

		file.getValue(bytes, value);

		if (value != i)
			value = value;
	}*/

	//instance.clearInstance("c:\\fts");

	/*uint32 controlValue = 0;

	HArrayTextFile file;

	file.init("c:\\fts\\", "dic", 3);

	file.create();
	
	for (ulong64 i = 0; i < 500000000; i++)
	{
		if (i == 214750728)
			i = i;

		char word[256];

		sprintf(word, "%u", i);

		uint32 key[8];

		HArrayVisitor::getPartWords(word, strlen(word), 8, key, 12);

		controlValue += key[0];
		controlValue += i * 10;

		file.insert(key, i*10);
	}

	file.close();

	file.open();*/

	//file.init("c:\\fts\\dic.ha", 3, 12, false, false);

	/*char word[256];

	sprintf(word, "555555");

	uint32 key[8];

	HArrayVisitor::getPartWords(word, strlen(word), 8, key, 12);

	uint32 val = file.getValueByKey(key);*/

	/*ulong64 blockNumber = 0;
	uint32 wordInBlock = 0;

	HArrayFixPair* pairs = HArrayFixPair::CreateArray(1000000, 3);

	ulong64 val = 0;

	uint32 controlValue2 = 0;

	while (true)
	{
		uint32 count = file.getKeysAndValuesByPortions(pairs, 1000000, blockNumber, wordInBlock);

		if (!count)
			break;

		for (ulong64 i = 0; i < count; i++, val++)
		{
			if (pairs[i].Value != val * 10)
				i = i;

			controlValue2 += pairs[i].Key[0];
			controlValue2 += pairs[i].Value;
		}
	}

	if (controlValue != controlValue2)
		controlValue = controlValue;*/
	
	//for (int i = 0; i < 1000; i++)
	//{
	//	if (i != (uint32)pairs[i].Value)
	//		i = i; //error
	//}

	//count = file.getKeysAndValuesByPortions(pairs, 1000, blockNumber, wordInBlock);

	//for (int i = 0; i < 1000; i++)
	//{
	//	if (i != (uint32)pairs[i].Value - 1000)
	//		i = i; //error
	//}

	/*
	migrate("i:\\FTS_Merged\\Instance3");
	migrate("i:\\FTS_Merged\\Instance19");
	*/

	/*migrate("i:\\FTS_Merged\\Instance23");
	migrate("i:\\FTS_Merged\\Instance33");
	migrate("i:\\FTS_Merged\\Instance47");
	migrate("i:\\FTS_Merged\\Instance54");
	migrate("i:\\FTS_Merged\\Instance57");
	migrate("i:\\FTS_Merged\\Instance68");
	migrate("i:\\FTS_Merged\\Instance74");
	migrate("i:\\FTS_Merged\\Instance80");
	migrate("i:\\FTS_Merged\\Instance81");
	migrate("i:\\FTS_Merged\\Instance82");
	migrate("i:\\FTS_Merged\\Instance83");*/
	
	//migrate("i:\\FTS_Merged\\Instance3");

	//importIndex(1, "C:\\FTS\\Inst2", false);
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

	if (!countInstances)
	{
		FTSInstance::destroySharedResources();
	}
}