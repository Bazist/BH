#include "StdAfx.h"
#include "FTSInstance.h"
#include "Dictionary.h"
#include "WordRaiting.h"
#include "PostSelector.h"

void FTSInstance::calcMatchDocuments(const char* word,
	uchar8* pLevelBuffer,
	uint32 level,
	uint32& equals,
	uint32& notEquals,
	uint32 minPage,
	uint32 maxPage,
	uchar8* pSourceBuffer,
	ulong64& sourceFilePosition,
	uint32& sourceBuffPosition,
	uint32& sourceBuffLength)
{
	if (!checkStartedInstance(true))
		return;

	HArrayVisitor::getPartWords(word,
		strlen(word),
		tempKey,
		Configuration.AutoStemmingOn);

	//Read from RAM
	if (Info.CountWordsRAM)
	{
		uint32 id = haWordsRAM.getValueByKey(tempKey);

		if (id)
		{
			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

			pDocumentsBlock->calcMatchDocuments(pLevelBuffer,
				level,
				equals,
				notEquals,
				minPage,
				maxPage);
		}
	}

	//Read from HDD
	if (Configuration.MemoryMode != IN_MEMORY_MODE
		&& Info.CountWordsHDD)
	{
		uint32 minDocID = getDocHeaderSize();
		uint32 maxDocID = Info.LastNameIDRAM;

		/*uint32 sourceFilePosition = haWordsHDD.getValueByKey(tempKey);

		if (sourceFilePosition == 93049103)
		{
			sourceFilePosition = sourceFilePosition;
		}*/

		if (sourceFilePosition)
		{
			//read docs
			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getTempObject();

			bool isFormatCorrupted = false;
			pDocumentsBlock->readBlocksFromFile(pDocFile,
				pSourceBuffer,
				sourceFilePosition,
				sourceBuffPosition,
				sourceBuffLength,
				MAX_SIZE_BUFFER,
				minDocID,
				maxDocID,
				isFormatCorrupted);

			if (isFormatCorrupted)
			{
				printf("Read corrupted.");
			}

			pDocumentsBlock->calcMatchDocuments(pLevelBuffer,
				level,
				equals,
				notEquals,
				minPage,
				maxPage);

			pDocumentsBlock->clear();
		}
	}
}

void FTSInstance::markMatchDocuments(const char* word,
	int* pWeightBuffer,
	uchar8* pLevelBuffer,
	uint32 level,
	uint32& countUsedDocNumbers,
	uint32& countTotalDocuments,
	uint32 minPage,
	uint32 maxPage)
{
	if (!checkStartedInstance(true))
		return;

	HArrayVisitor::getPartWords(word,
		strlen(word),
		tempKey,
		Configuration.AutoStemmingOn);

	//Read from RAM
	if (Info.CountWordsRAM)
	{
		uint32 id = haWordsRAM.getValueByKey(tempKey);

		if (id)
		{
			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

			pDocumentsBlock->markMatchDocuments(pWeightBuffer,
				pLevelBuffer,
				pUsedDocNumbers,
				countUsedDocNumbers,
				1,
				level,
				minPage,
				maxPage);

			countTotalDocuments = pDocumentsBlock->CountDocuments;

		}
	}

	uint32 minDocID = getDocHeaderSize();
	uint32 maxDocID = Info.LastNameIDRAM;

	//Read from HDD
	if (Configuration.MemoryMode != IN_MEMORY_MODE
		&& Info.CountWordsHDD)
	{
		ulong64 sourceFilePosition = haWordsHDD.getValueByKey(tempKey);

		if (sourceFilePosition)
		{
			//read docs
			uint32 id;
			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getTempObject();

			uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];

			uint32 sourceBuffPosition = 0;
			uint32 sourceBuffLength = 0;

			bool isFormatCorrupted = false;
			pDocumentsBlock->readBlocksFromFile(pDocFile,
				pSourceBuffer,
				sourceFilePosition,
				sourceBuffPosition,
				sourceBuffLength,
				MAX_SIZE_BUFFER,
				minDocID,
				maxDocID,
				isFormatCorrupted);
			delete[] pSourceBuffer;

			if (isFormatCorrupted)
			{
				logError("Read corrupted.");

				return;
			}

			pDocumentsBlock->markMatchDocuments(pWeightBuffer,
				pLevelBuffer,
				pUsedDocNumbers,
				countUsedDocNumbers,
				1,
				level,
				minPage,
				maxPage);

			countTotalDocuments += pDocumentsBlock->CountDocuments;

			//release documents
			pDocumentsBlock->clear();
		}
	}
}

void FTSInstance::buildWiki()
{
	uint32 countWords;
	uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;

	HArrayFixPair* pKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);

	//save haWordsRAM
	countWords = haWordsRAM.getKeysAndValuesByRange(pKeysAndValuesRAM,
		Info.CountWordsRAM,
		0,
		0);

	for (uint32 i = 0; i < countWords; i++)
	{
		DocumentsBlock* p = (DocumentsBlock*)pKeysAndValuesRAM[i].Value;
		if (p->CountDocuments > 1000 && p->CountDocuments <= 10000)
		{
			char word2[16];
			Selector::getWord(word2,
				pKeysAndValuesRAM[i].Key,
				Configuration.AutoStemmingOn);

			Dictionary dics[1];
			dics[0].addWord(word2, 10);

			char tmp[256];
			sprintf(tmp, "j:\\dics\\%s_w.txt", dics[0].Words[0].Word);

			if (!BinaryFile::existsFile(tmp))
			{
				//searchDistances(pKeysAndValuesRAM, countWords, dics, 1);
			}

			printf("=>%d\n", i);
		}
	}

	HArrayFixPair::DeleteArray(pKeysAndValuesRAM);
}

int FTSInstance::compareDocumentsCount(HArrayFixPair& pair1,
	HArrayFixPair& pair2,
	uint32 countKeySegments)
{
	DocumentsBlock* pDocumentsBlock1 = pDocumentsBlockPool->getObject(pair1.Value);
	DocumentsBlock* pDocumentsBlock2 = pDocumentsBlockPool->getObject(pair2.Value);

	if (pDocumentsBlock1->CountDocuments < pDocumentsBlock2->CountDocuments)
		return -1;

	if (pDocumentsBlock1->CountDocuments > pDocumentsBlock2->CountDocuments)
		return 1;

	return 0;
}

void FTSInstance::readBlocksFromFile(
	DocumentsBlock* pDocumentsBlock,
	uint32 minPage,
	uint32 maxPage,
	uchar8* pSourceBuffer,
	ulong64& sourceFilePosition,
	uint32& sourceBuffPosition,
	uint32& sourceBuffLength)
{
	pDocFile->setPosition(sourceFilePosition);

	bool isFormatCorrupted = false;
	pDocumentsBlock->readBlocksFromFile(pDocFile,
		pSourceBuffer,
		sourceFilePosition,
		sourceBuffPosition,
		sourceBuffLength,
		MAX_SIZE_BUFFER,
		minPage,
		maxPage,
		isFormatCorrupted);

	if (isFormatCorrupted)
	{
		printf("Read corrupted.");
	}
}

void FTSInstance::initDataForSearchDistances()
{
	uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];
	memset(pSourceBuffer, 0, MAX_SIZE_BUFFER);

	ulong64 sourceFilePosition = getDocHeaderSize();
	uint32 sourceBuffPosition = 0;
	uint32 sourceBuffLength = 0;

	uint32 minDocID = 1;
	uint32 maxDocID = Info.LastNameIDRAM;

	uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;

	pAllKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);
	pAllKeysAndValuesRAMCount = Info.CountWordsRAM;

	//HArrayFixRAM* pTempHAWordsRAM = 0;

	uint32 minAllowedCount = 25;
	uint32 maxAllowedCount = Info.CountWordsRAM / 20; //5%

																			 //save haWordsHDD
	/*if (Configuration.MemoryMode != IN_MEMORY_MODE && Info.CountWordsHDD)
	{
		pTempHAWordsRAM = new HArrayFixRAM();

		char indexPath[1024];
		Configuration.getIndexPath(indexPath);

		pTempHAWordsRAM->init(indexPath,
			"",
			Configuration.AutoStemmingOn,
			4,
			Configuration.WordsHeaderBase);

		haWordsHDD.close();

		pTempHAWordsRAM->load();

		uint32 count = pTempHAWordsRAM->getKeysAndValuesByRange(pAllKeysAndValuesRAM,
			Info.CountWordsHDD,
			0,
			0);

		haWordsHDD.open();
	}*/

	if (Info.CountWordsRAM)
	{
		//save haWordsRAM
		//HArrayFixPair* pKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);

		uint32 count = haWordsRAM.getKeysAndValuesByRange(pAllKeysAndValuesRAM,
			Info.CountWordsRAM,
			0,
			0);

		//merge results
		//for (uint32 i = 0; i < count; i++)
		//{
		//	if (pTempHAWordsRAM->getValueByKey(pKeysAndValuesRAM[i].Key) == 0)
		//	{
		//		DocumentsBlock* pDocumentsBlock = (DocumentsBlock*)pKeysAndValuesRAM[i].Value;

		//		//filter elements not in the range
		//		if (minAllowedCount <= pDocumentsBlock->CountDocuments && pDocumentsBlock->CountDocuments <= maxAllowedCount)
		//		{
		//			pKeysAndValuesRAM[i].copyTo(pAllKeysAndValuesRAM[pAllKeysAndValuesRAMCount++], countKeySegments);
		//		}
		//	}
		//}

		//remove elements not in the range
		uint32 currLeft = 0;
		uint32 currRight = count - 1;

		while (currLeft < currRight)
		{
			//read docs
			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(pAllKeysAndValuesRAM[currLeft].Value);

			uint32 countDocuments = pDocumentsBlock->CountDocuments;

			if (minAllowedCount <= countDocuments && countDocuments <= maxAllowedCount)
			{
				//nothing
			}
			else
			{
				pDocumentsBlock->clear();

				while (currLeft < currRight)
				{
					DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(pAllKeysAndValuesRAM[currRight].Value);

					uint32 countDocuments = pDocumentsBlock->CountDocuments;

					if (minAllowedCount <= countDocuments && countDocuments <= maxAllowedCount)
					{
						pAllKeysAndValuesRAM[currRight].copyTo(pAllKeysAndValuesRAM[currLeft], countKeySegments);

						currRight--;

						break;
					}
					else
					{
						pDocumentsBlock->clear();

						currRight--;
					}
				}
			}

			currLeft++;
		}

		pAllKeysAndValuesRAMCount = currRight;

		//HArrayFixPair::DeleteArray(pKeysAndValuesRAM);
	}

	/*if (pTempHAWordsRAM)
	{
		pTempHAWordsRAM->destroy();

		delete pTempHAWordsRAM;
	}*/

	//sort by count
	HArrayFixPairUtils::sort(pAllKeysAndValuesRAM,
		pAllKeysAndValuesRAMCount,
		true,
		countKeySegments,
		compareDocumentsCount);

	//calc optimal start scan positions
	pStartScanWordsFrom = new uint32[Info.LastNameIDRAM];
	memset(pStartScanWordsFrom, 0, Info.LastNameIDRAM * sizeof(uint32));

	for (uint32 i = 0; i < pAllKeysAndValuesRAMCount; i++)
	{
		DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(pAllKeysAndValuesRAM[i].Value);

		if (!pStartScanWordsFrom[pDocumentsBlock->CountDocuments])
		{
			pStartScanWordsFrom[pDocumentsBlock->CountDocuments] = i;
		}
	}

	delete[] pSourceBuffer;
}

void FTSInstance::searchDistances(WordRaiting& wordRaiting,
	Dictionary* dics,
	uint32 count,
	uint32 minPage,
	uint32 maxPage)
{
	if (!checkStartedInstance(true))
		return;


	if (!pAllKeysAndValuesRAM)
	{
		initDataForSearchDistances();
	}

	uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];
	memset(pSourceBuffer, 0, MAX_SIZE_BUFFER);

	ulong64 sourceFilePosition = getDocHeaderSize();
	uint32 sourceBuffPosition = 0;
	uint32 sourceBuffLength = 0;

	FILE* file = 0;

	//1. Parse phrase
	int* pWeightBuffer = new int[Info.LastNameIDRAM + 1];
	uchar8* pLevelBuffer = new uchar8[Info.LastNameIDRAM + 1];

	memset(pWeightBuffer, 0, sizeof(int) * (Info.LastNameIDRAM + 1));
	memset(pLevelBuffer, 0, Info.LastNameIDRAM + 1);

	uint32 countUsedDocNumbers = 0;
	uint32 countTotalDocuments = 0;

	//mark first words
	uint32 level = 0;

	for (uint32 i = 0; i < count; i++, level++)
	{
		markMatchDocuments(dics[i].Words[0].Word,
			pWeightBuffer,
			pLevelBuffer,
			level,
			countUsedDocNumbers,
			countTotalDocuments,
			minPage,
			maxPage);
	}

	//level = 1;

	//calculate documents
	uint32 countDocuments = 0;

	for (uint32 i = 0; i < countUsedDocNumbers; i++)
	{
		uint32 usedDocNumber = pUsedDocNumbers[i];

		if (pLevelBuffer[usedDocNumber] == level)
		{
			countDocuments++;
		}
	}

	printf("Count docs: %d\n", countDocuments);

	char tempWord[16];

	uint32 start = pStartScanWordsFrom[countTotalDocuments];
	uint32 curr = start + 1;
	uint32 stop = pAllKeysAndValuesRAMCount;
	bool ascScanning = true;

	//search distances
	while (true)
	{
		//check max possible weight
		DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(pAllKeysAndValuesRAM[curr].Value);

		uint32 count1 = countTotalDocuments;
		uint32 count2 = pDocumentsBlock->CountDocuments;

		uint32 maxMatched;
		uint32 maxCount;

		if (count1 < count2)
		{
			maxMatched = count1;
			maxCount = count2;
		}
		else
		{
			maxMatched = count2;
			maxCount = count1;
		}

		uint32 maxPossibleWeight = maxMatched * 100 / maxCount;

		if (maxPossibleWeight > wordRaiting.minWeight)
		{
			HArrayVisitor::getWord(tempWord,
				pAllKeysAndValuesRAM[curr].Key,
				Configuration.AutoStemmingOn);

			/*if(strcmp(word2, "êóïèòü") == 0)
			{
			word2[0] = word2[0];
			}*/

			//uint32 minCount = count1 < count2 ? count1 : count2;

			//double maxPossibleWeight = (double)(minCount * 2) / (double)(count1 + count2);

			//if(maxPossibleWeight >= 0.1) //max possible weight
			//{
				//if (curr >= Info.CountWordsHDD)
				//{
				//	sourceFilePosition = 0; //do not read from HDD
				//}

				//in memory + hdd
			double distance = calculateDistance(tempWord,
												maxMatched,
												pLevelBuffer,
												countDocuments, //countTotalDocuments,
												level,
												minPage,
												maxPage,
												pSourceBuffer,
												sourceFilePosition,
												sourceBuffPosition,
												sourceBuffLength);

			wordRaiting.addWord(tempWord, distance * 100);

			//if(distance >= 0.1)
			//{
			//	char buff[100];

			//	sprintf(buff, "%s => %f\r\n", word2, distance);
			//	
			//	//fwrite(buff, strlen(buff), 1, file);

			//	//fflush(file);
			//}
		//}

		//if (curr % 1000 == 0)
		//	printf("\r%d", curr);

		//firstly scan nearest ranges
			if (ascScanning)
			{
				if (curr < stop)
				{
					curr++;
				}
				else
				{
					curr = start;
					ascScanning = false;
				}

			}
			else
			{
				if (curr > 0)
				{
					curr--;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			if (ascScanning)
			{
				curr = start;
				ascScanning = false;
			}
			else
			{
				break;
			}
		}
	}

	//fclose(file);

	delete[] pSourceBuffer;

	delete[] pWeightBuffer;
	delete[] pLevelBuffer;

	//HArrayFixPair::DeleteArray(pKeysAndValuesRAM);
}

double FTSInstance::calculateDistance(const char* word,
	uint32 maxMatched,
	uchar8* pLevelBuffer,
	uint32 countUsedDocNumbers,
	uint32 level,
	uint32 minPage,
	uint32 maxPage,
	uchar8* pSourceBuffer,
	ulong64& sourceFilePosition,
	uint32& sourceBuffPosition,
	uint32& sourceBuffLength)
{
	if (!checkStartedInstance(true))
		return 0;

	uint32 equals = 0;
	uint32 notEquals = 0;

	//mark second words
	calcMatchDocuments(word,
					   pLevelBuffer,
					   level,
					   equals,
					   notEquals,
					   minPage,
					   maxPage,
					   pSourceBuffer,
					   sourceFilePosition,
					   sourceBuffPosition,
					   sourceBuffLength);

	return (double)equals / (double)maxMatched;

	//50 * 2 / 100 + 200

	//return (double)(equals) / (double)(equals + notEquals);
}

void FTSInstance::calculateTrend(const char* phrase,
	uint32 phraseLen,
	uint32* points,
	uint32 count,
	uint32 minPage,
	uint32 maxPage)
{
	if (!checkStartedInstance(true))
		return;

	memset(points, 0, sizeof(uint32) * count);

	//get DocumentsBlock
	HArrayVisitor::getPartWords(phrase,
		phraseLen,
		tempKey,
		Configuration.AutoStemmingOn);

	//Read from RAM
	if (Info.CountWordsRAM)
	{
		uint32 id = haWordsRAM.getValueByKey(tempKey);

		if (id)
		{
			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

			pDocumentsBlock->markTrend(points,
				(maxPage - minPage) / count,
				minPage,
				maxPage);
		}
	}

	uint32 minDocID = getDocHeaderSize();
	uint32 maxDocID = Info.LastNameIDRAM;

	//Read from HDD
	if (Configuration.MemoryMode != IN_MEMORY_MODE
		&& Info.CountWordsHDD)
	{
		ulong64 sourceFilePosition = haWordsHDD.getValueByKey(tempKey);

		if (sourceFilePosition)
		{
			//read docs
			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getTempObject();

			uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];

			uint32 sourceBuffPosition = 0;
			uint32 sourceBuffLength = 0;

			bool isFormatCorrupted = false;
			pDocumentsBlock->readBlocksFromFile(pDocFile,
				pSourceBuffer,
				sourceFilePosition,
				sourceBuffPosition,
				sourceBuffLength,
				MAX_SIZE_BUFFER,
				minDocID,
				maxDocID,
				isFormatCorrupted);
			delete[] pSourceBuffer;

			if (isFormatCorrupted)
			{
				logError("Read corrupted.");

				return;
			}

			pDocumentsBlock->markTrend(points,
				(maxPage - minPage) / count,
				minPage,
				maxPage);

			//release documents
			pDocumentsBlock->clear();
		}
	}
}

void FTSInstance::relevantMatch(Dictionary& dictionary)
{
	if (!checkStartedInstance(true))
		return;

	for (uint32 i = 0; i < dictionary.Count; i++)
	{
		//get DocumentsBlock
		DictionaryWord& dicWord = dictionary.Words[i];

		HArrayVisitor::getPartWords(dicWord.Word,
			strlen(dicWord.Word),
			tempKey,
			Configuration.AutoStemmingOn);

		//Read from RAM
		if (Info.CountWordsRAM)
		{
			uint32 id = haWordsRAM.getValueByKey(tempKey);

			if (id)
			{
				DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

				dicWord.Weight = pDocumentsBlock->CountDocuments;
			}
			else
			{
				dicWord.Weight = 0;
			}
		}

		uint32 minDocID = getDocHeaderSize();
		uint32 maxDocID = Info.LastNameIDRAM;

		//Read from HDD
		if (Configuration.MemoryMode != IN_MEMORY_MODE
			&& Info.CountWordsHDD)
		{
			ulong64 sourceFilePosition = haWordsHDD.getValueByKey(tempKey);

			if (sourceFilePosition)
			{
				//read docs
				DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getTempObject();

				uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];

				uint32 sourceBuffPosition = 0;
				uint32 sourceBuffLength = 0;

				bool isFormatCorrupted = false;
				pDocumentsBlock->readBlocksFromFile(pDocFile,
					pSourceBuffer,
					sourceFilePosition,
					sourceBuffPosition,
					sourceBuffLength,
					MAX_SIZE_BUFFER,
					minDocID,
					maxDocID,
					isFormatCorrupted);
				delete[] pSourceBuffer;

				if (isFormatCorrupted)
				{
					logError("Read corrupted.");

					return;
				}

				dicWord.Weight = pDocumentsBlock->CountDocuments;

				//release documents
				pDocumentsBlock->clear();
			}
			else
			{
				dicWord.Weight = 0;
			}
		}
	}

	dictionary.sortByWeight(true);
}

void FTSInstance::searchMatch(WordRaiting& docRaiting,
	Dictionary& dictionary,
	uint32 count,
	uint32 minPage,
	uint32 maxPage,
	uchar8* excluded)
{
	if (!checkStartedInstance(true))
		return;

	//1. Parse phrase
	//uint32 countResultDocNumbers = 0;

	uint32 countUsedDocNumbers = 0;

	int* pWeightBuffer = new int[Info.LastNameIDRAM];
	uchar8* pLevelBuffer = new uchar8[Info.LastNameIDRAM];

	memset(pWeightBuffer, 0, sizeof(int) * Info.LastNameIDRAM);
	memset(pLevelBuffer, 0, Info.LastNameIDRAM);

	/*
	uint32 level;
	for(level=0; level < count; level++)
	{
		Dictionary& dictionary = dictionaries[level];
	*/

	for (uint32 i = 0; i < dictionary.Count; i++)
	{
		//get DocumentsBlock
		DictionaryWord& dicWord = dictionary.Words[i];

		HArrayVisitor::getPartWords(dicWord.Word,
			strlen(dicWord.Word),
			tempKey,
			Configuration.AutoStemmingOn);

		//Read from RAM
		if (Info.CountWordsRAM)
		{
			uint32 id = haWordsRAM.getValueByKey(tempKey);

			if (id)
			{
				DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

				pDocumentsBlock->markMatchDocuments(pWeightBuffer,
					pLevelBuffer,
					pUsedDocNumbers,
					countUsedDocNumbers,
					dicWord.Weight,
					0,
					minPage,
					maxPage);
			}
		}

		uint32 minDocID = getDocHeaderSize();
		uint32 maxDocID = Info.LastNameIDRAM;

		//Read from HDD
		if (Configuration.MemoryMode != IN_MEMORY_MODE
			&& Info.CountWordsHDD)
		{
			ulong64 sourceFilePosition = haWordsHDD.getValueByKey(tempKey);

			if (sourceFilePosition)
			{
				//read docs
				DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getTempObject();

				uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];

				uint32 sourceBuffPosition = 0;
				uint32 sourceBuffLength = 0;

				bool isFormatCorrupted = false;
				pDocumentsBlock->readBlocksFromFile(pDocFile,
					pSourceBuffer,
					sourceFilePosition,
					sourceBuffPosition,
					sourceBuffLength,
					MAX_SIZE_BUFFER,
					minDocID,
					maxDocID,
					isFormatCorrupted);

				delete[] pSourceBuffer;

				if (isFormatCorrupted)
				{
					logError("Read corrupted.");

					return;
				}

				pDocumentsBlock->markMatchDocuments(pWeightBuffer,
					pLevelBuffer,
					pUsedDocNumbers,
					countUsedDocNumbers,
					dicWord.Weight,
					0,
					minPage,
					maxPage);

				//release documents
				pDocumentsBlock->clear();
			}
		}
	}

	//3. Sort and clear merge buffer
	//uint32 maxPage = 0;
	//int maxWeight = 0;

	for (uint32 i = 0; i < countUsedDocNumbers; i++)
	{
		uint32 usedDocNumber = pUsedDocNumbers[i];

		if (!excluded || !excluded[usedDocNumber])
		{
			//pWeightBuffer[usedDocNumber] > maxWeight &&

			if (pLevelBuffer[usedDocNumber] == 1)
			{
				docRaiting.addWord(usedDocNumber, pWeightBuffer[usedDocNumber]);

				//maxPage = usedDocNumber;
				//maxWeight = pWeightBuffer[usedDocNumber];
			}

			pWeightBuffer[usedDocNumber] = 0;
			pLevelBuffer[usedDocNumber] = 0;
		}
	}

	////4. Relevant result
	//RelevantResult* pResult = pRelevantResultPool->newObject();
	//
	//if(maxWeight)
	//{
	//	pResult->Matches[0] = maxPage;
	//	pResult->CountMatches = 1;
	//}

	/*pResult->MatchPositions[0] = 0;
		pResult->CountMatches = 1;

	pResult->Matches[0] = maxPage;*/

	delete[] pWeightBuffer;
	delete[] pLevelBuffer;
}

RelevantResult* FTSInstance::searchPhrase(const char* phrase,
	uint32 phraseLen,
	const char* templateName,
	uint32 templateNameLen,
	uint32 minPage,
	uint32 maxPage,
	uint32 skip)
{
	if (!checkStartedInstance(true))
		return 0;

	RelevantResult* pResult = pRelevantResultPool->newObject();
	pResult->CountMatches = 0;
	pResult->FullCountMatches = 0;

	//1. Parse phrase
	uint32 minCountPages = MAX_INT;

	DocumentsBlock* pDocumentsBlocks[MAX_WORDS_IN_QUERY];
	//DocumentsBlock* pDocumentsBlocksToRelease[MAX_WORDS_IN_QUERY];

	uint32 countWords = 0;
	uint32 countWordsToRelease = 0;

	char lowerPhrase[1024];

	uint32 currLen = 0;

	char* prevWord = 0;
	uint32 prevCurrLen = 0;

	uchar8 c = 0;

	uint32 minDocID = getDocHeaderSize();
	uint32 maxDocID = Info.LastNameIDRAM;

	for (uint32 i = 0; i <= phraseLen; i++)
	{
		if (i < phraseLen)
		{
			uchar8 c = phrase[i];

			if ((lowerPhrase[i] = alphabet[c]) ||
				c == (uchar8)'%')
			{
				currLen++;

				continue;
			}
		}

		if (currLen > 0)
		{
			if (currLen >= Configuration.MinLenWord && currLen <= Configuration.MaxLenWord)
			{
				if (countWords >= MAX_WORDS_IN_QUERY)
				{
					logError("Error: Max words in query %d.", MAX_WORDS_IN_QUERY);
					return 0;
				}

				//get DocumentsBlock
				char* word = lowerPhrase + i - currLen;

				char phraseTemp[258];

				if (Info.CountWordInPhrase == 2)
				{
					if ((word - prevWord) == prevCurrLen + 1)
					{
						if (prevCurrLen > 8)
						{
							uint32 i = 0;

							for (; i < 8; i++)
							{
								phraseTemp[i] = prevWord[i];
							}

							phraseTemp[i++] = ' ';

							for (uint32 j = 0; j < currLen; i++, j++)
							{
								phraseTemp[i] = word[j];
							}

							HArrayVisitor::getPartWords(phraseTemp,
								i,
								tempKey,
								Configuration.AutoStemmingOn);
						}
						else
						{
							uint32 phraseLen = prevCurrLen + currLen + 1;

							prevWord[prevCurrLen] = ' ';

							HArrayVisitor::getPartWords(prevWord,
								phraseLen,
								tempKey,
								Configuration.AutoStemmingOn);
						}
					}
					else
					{
						tempKey[0] = 0;
					}
				}
				else
				{
					HArrayVisitor::getPartWords(word,
						currLen,
						tempKey,
						Configuration.AutoStemmingOn);
				}

				if (tempKey[0])
				{
					bool bFindWord = false;

					//Read from RAM
					if (Info.CountWordsRAM)
					{
						//read docs
						uint32 id = haWordsRAM.getValueByKey(tempKey);

						//add docs
						if (id)
						{
							DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

							if (pDocumentsBlock->CountDocuments < minCountPages)
							{
								if (countWords)
								{
									pDocumentsBlocks[countWords++] = pDocumentsBlocks[0]; //first element always minimum
									pDocumentsBlocks[0] = pDocumentsBlock;
								}
								else
								{
									pDocumentsBlocks[countWords++] = pDocumentsBlock;
								}

								minCountPages = pDocumentsBlock->CountDocuments;
							}
							else
							{
								pDocumentsBlocks[countWords++] = pDocumentsBlock;
							}

							bFindWord = true;
						}
					}

					//Read from HDD
					if (Configuration.MemoryMode != IN_MEMORY_MODE
						&& Info.CountWordsHDD)
					{
						ulong64 sourceFilePosition = haWordsHDD.getValueByKey(tempKey);

						if (sourceFilePosition)
						{
							//read docs
							uint32 id;
							DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->newObject(id);

							uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];

							uint32 sourceBuffPosition = 0;
							uint32 sourceBuffLength = 0;

							bool isFormatCorrupted = false;
							pDocumentsBlock->readBlocksFromFile(pDocFile,
								pSourceBuffer,
								sourceFilePosition,
								sourceBuffPosition,
								sourceBuffLength,
								MAX_SIZE_BUFFER,
								minDocID,
								maxDocID,
								isFormatCorrupted);

							delete[] pSourceBuffer;

							if (isFormatCorrupted)
							{
								logError("Read corrupted.");

								return 0;
							}

							//add docs
							if (pDocumentsBlock->CountDocuments < minCountPages)
							{
								if (countWords)
								{
									pDocumentsBlocks[countWords++] = pDocumentsBlocks[0]; //first element always minimum
									pDocumentsBlocks[0] = pDocumentsBlock;
								}
								else
								{
									pDocumentsBlocks[countWords++] = pDocumentsBlock;
								}

								minCountPages = pDocumentsBlock->CountDocuments;
							}
							else
							{
								pDocumentsBlocks[countWords++] = pDocumentsBlock;
							}

							//release later
							//pDocumentsBlocksToRelease[countWordsToRelease] = pDocumentsBlock;

							bFindWord = true;
						}
					}

					//word in phrase is not found, return
					if (!bFindWord)
					{
						goto RETURN_RESULT;
					}
				}

				prevWord = word;
				prevCurrLen = currLen;
			}

			currLen = 0;
		}
	}

	//2. Search phrase
	uint32 countResultDocNumbers = 0;

	if (countWords == 1) //one word
	{
		pDocumentsBlocks[0]->markOneDocuments(pResultDocNumbers,
			countResultDocNumbers,
			minPage,
			maxPage);
	}
	else if (countWords == 2) //two words
	{
		uint32 countUsedDocNumbers = 0;
		uint32 minDocNumber = 0;
		uint32 maxDocNumber = 0;

		pDocumentsBlocks[0]->markFirstDocuments(pBuffer,
			pUsedDocNumbers,
			countUsedDocNumbers,
			minDocNumber,
			maxDocNumber,
			minPage,
			maxPage);

		pDocumentsBlocks[1]->markLastDocuments(pBuffer,
			pResultDocNumbers,
			countResultDocNumbers,
			minDocNumber,
			maxDocNumber,
			2,
			minPage,
			maxPage);

		//clear merge buffer
		for (uint32 i = 0; i < countUsedDocNumbers; i++)
		{
			pBuffer[pUsedDocNumbers[i]] = 0;
		}
	}
	else if (countWords > 0) //many words
	{
		uint32 countUsedDocNumbers = 0;
		uint32 minDocNumber = 0;
		uint32 maxDocNumber = 0;

		pDocumentsBlocks[0]->markFirstDocuments(pBuffer,
			pUsedDocNumbers,
			countUsedDocNumbers,
			minDocNumber,
			maxDocNumber,
			minPage,
			maxPage);

		uint32 count = countWords - 1;
		for (uint32 j = 1; j < count; j++)
		{
			pDocumentsBlocks[j]->markNextDocuments(pBuffer,
				minDocNumber,
				maxDocNumber,
				minPage,
				maxPage);
		}

		pDocumentsBlocks[count]->markLastDocuments(pBuffer,
			pResultDocNumbers,
			countResultDocNumbers,
			minDocNumber,
			maxDocNumber,
			countWords,
			minPage,
			maxPage);

		//clear merge buffer
		for (uint32 i = 0; i < countUsedDocNumbers; i++)
		{
			pBuffer[pUsedDocNumbers[i]] = 0;
		}
	}

	//3. Relevant result
	//pResult->FullCountMatches = countResultDocNumbers;

	/*if(countResultDocNumbers > MAX_FOUND_DOCUMENTS_IN_QUERY)
	{
		countResultDocNumbers = MAX_FOUND_DOCUMENTS_IN_QUERY;
	}*/

	if (Info.RelevantLevel)
	{
		//NOT IMPLEMENTED

		/*if(countResultDocNumbers)
		{
			pResult->Matches[pResult->CountMatches++] =  (pResultDocNumbers[0] >> Info.RelevantLevel);
		}

		for(int i = 1; i < countResultDocNumbers; i++)
		{
			uint32 id = (pResultDocNumbers[i] >> Info.RelevantLevel);

			if(id != pResult->Matches[pResult->CountMatches - 1])
			{
				pResult->Matches[pResult->CountMatches++] = id;
			}
		}*/
	}
	else
	{
		char* pPrevName = new char[Info.DocumentNameSize];
		char* pName = 0;

		for (int i = 0; i < countResultDocNumbers; i++)
		{
			uint32 id = pResultDocNumbers[i];

			pName = pResult->Matches[pResult->CountMatches];

			getDocumentNameByID(id, pName, Info.DocumentNameSize);

			if (templateNameLen == 0 || wildCardCompare(templateName, pName))
			{
				if (pPrevName)
				{
					if (strcmp(pPrevName, pName)) //group if items has the same name
					{
						if (!skip)
						{
							pResult->CountMatches++;
						}
						else
						{
							skip--;
						}

						pResult->FullCountMatches++;
					}
				}
				else
				{
					if (!skip)
					{
						pResult->CountMatches++;
					}
					else
					{
						skip--;
					}

					pResult->FullCountMatches++;
				}

				strcpy(pPrevName, pName);

				if (pResult->CountMatches >= MAX_FOUND_DOCUMENTS_IN_QUERY)
				{
					break;
				}
			}
		}

		delete[] pPrevName;

		//WordRaiting docRaiting(MAX_FOUND_DOCUMENTS_IN_QUERY); //top 25 words

		//uint32 weight = 0; //by default

		//uint32 prevDocNumber = MAX_INT;

		//for(int i = countResultDocNumbers - 1; i >= 0; i--)
		//{
		//	uint32 currDocNumber = pResultDocNumbers[i];

		//	//minus one on next doc
		//	uint32 delta = prevDocNumber - currDocNumber;
		//	
		//	if(delta < weight)
		//	{
		//		weight -= delta;
		//		weight += 1000;
		//	}
		//	else
		//	{
		//		weight = 1000;
		//	}

		//	//rate
		//	docRaiting.addWord(currDocNumber, weight);

		//	//save prev
		//	prevDocNumber = currDocNumber;
		//}

		//docRaiting.dictionary.sortByWeight(false);

		////create result
		//pResult->MatchWords[0] = 0;

		//for(uint32 i=0; i < docRaiting.dictionary.Count; i++)
		//{
		//	pResult->Matches[i] = docRaiting.dictionary.Words[i].DocID;
		//	pResult->CountMatches++;
		//}
	}

RETURN_RESULT:

	//release documents
	pDocumentsBlockPool->releaseObjects();

	return pResult;
}

RelevantResult* FTSInstance::searchHtmlSeldomWords(char* text,
	uint32 textLen)
{
	if (!checkStartedInstance(true))
		return 0;

	WordRaiting wr(10);

	bool isText = false;
	uint32 currLen = 0;

	//calculate relevant parameters
	uint32 docID = Info.LastNameIDRAM;
	uint32 relevantDocID;
	uint32 relevantBlockSize;

	if (Info.RelevantLevel)
	{
		relevantDocID = (docID << Info.RelevantLevel);
		relevantBlockSize = (textLen >> Info.RelevantLevel) + 1;

		if (relevantBlockSize < MIN_RELEVANT_BLOCK)
		{
			relevantBlockSize = MIN_RELEVANT_BLOCK;
		}
	}

	//body document
	for (uint32 i = 0; i < textLen; i++)
	{
		uchar8 c = text[i];
		if (c == '>')
		{
			isText = true;
			continue;
		}

		if (isText)
		{
			if (text[i] = alphabet[c])
			{
				currLen++;
			}
			else if (currLen > 0)
			{
				if (currLen >= Configuration.MinLenWord && currLen <= Configuration.MaxLenWord)
				{
					uint32 position = i - currLen;
					char* word = text + position;

					//word, currLen;
					HArrayVisitor::getPartWords(word,
						currLen,
						tempKey,
						Configuration.AutoStemmingOn);

					//Read from RAM
					if (Info.CountWordsRAM)
					{
						uint32 id = haWordsRAM.getValueByKey(tempKey);

						if (id)
						{
							DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

							wr.addWord(word, Info.LastNameIDRAM - pDocumentsBlock->CountDocuments);
						}
					}
				}

				currLen = 0;
			}

			if (c == '<')
			{
				isText = false;
			}
		}
	}

	wr.dictionary.sortByWeight(false);

	RelevantResult* pResult = pRelevantResultPool->newObject();
	pResult->CountMatches = 0;
	pResult->FullCountMatches = 0;

	for (uint32 i = 0; i < wr.dictionary.Count; i++)
	{
		strcat(pResult->MatchWords, wr.dictionary.Words[i].Word);
		strcat(pResult->MatchWords, " ");
	}

	return pResult;
}

RelevantResult* FTSInstance::searchPhraseRel(const char* phrase,
	uint32 phraseLen,
	uint32 minPage,
	uint32 maxPage)
{
	if (!checkStartedInstance(true))
		return 0;

	RelevantResult* pResult = pRelevantResultPool->newObject();
	pResult->CountMatches = 0;
	pResult->FullCountMatches = 0;

	Dictionary dics[MAX_WORDS_IN_QUERY]; //max words
	uint32 countWords = 0;

	//1. Parse phrase
	char lowerPhrase[1024];

	uint32 currLen = 0;

	uchar8 c = 0;

	for (uint32 i = 0; i <= phraseLen; i++)
	{
		if (i < phraseLen)
		{
			uchar8 c = phrase[i];

			if ((lowerPhrase[i] = alphabet[c]) ||
				c == (uchar8)'%')
			{
				currLen++;

				continue;
			}
		}

		if (currLen > 0)
		{
			if (currLen >= Configuration.MinLenWord && currLen <= Configuration.MaxLenWord)
			{
				if (countWords >= MAX_WORDS_IN_QUERY)
				{
					logError("Error: Max words in query %d.", MAX_WORDS_IN_QUERY);
					return 0;
				}

				//get DocumentsBlock
				char* word = lowerPhrase + i - currLen;

				dics[countWords++].addWord(word, currLen, 1);
			}

			currLen = 0;
		}
	}

	//2. Search distances
	//for(uint32 i=0; i<countWords; i++)
	//{
	WordRaiting wordRaiting(25); //top 25 words

	searchDistances(wordRaiting,
		dics,
		countWords,
		minPage,
		maxPage);
	//wr.print();

	wordRaiting.dictionary.sortByWeight(false);

	//create list
	for (uint32 i = 0; i < wordRaiting.dictionary.Count; i++)
	{
		strcat(pResult->MatchWords, wordRaiting.dictionary.Words[i].Word);
		strcat(pResult->MatchWords, " ");
	}
	//}

	//3. Search matches
	WordRaiting docRaiting(MAX_FOUND_DOCUMENTS_IN_QUERY);

	searchMatch(docRaiting,
		wordRaiting.dictionary,
		countWords,
		minPage,
		maxPage,
		0);

	docRaiting.dictionary.sortByWeight(false);

	//4. Fill the result
	/*for(uint32 i=0; i < wr.dictionary.Count; i++)
	{
		strcat(pResult->MatchWords, wr.dictionary.Words[i].Word);
		strcat(pResult->MatchWords, " ");
	}*/

	for (uint32 i = 0; i < docRaiting.dictionary.Count; i++)
	{
		getDocumentNameByID(docRaiting.dictionary.Words[i].DocID, pResult->Matches[i], Info.DocumentNameSize);

		pResult->CountMatches++;
	}

	return pResult;
}

QueryResult* FTSInstance::searchQuery(Selector** selectors,
	uint32 countSelectors,
	uint32 minPage,
	uint32 maxPage,
	uint32 skip,
	bool agregateBySubject)
{
	if (!checkStartedInstance(true))
		return 0;

	//1. Parse phrase
	uint32 countUsedDocNumbers = 0;
	uint32 countFilterSelectors = 0;
	uint32 countColumnSelectors = 0;
	uint32 countSortSelectors = 0;
	bool isSortAsc = false;
	bool needFullScan = false;
	bool maxPages = false;

	PostSelector::CountWords = 0;

	PostSelector* pPostSelectorBuffer = new PostSelector[Info.LastNameIDRAM];
	memset(pPostSelectorBuffer, 0, sizeof(PostSelector) * Info.LastNameIDRAM);

	for (uint32 i = 0; i < countSelectors; i++)
	{
		Selector* pSelector = selectors[i];
		pSelector->init(alphabet, Configuration.MinLenWord, Configuration.MaxLenWord);

		if (pSelector->SelectorType == 1 || pSelector->SelectorType == 3) //amount filters
		{
			countFilterSelectors++;
		}
		else if (pSelector->SelectorType == 2 || pSelector->SelectorType == 3) //amount sortings
		{
			countColumnSelectors++;
		}

		if (pSelector->AgregationSortType) //amount sortings
		{
			isSortAsc = pSelector->IsSortAsc;

			if (pSelector->AgregationSortType == 1 ||
				pSelector->AgregationSortType == 2 ||
				pSelector->AgregationSortType == 3)
			{
				needFullScan = true;
			}

			if (pSelector->AgregationSortType == 5)
			{
				needFullScan = true;
				maxPages = true;
			}

			countSortSelectors++;
		}

		//work only for ram !!!
		if (Info.CountWordsRAM && pSelector->RangeType)
		{
			pSelector->AutoStemmingOn = Configuration.AutoStemmingOn;
			haWordsRAM.scanByVisitor(pSelector);
		}

		uint32 minDocID = getDocHeaderSize();
		uint32 maxDocID = Info.LastNameIDRAM;

		for (uint32 j = 0; j < pSelector->pDictionary->Count; j++)
		{
			//get DocumentsBlock
			DictionaryWord& dicWord = pSelector->pDictionary->Words[j];

			HArrayVisitor::getPartWords(dicWord.Word,
				strlen(dicWord.Word),
				tempKey,
				Configuration.AutoStemmingOn);

			//get word id
			uint32 wordId = PostSelector::addSelectorWord(pSelector, dicWord.Word);

			//Read from HDD
			if (Configuration.MemoryMode != IN_MEMORY_MODE
				&& Info.CountWordsHDD)
			{
				ulong64 sourceFilePosition = haWordsHDD.getValueByKey(tempKey);

				if (sourceFilePosition)
				{
					//read docs
					DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getTempObject();

					uchar8* pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];

					uint32 sourceBuffPosition = 0;
					uint32 sourceBuffLength = 0;

					bool isFormatCorrupted = false;
					pDocumentsBlock->readBlocksFromFile(pDocFile,
						pSourceBuffer,
						sourceFilePosition,
						sourceBuffPosition,
						sourceBuffLength,
						MAX_SIZE_BUFFER,
						minDocID,
						maxDocID,
						isFormatCorrupted);

					delete[] pSourceBuffer;

					if (isFormatCorrupted)
					{
						logError("Read corrupted.");

						return 0;
					}

					//mark documents
					pDocumentsBlock->markSelectorDocuments(pPostSelectorBuffer,
						pUsedDocNumbers,
						countUsedDocNumbers,
						wordId,
						minPage,
						maxPage);

					//release documents
					pDocumentsBlock->clear();

				}
			}

			//Read from RAM
			if (Info.CountWordsRAM)
			{
				uint32 id = haWordsRAM.getValueByKey(tempKey);

				if (id)
				{
					DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

					pDocumentsBlock->markSelectorDocuments(pPostSelectorBuffer,
						pUsedDocNumbers,
						countUsedDocNumbers,
						wordId,
						minPage,
						maxPage);
				}
			}
		}
	}

	//3. Sort and clear merge buffer

	char* wordsInSelector[256];
	uint32 countWordsInSelector = 0;

	Selector* pSelector;
	Selector* pPrevSelector = 0;

	QueryRow queryRow;

	QueryResult* pQueryResult;

	if (countUsedDocNumbers)
	{
		if (!maxPages)
		{
			pQueryResult = new QueryResult(skip + MAX_FOUND_DOCUMENTS_IN_QUERY);
		}
		else
		{
			pQueryResult = new QueryResult(skip + 30000); //max pages
		}
	}
	else
	{
		pQueryResult = new QueryResult(MAX_FOUND_DOCUMENTS_IN_QUERY);
		return pQueryResult;
	}

	uint32 curr = 0;
	uint32 end = countUsedDocNumbers - 1;
	int step = 1;

	//reverse
	if (countSortSelectors && !isSortAsc && !needFullScan)
	{
		curr = countUsedDocNumbers - 1;
		end = 0;
		step = -1;
	}

	/*char docNames[MAX_FOUND_DOCUMENTS_IN_QUERY][64];
	char docName[64];*/

	while (true)
	{
		uint32 usedDocNumber = pUsedDocNumbers[curr];

		////check dublicates
		//this->getDocumentNameByID(usedDocNumber, docName, 64);

		//for(uint32 i = 0; i < pQueryResult->CountRows; i++)
		//{
		//	if(strcmp(docNames[i], docName) == 0) //equal
		//	{
		//		goto NEXT_ITERATION;
		//	}
		//}
		//
		////copy
		//strcpy(docNames[pQueryResult->CountRows], docName);

		PostSelector* pPostSelector = &pPostSelectorBuffer[usedDocNumber];

		//init row
		memset(&queryRow, 0, sizeof(QueryRow));
		queryRow.DocNumber = usedDocNumber;

		this->getDocumentNameByID(usedDocNumber,
			queryRow.DocName,
			Configuration.WordsHeaderBase);

		/*if(strcmp(queryRow.DocName, "sql-1161120-13") == 0)
		{
			usedDocNumber = usedDocNumber;
		}*/

		pPrevSelector = 0;

		//get selectors
		while (true)
		{
			//first word ==============================================
			uint32 wordId = pPostSelector->WordID[0];

			if (!wordId) //realy need condition ???
			{
				//handled!!!
				pPrevSelector->handleResult(queryRow,
					wordsInSelector,
					countWordsInSelector);

				countWordsInSelector = 0;

				break;
			}

			pSelector = PostSelector::Selectors[wordId];

			if (pPrevSelector != pSelector && pPrevSelector)
			{
				//handled!!!
				pPrevSelector->handleResult(queryRow,
					wordsInSelector,
					countWordsInSelector);

				countWordsInSelector = 0;

				if (queryRow.IsExcluded) //row dont need
				{
					goto NEXT_ITERATION;
				}
			}

			pPrevSelector = pSelector;

			wordsInSelector[countWordsInSelector++] = PostSelector::Words[wordId];

			//second word =============================================
			wordId = pPostSelector->WordID[1];

			if (!wordId)
			{
				//handled!!!

				pPrevSelector->handleResult(queryRow,
					wordsInSelector,
					countWordsInSelector);

				countWordsInSelector = 0;

				break;
			}

			pSelector = PostSelector::Selectors[wordId];

			if (pPrevSelector != pSelector && pPrevSelector)
			{
				//handled!!!

				pPrevSelector->handleResult(queryRow,
					wordsInSelector,
					countWordsInSelector);

				countWordsInSelector = 0;

				if (queryRow.IsExcluded) //row dont need
				{
					goto NEXT_ITERATION;
				}
			}

			pPrevSelector = pSelector;

			wordsInSelector[countWordsInSelector++] = PostSelector::Words[wordId];

			//next post selector ======================================
			pPostSelector = pPostSelector->pNextPostSelector;

			if (!pPostSelector)
			{
				//handled!!!
				pPrevSelector->handleResult(queryRow,
					wordsInSelector,
					countWordsInSelector);

				countWordsInSelector = 0;

				break;
			}
		}

		//row need ?
		if (!queryRow.IsExcluded &&
			queryRow.CountFilterSelectors == countFilterSelectors)
		{
			//accepted
			pQueryResult->addQueryRow(queryRow, agregateBySubject);

			//if no sorting and result is full then finish
			if (!needFullScan &&
				pQueryResult->CountRows >= pQueryResult->Size)
			{
				break;
			}
		}

	NEXT_ITERATION:

		//clear
		//pPostSelectorPool->releasePostSelectorList(pPostSelectorBuffer[usedDocNumber]);

		//iterate
		if (curr == end)
		{
			break;
		}

		curr += step;
	}

	//clear pool
	pPostSelectorPool->releaseObjects();

	delete[] pPostSelectorBuffer;

	//4. Sorting
	if (!skip && !maxPages)
	{
		if (countSortSelectors)
		{
			pQueryResult->sort(isSortAsc); //descending
		}

		return pQueryResult;
	}
	else
	{
		//get only last 25
		if (countSortSelectors)
		{
			pQueryResult->sort(isSortAsc); //descending
		}

		QueryResult* pQueryResultNew = new QueryResult(MAX_FOUND_DOCUMENTS_IN_QUERY);

		uint32 count = (skip + MAX_FOUND_DOCUMENTS_IN_QUERY);

		if (count > pQueryResult->CountRows)
		{
			count = pQueryResult->CountRows;
		}

		for (uint32 i = skip; i < count; i++)
		{
			pQueryResultNew->addQueryRow(pQueryResult->QueryRows[i]);
		}

		delete pQueryResult;

		return pQueryResultNew;
	}
}

void FTSInstance::extractDocument(uint32 docNumber,
	char* docName,
	char* docText)
{
	if (!checkStartedInstance(true))
		return;

	if (!pAllKeysAndValuesRAM)
	{
		uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;

		pAllKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);

		//save haWordsRAM
		haWordsRAM.getKeysAndValuesByRange(pAllKeysAndValuesRAM,
			Info.CountWordsRAM,
			0,
			0);
	}

	char tempWord[16];

	//search distances
	for (uint32 i = 0; i < Info.CountWordsRAM; i++)
	{
		uint32 id = pAllKeysAndValuesRAM[i].Value;

		DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

		if (pDocumentsBlock->hasDocNumber(docNumber))
		{
			HArrayVisitor::getWord(tempWord,
				pAllKeysAndValuesRAM[i].Key,
				Configuration.AutoStemmingOn);

			//add word
			strcat(docText, tempWord);
			strcat(docText, " ");
		}
	}
}

RelevantResult* FTSInstance::searchNewMems(uint32 startDate1Year,
	uint32 startDate1Month,
	uint32 startDate2Year,
	uint32 startDate2Month,
	uint32 startDate2Day,
	uint32 endDate2Year,
	uint32 endDate2Month,
	uint32 endDate2Day,
	uint32 periodInDays)
{
	if (!checkStartedInstance(true))
		return 0;

	uint32 minSites = 2;
	uint32 minWordsOnSite = 2;

	RelevantResult* pResult;

	char tempWord[16];

	char phrase[64];

	WordRaiting wordRaiting(MAX_FOUND_DOCUMENTS_IN_QUERY);

	if (pAllKeysAndValuesRAM)
	{
		HArrayFixPair::DeleteArray(pAllKeysAndValuesRAM);
	}

	//always reload new words
	uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;

	pAllKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);
	pAllKeysAndValuesRAMCount = Info.CountWordsRAM;

	//save haWordsRAM
	haWordsRAM.getKeysAndValuesByRange(pAllKeysAndValuesRAM,
		Info.CountWordsRAM,
		0,
		0);

	//minRange1
	sprintf(phrase, "y%04d m%02d", startDate1Year, startDate1Month);
	pResult = searchPhrase(phrase, strlen(phrase), "", 0, 80, Info.LastNameIDRAM, 0);

	uint32 minRange1 = 0;

	if (pResult->CountMatches)
	{
		//NOT IMPLEMENTED
		//minRange1 = pResult->Matches[pResult->CountMatches - 1];
	}

	this->releaseRelevantResult(pResult);

	//minRange2
	sprintf(phrase, "y%04d m%02d d%02d", startDate2Year, startDate2Month, startDate2Day);
	pResult = searchPhrase(phrase, strlen(phrase), "", 0, minRange1 + 1, Info.LastNameIDRAM, 0);

	uint32 minRange2 = 0;

	if (pResult->CountMatches)
	{
		//NOT IMPLEMENTED
		//minRange2 = pResult->Matches[pResult->CountMatches - 1];
	}

	this->releaseRelevantResult(pResult);

	//maxRange2
	sprintf(phrase, "y%04d m%02d d%02d", endDate2Year, endDate2Month, endDate2Day);
	pResult = searchPhrase(phrase, strlen(phrase), "", 0, minRange2 + 1, Info.LastNameIDRAM, 0);

	uint32 maxRange2 = 0;

	if (pResult->CountMatches)
	{
		//NOT IMPLEMENTED
		//maxRange2 = pResult->Matches[pResult->CountMatches - 1];
	}

	this->releaseRelevantResult(pResult);

	if (minRange1 == 0 || minRange2 == 0 || maxRange2 == 0)
	{
		return 0;
	}

	//get sites
	Dictionary sites;
	sites.addWord("ssearch", 0);
	sites.addWord("sdou", 0);
	sites.addWord("sgame", 0);
	sites.addWord("shabr", 0);
	sites.addWord("sgotai", 0);
	sites.addWord("ssql", 0);
	sites.addWord("ssevas", 0);
	sites.addWord("sskc", 0);
	sites.addWord("svelo", 0);
	sites.addWord("spravda", 0);
	sites.addWord("sixbt", 0);

	//search distances
	for (uint32 i = 0; i < Info.CountWordsRAM; i++)
	{
		if (i % 1000 == 0)
		{
			printf("\r=>%d", i);
		}

		uint32 id = pAllKeysAndValuesRAM[i].Value;

		DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

		HArrayVisitor::getWord(tempWord,
			pAllKeysAndValuesRAM[i].Key,
			Configuration.AutoStemmingOn);

		if (strcmp(tempWord, "йовбак") == 0)
		{
			tempWord[0] = tempWord[0];
		}

		if (('а' <= tempWord[0] && tempWord[0] <= 'я')) //exclude numbers
		{
			//nothing			
		}
		else
		{
			continue;
		}

		uint32 onMonth = 0;
		uint32 onToday = 0;

		pDocumentsBlock->calcCountInRanges(minRange1,
			minRange2 - 1,
			minRange2,
			maxRange2 - 1,
			onMonth,
			onToday);

		//sixbt
		//sprintf(phrase, "y2015 m07 d16 %s", tempWord);
		//pResult = searchPhrase(phrase, strlen(phrase), 80, Info.LastNameIDRAM);

		//uint32 onToday = pResult->FullCountMatches;

		if (onToday >= minSites) //more than three times
		{
			if ((onMonth * 30) < (onToday * 30 / periodInDays))
			{
				uint32 count = 0;

				for (uint32 j = 0; j < sites.Count; j++)
				{
					//ssql
					sprintf(phrase, "%s %s", sites.Words[j].Word, tempWord);
					pResult = searchPhrase(phrase, strlen(phrase), "", 0, minRange2, maxRange2 - 1, 0);

					if (pResult->CountMatches >= minWordsOnSite)
					{
						count++;
					}

					releaseRelevantResult(pResult);
				}

				if (count >= minSites)
				{
					if (onMonth)
					{
						wordRaiting.addWord(tempWord, (onToday * 30 / periodInDays) * (count - minSites + 1) / onMonth);
					}
					else
					{
						wordRaiting.addWord(tempWord, (onToday * 30 / periodInDays) * (count - minSites + 1) * 10);
					}
				}

				//if(pRelevantResult->CountMatches > 0)
				//	count++;

				//2. Word today should be mentions on several sites

				//3. Word today should be mentions many times

				//4. Word in month should be mentions very rare

				//5. Calculate freq and add to result
			}
		}
	}

	//3. Search matches
	pResult = pRelevantResultPool->newObject();

	wordRaiting.dictionary.sortByWeight(false);

	pResult->CountMatches = 0;
	pResult->FullCountMatches = 0;

	for (uint32 i = 0; i < wordRaiting.dictionary.Count; i++)
	{
		strcat(pResult->MatchWords, wordRaiting.dictionary.Words[i].Word);
		strcat(pResult->MatchWords, " ");

		pResult->CountMatches++;
	}

	return pResult;
}
