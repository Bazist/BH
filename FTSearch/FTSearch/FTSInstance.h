#include "stdafx.h"
#include "DocumentsBlock.h"
#include "DocumentsBlock.h"
#include "HArrayFixRAM.h"
#include "HArrayFixHDD.h"
#include "DocumentsBlockPool.h"
#include "RelevantResultPool.h"
#include "BinaryFile.h"
#include "FTSConfiguration.h"
#include "FTSInstanceInfo.h"
#include "RelevantResult.h"
#include "DocumentsNameTextFile.h"
#include "DocumentsDict.h"
#include "Dictionary.h"
#include "WordRaiting.h"
#include "Selector.h"
#include "QueryResult.h"

class FTSInstance
{
public:
	FTSInstance()
	{
		memset(this, 0, sizeof(FTSInstance));
	}

	static void initSharedResources()
	{
		pBuffer = new uchar8[MAX_SIZE_BUFFER];
		memset(pBuffer, 0, MAX_SIZE_BUFFER);

		pUsedDocNumbers = new uint32[MAX_SIZE_BUFFER];
		pResultDocNumbers = new uint32[MAX_SIZE_BUFFER];

		pBlockMemoryPool = new BlockMemoryPool(LastErrorMessage);
		pPostSelectorPool = new PostSelectorPool(LastErrorMessage);
		pDocumentsBlockPool = new DocumentsBlockPool(pBlockMemoryPool, pPostSelectorPool, LastErrorMessage);
		pRelevantResultPool = new RelevantResultPool(LastErrorMessage);
	}

	static void destroySharedResources()
	{
		if (pBuffer)
		{
			delete[] pBuffer;
			pBuffer = 0;
		}

		if (pUsedDocNumbers)
		{
			delete[] pUsedDocNumbers;
			pUsedDocNumbers = 0;
		}

		if (pResultDocNumbers)
		{
			delete[] pResultDocNumbers;
			pResultDocNumbers = 0;
		}

		if (pBlockMemoryPool)
		{
			delete pBlockMemoryPool;
			pBlockMemoryPool = 0;
		}

		if (pDocumentsBlockPool)
		{
			delete pDocumentsBlockPool;
			pDocumentsBlockPool = 0;
		}

		if (pRelevantResultPool)
		{
			delete pRelevantResultPool;
			pRelevantResultPool = 0;
		}

		if (pPostSelectorPool)
		{
			delete pPostSelectorPool;
			pPostSelectorPool = 0;
		}
	}

	void startInstance(FTSConfiguration& configuration)
	{
		startInstance(configuration, false);
	}

	void startInstance(FTSConfiguration& configuration, bool onlyCheckIndex)
	{
		if (!checkStartedInstance(false))
			return;

		if (strlen(Configuration.IndexPath) > 0)
		{
			//destroy previous instance
			destroy();
		}

		Configuration = configuration;

		//recovery
		recoveryIndex();

		init();

		openOrCreateIndex(onlyCheckIndex);
	}

	uint32 getSearchPoolsMemory();
	void logFile(const char* text);

	void searchMatch(WordRaiting& wordRaiting,
		Dictionary& dictionary,
		uint32 count,
		uint32 minPage,
		uint32 maxPage,
		uchar8* excluded);

	void relevantMatch(Dictionary& dictionary);

	RelevantResult* searchPhrase(const char* phrase)
	{
		return searchPhrase(phrase, strlen(phrase), 0, 0, 0, MAX_INT, 0);
	}

	RelevantResult* searchPhrase(const char* phrase,
		uint32 phraseLen,
		const char* templateName,
		uint32 templateNameLen,
		uint32 minPage,
		uint32 maxPage,
		uint32 skip);

	RelevantResult* searchHtmlSeldomWords(char* text,
		uint32 textLen);

	RelevantResult* searchPhraseRel(const char* phrase,
		uint32 phraseLen,
		uint32 minPage,
		uint32 maxPage);

	QueryResult* searchQuery(Selector** selectors,
		uint32 countSelectors,
		uint32 minPage,
		uint32 maxPage,
		uint32 skip,
		bool agregateBySubject);

	void extractDocument(uint32 docNumber,
		char* docName,
		char* docText);

	RelevantResult* searchNewMems(uint32 startDate1Year,
		uint32 startDate1Month,
		uint32 startDate2Year,
		uint32 startDate2Month,
		uint32 startDate2Day,
		uint32 endDate2Year,
		uint32 endDate2Month,
		uint32 endDate2Day,
		uint32 periodInDays);

	void calculateTrend(const char* phrase,
		uint32 phraseLen,
		uint32* points,
		uint32 count,
		uint32 minPage,
		uint32 maxPage);

	double calculateDistance(const char* word,
		uchar8* pLevelBuffer,
		uint32 countUsedDocNumbers,
		uint32 level,
		uint32 minPage,
		uint32 maxPage,
		uchar8* pSourceBuffer,
		ulong64& sourceFilePosition,
		uint32& sourceBuffPosition,
		uint32& sourceBuffLength);

	void FTSInstance::markMatchDocuments(const char* word,
		int* pWeightBuffer,
		uchar8* pLevelBuffer,
		uint32 level,
		uint32& countUsedDocNumbers,
		uint32& countTotalDocuments,
		uint32 minPage,
		uint32 maxPage);

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
		uint32& sourceBuffLength);

	void FTSInstance::searchDistances(WordRaiting& wordRaiting,
		Dictionary* dics,
		uint32 count,
		uint32 minPage,
		uint32 maxPage);

	void FTSInstance::buildWiki();

	void releaseRelevantResult(RelevantResult* pRelevantResult)
	{
		pRelevantResultPool->releaseObject(pRelevantResult);
	}

	void releaseQueryResult(QueryResult* pQueryResult)
	{
		delete pQueryResult;
	}

	FTSInstanceInfo* getInfo()
	{
		if (!checkStartedInstance(true))
			return 0;

		Info.UsedMemory = getUsedMemory();
		Info.TotalMemory = getTotalMemory();

		return &Info;
	}

	/*bool isWord(const char* word, uint32 res1, uint32 res2)
	{
		getPartWords(word, strlen(word), tempKey);

		return (val1 == res1 && val2 == res2);
	}*/

	uint32 indexHtmlFile(const char* name, const char* path);
	uint32 indexTextFile(const char* name, const char* path);

	bool indexText(const char* name, uint32 nameLen, char* text, uint32 textLen);
	bool indexText(const char* name,
		char* text,
		uint32 textLen)
	{
		return indexText(name, strlen(name), text, textLen);
	}

	bool indexHtml(const char* name, uint32 nameLen, char* text, uint32 textLen);
	bool indexHtml(const char* name, char* text, uint32 textLen)
	{
		return indexHtml(name, strlen(name), text, textLen);
	}

	void getDocumentNameByID(uint32 id, char* name, uint32 sizeName)
	{
		if (!checkStartedInstance(true))
			return;

		documentsName.getDocumentName(id, name, sizeName);

		/*
		uint32 docHeaderSize = getDocHeaderSize();

		if (Configuration.MemoryMode == IN_MEMORY_MODE)
		{
			//uint32 offset = (id - docHeaderSize) * Info.DocumentNameSize;
			//documentsName.getDocumentNameByOffset(name, offset, sizeName);
			
		}
		else
		{
			if (id < Info.LastNameIDRAM)
			{
				if (id >= Info.LastNameIDHDD)
				{
					//document in ram
					uint32 offset = (id - Info.LastNameIDHDD) * Info.DocumentNameSize;
					documentsName.getDocumentNameByOffset(name, offset, sizeName);
				}
				else
				{
					//document on hdd
					uint32 offset = (id - docHeaderSize) * Info.DocumentNameSize + DOC_NAME_HEADER_SIZE;
					pDocNameFile->read(name, offset, sizeName);
					
				}
			}
		}
		*/
	}

	//bool hasDocumentNameHDD(const char* name, uint32 len, uint32 sizeName)
	//{
	//	if (!checkStartedInstance(true))
	//		return 0;

	//	uint32 offset = DOC_NAME_HEADER_SIZE;

	//	for (uint32 i = 0; i < Info.LastNameIDHDD; i++)
	//	{
	//		pDocNameFile->read(pBuffer, offset, sizeName);

	//		//compare string
	//		uint32 currIdx = 0;
	//		for (; currIdx < len; currIdx++)
	//		{
	//			if (name[currIdx] != pBuffer[currIdx])
	//			{
	//				goto EXIT;
	//			}
	//		}

	//		//compare zeros
	//		for (; currIdx < sizeName; currIdx++)
	//		{
	//			if (pBuffer[currIdx])
	//			{
	//				goto EXIT;
	//			}
	//		}

	//		return true;

	//	EXIT:

	//		offset += sizeName;
	//	}

	//	return false;
	//}

	bool hasDocumentName(const char* name, uint32 len)
	{
		if (!checkStartedInstance(true))
			return 0;

		return documentsName.hasDocumentName(name, len);

		/*
		uint32 docHeaderSize = getDocHeaderSize();

		if (Configuration.MemoryMode == IN_MEMORY_MODE)
		{
			
		}
		else
		{
			return documentsName.hasDocumentName(name, len)
				//|| hasDocumentNameHDD(name, len);
		}
		*/
	}

	uint32 getDocHeaderSize()
	{
		return (sizeof(UNIQUE_IDENTIFIER) + sizeof(Info.Version) + sizeof(FTSInstanceInfo));
	}

	static void clearInstance(char* path)
	{
		//clear dictionary
		HArrayTextFile::deleteFiles(path, "");
		HArrayTextFile::deleteFiles(path, "temp");

		//clear instance
		char documentPath[1024];
		char documentNamePath[1024];

		char documentTempPath[1024];
		char documentNameTempPath[1024];

		FTSConfiguration configuration;
		configuration.setIndexPath(path);

		configuration.getDocumentPath(documentPath);
		configuration.getDocumentNamePath(documentNamePath);
		configuration.getDocumentTempPath(documentTempPath);
		configuration.getDocumentNameTempPath(documentNameTempPath);

		if (BinaryFile::existsFile(documentPath))
		{
			BinaryFile::deleteFile(documentPath);
		}

		if (BinaryFile::existsFile(documentNamePath))
		{
			BinaryFile::deleteFile(documentNamePath);
		}

		if (BinaryFile::existsFile(documentTempPath))
		{
			BinaryFile::deleteFile(documentTempPath);
		}

		if (BinaryFile::existsFile(documentNameTempPath))
		{
			BinaryFile::deleteFile(documentNameTempPath);
		}
	}

	void saveIndex();

	void importIndex(const char* path);

	bool isExistsIndex();

	bool checkStartedInstance(bool requiredStart)
	{
		if (requiredStart && !isStarted)
		{
			logError("Method required start instance.");

			return false;
		}
		
		if(!requiredStart && isStarted)
		{
			logError("Method required stop instance.");

			return false;
		}

		return true;
	}

	void stopInstance()
	{
		if (!checkStartedInstance(true))
			return;

		destroy();

		isStarted = false;
	}

	ulong64 getUsedMemory();
	ulong64 getTotalMemory();
	void printTotalMemory();

	bool checkMemory()
	{
		//need memory on buffer
		uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;
		uint32 needOnBufferMemory = HArrayFixPair::calcArrayMemory(Info.CountWordsRAM, countKeySegments) +
			HArrayFixPair::calcArrayMemory(MAX_SIZE_BUFFER, countKeySegments);

		if (getUsedMemory() + needOnBufferMemory >= Configuration.LimitUsedMemory)
		{
			if (Configuration.AutoSaveIndex)
			{
				if (Configuration.MemoryMode == IN_MEMORY_MODE)
				{
					logWarning("WARNING: Limit of used memory exceed. InMemoryMode of the server is reset.");
					Configuration.MemoryMode = HDD_BUFFERED_MEMORY_MODE;
				}

				this->saveIndex();

				pBlockMemoryPool->clear();
				pDocumentsBlockPool->clear();
				pRelevantResultPool->clear();
				pPostSelectorPool->clear();
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	FTSConfiguration Configuration;
	uchar8 alphabet[256];
	bool needSaveIndex;
	bool isStarted;

	~FTSInstance()
	{
	}

private:
	static uchar8* pBuffer;
	static uint32* pUsedDocNumbers;
	static uint32* pResultDocNumbers;

	HArrayFixRAM haWordsRAM;
	HArrayTextFile haWordsHDD;

	FTSInstanceInfo Info;
	static char LastErrorMessage[1024]; //last error message in instance

	static BlockMemoryPool* pBlockMemoryPool;
	static DocumentsBlockPool* pDocumentsBlockPool;
	static RelevantResultPool* pRelevantResultPool;
	static PostSelectorPool* pPostSelectorPool;

	DocumentsNameTextFile documentsName;

	BinaryFile* pDocFile;
	//BinaryFile* pDocNameFile;

	HArrayFixPair* pAllKeysAndValuesRAM;
	uint32 pAllKeysAndValuesRAMCount;

	uint32 tempKey[25];

	void initAlphabet()
	{
		uchar8 c = 0;
		while (true)
		{
			alphabet[c] = 0; //by default

			if (Configuration.IsUseEnglishAlphabet)
			{
				if ((uchar8)'a' <= c && c <= (uchar8)'z') //lower case
				{
					alphabet[c] = c;
				}
				else if ((uchar8)'A' <= c && c <= (uchar8)'Z') //upper case
				{
					alphabet[c] = c + 32;
				}
				else if ((uchar8)'@' == c || (uchar8)'_' == c || (uchar8)'-' == c) //spec
				{
					alphabet[c] = c;
				}
			}

			if (Configuration.IsUseRussianAlphabet || Configuration.IsUseUkranianAlphabet)
			{
				if (c == (uchar8)'¸' || c == (uchar8)'¨') //convert to "e"
				{
					alphabet[c] = (uchar8)'å';
				}
				else if ((uchar8)'à' <= c && c <= (uchar8)'ÿ') //lower case
				{
					alphabet[c] = c;
				}
				else if ((uchar8)'À' <= c && c <= (uchar8)'ß') //upper case
				{
					alphabet[c] = c + 32;
				}
			}

			if (Configuration.IsUseUkranianAlphabet)
			{
				if (c == (uchar8)'³' || c == (uchar8)'²') //convert to "e"
				{
					alphabet[c] = (uchar8)'³';
				}

				if (c == (uchar8)'¿' || c == (uchar8)'¯') //convert to "e"
				{
					alphabet[c] = (uchar8)'¿';
				}

				if (c == (uchar8)'º' || c == (uchar8)'ª') //convert to "e"
				{
					alphabet[c] = (uchar8)'º';
				}
			}

			if (Configuration.IsUseNumberAlphabet)
			{
				if ((uchar8)'0' <= c && c <= (uchar8)'9') //lower case
				{
					alphabet[c] = c;
				}
			}

			if (c < 255)
				c++;
			else
				break;
		}
	}

	void init()
	{
		initAlphabet();

		pDocFile = 0;
		
		pAllKeysAndValuesRAM = 0;

		Info.init(Configuration.WordsHeaderBase,
			Configuration.RelevantLevel,
			Configuration.CountWordInPhrase,
			LastErrorMessage);

		if (Configuration.MemoryMode == IN_MEMORY_MODE)
		{
			initDictionaryRAM();
		}

		char indexPath[1024];
		Configuration.getIndexPath(indexPath);
		
		haWordsHDD.init(indexPath,
			"",
			Configuration.AutoStemmingOn / 4);

		documentsName.init(indexPath, "");
	}

	inline void indexWord(char* word,
		uint32& len,
		uint32 docID);
	void rebuildIndex();

	void moveDocFileBlocks(BinaryFile* pSourceDocFile,
		BinaryFile* pDestDocFile,

		uchar8* pSourceBuffer,
		uchar8* pDestBuffer,

		ulong64& sourceFilePosition,
		uint32& sourceBuffPosition,
		uint32& sourceBuffLength,

		uint32& destBuffPosition,
		uint32& destBuffLength,

		uint32 maxSizeBuffer,

		uint32 baseDocNumber,
		uint32& lastDocNumber,

		uint32 maxLastNameID)
	{
		bool isFirstIteration = true;

		while (true)
		{
			//1. Read data from file to buffer
			if (sourceBuffLength == 0 || sourceBuffPosition + 128 >= MAX_SIZE_BUFFER)
			{
				//read new data from file
				sourceFilePosition += sourceBuffPosition;
				sourceBuffLength = pSourceDocFile->read(pSourceBuffer, sourceFilePosition, MAX_SIZE_BUFFER);

				sourceBuffPosition = 0;

				if (!sourceBuffLength)
				{
					return;
				}
			}

			//2. Write data to file from buffer
			if (destBuffPosition + 128 >= destBuffLength)
			{
				pDestDocFile->write(pDestBuffer, destBuffPosition);

				destBuffPosition = 0;
			}

			//3. Create header
			uchar8 header = pSourceBuffer[sourceBuffPosition++];

			uint32 leftHeaderPart = (header >> 6);
			uint32 rightHeaderPart = (header & 0x3F);

			uint32 deltaDocNumber = 0;

			if (isFirstIteration)
			{
				//read old number
				if (leftHeaderPart == 2)
				{
					deltaDocNumber = baseDocNumber + rightHeaderPart - lastDocNumber;

					rightHeaderPart = 1; //reset, because is coded one number
				}
				else
				{
					uint32 sizeNumber = sourceBuffPosition + leftHeaderPart + 1;

					uint32 firstDocNumber = 0;
					while (sourceBuffPosition < sizeNumber)
					{
						firstDocNumber = (firstDocNumber << 8) | pSourceBuffer[sourceBuffPosition++];
					}

					deltaDocNumber = baseDocNumber + firstDocNumber - lastDocNumber;
				}

				//save new number
				if (deltaDocNumber < 64)
				{
					pDestBuffer[destBuffPosition++] = (2 << 6) | deltaDocNumber;
				}
				else if (deltaDocNumber < MAX_CHAR)
				{
					pDestBuffer[destBuffPosition++] = rightHeaderPart;
					pDestBuffer[destBuffPosition++] = deltaDocNumber;

					deltaDocNumber += (rightHeaderPart - 1);
				}
				else if (deltaDocNumber < MAX_SHORT)
				{
					pDestBuffer[destBuffPosition++] = (1 << 6) | rightHeaderPart;
					pDestBuffer[destBuffPosition++] = (deltaDocNumber >> 8);
					pDestBuffer[destBuffPosition++] = (deltaDocNumber & 0xFF);

					deltaDocNumber += (rightHeaderPart - 1);
				}
				else
				{
					pDestBuffer[destBuffPosition++] = (3 << 6) | rightHeaderPart;
					pDestBuffer[destBuffPosition++] = (deltaDocNumber >> 24);
					pDestBuffer[destBuffPosition++] = (deltaDocNumber << 8 >> 24);
					pDestBuffer[destBuffPosition++] = (deltaDocNumber << 16 >> 24);
					pDestBuffer[destBuffPosition++] = (deltaDocNumber & 0xFF);

					deltaDocNumber += (rightHeaderPart - 1);
				}

				isFirstIteration = false;
			}
			else
			{
				//read old header
				if (leftHeaderPart == 2)
				{
					pDestBuffer[destBuffPosition++] = header;

					deltaDocNumber = rightHeaderPart;
				}
				else
				{
					pDestBuffer[destBuffPosition++] = header;

					//read document number
					uint32 sizeNumber = sourceBuffPosition + leftHeaderPart + 1;
					while (sourceBuffPosition < sizeNumber)
					{
						uchar8 byte = (uchar8)pSourceBuffer[sourceBuffPosition++];
						deltaDocNumber = (deltaDocNumber << 8) | byte;
						pDestBuffer[destBuffPosition++] = byte;
					}

					deltaDocNumber += (rightHeaderPart - 1);
				}
			}

			lastDocNumber += deltaDocNumber;

			if (lastDocNumber > maxLastNameID)
			{
				logError("Format corrupted !");
				return;
			}

			if (!pSourceBuffer[sourceBuffPosition]) //is last
			{
				//pDestBuffer[destBuffPosition++] = 0;
				sourceBuffPosition++; //with null terminated
				return;
			}
		}

		return;
	}

	void moveDocNameFileBlocks(BinaryFile* pSourceDocNameFile,
		BinaryFile* pDestDocNameFile,
		ulong64& sourceFilePosition,
		uchar8* pBuffer,
		uint32 maxSizeBuffer)
	{
		while (true)
		{
			uint32 buffLength = pSourceDocNameFile->read(pBuffer, sourceFilePosition, maxSizeBuffer);

			pDestDocNameFile->write(pBuffer, buffLength);

			if (buffLength == maxSizeBuffer)
			{
				sourceFilePosition += buffLength;
			}
			else
			{
				break;
			}
		}

		return;
	}

	/*
	void moveDocNameFileBlocksToRAM(BinaryFile* pSourceDocNameFile,
		DocumentsName* pDocumentsName,
		ulong64& sourceFilePosition,
		uchar8* pBuffer,
		uint32 maxSizeBuffer)
	{
		while (true)
		{
			uint32 buffLength = pSourceDocNameFile->read(pBuffer, sourceFilePosition, maxSizeBuffer);

			documentsName.addBytes(pBuffer, buffLength);

			if (buffLength == maxSizeBuffer)
			{
				sourceFilePosition += buffLength;
			}
			else
			{
				break;
			}
		}

		return;
	}
	*/

	//=========================== LOG ==================================
	void logError(const char* text, const char* param);
	void logError(const char* text, uint32 param);
	void logError(const char* text);
	void logWarning(const char* text, const char* param);
	void logWarning(const char* text, uint32 param);
	void logWarning(const char* text);
	void checkKeyAndValue(const char* text,
		ulong64 prevKey,
		ulong64 currKey,
		uint32 currValue,
		uint32 maxValue);

	//=========================== SAVE ==================================
	uint32 getRecoveryIndexState();
	void recoveryIndex();
	void recoveryIndex(uint32 recoveryIndexState);

	void createIndex();
	void updateIndex();

	void openIndex(bool onlyCheckIndex);
	void openOrCreateIndex(bool onlyCheckIndex);

	void closeIndex();

	void openDocIndex()
	{
		if (!pDocFile)
		{
			char documentPath[1024];

			Configuration.getDocumentPath(documentPath);

			pDocFile = new BinaryFile(documentPath, true, false);
			if (!pDocFile->open())
			{
				pDocFile = 0;
			}
		}
	}

	void openDocNameIndex()
	{
		/*if (!pDocNameFile)
		{
			char documentNamePath[1024];

			Configuration.getDocumentNamePath(documentNamePath);

			pDocNameFile = new BinaryFile(documentNamePath, true, false);
			if (!pDocNameFile->open())
			{
				pDocNameFile = 0;
			}
		}*/

		documentsName.openOrCreate(UINT_MAX);
	}

	void closeDicIndex()
	{
		haWordsHDD.close();
	}

	void closeDocIndex()
	{
		if (pDocFile)
		{
			pDocFile->close();

			delete pDocFile;

			pDocFile = 0;
		}
	}

	void closeDocNameIndex()
	{
		/*if (pDocNameFile)
		{
			pDocNameFile->close();

			delete pDocNameFile;

			pDocNameFile = 0;
		}*/

		documentsName.close();
	}

	void initDictionaryRAM()
	{
		if (!haWordsRAM.pHeader)
		{
			char indexPath[1024];
			Configuration.getIndexPath(indexPath);

			haWordsRAM.init(indexPath,
				"",
				Configuration.AutoStemmingOn,
				4,
				Configuration.WordsHeaderBase);
		}
	}

	bool wildCardCompare(const char* strTemplate, const char* str)
	{
		int posStr = 0;
		int posStrTemplate = 0;

		bool anySymbol = false;

		while (str[posStr])
		{
			//skip symbols
			while (strTemplate[posStrTemplate] == '*')
			{
				posStrTemplate++;

				anySymbol = true;
			}

			//exact symbols
			if (str[posStr] == strTemplate[posStrTemplate])
			{
				posStr++;
				posStrTemplate++;

				anySymbol = false;
			}
			else if (anySymbol)
			{
				posStr++;
			}
			else
			{
				return false;
			}
		}

		//skip last symbols
		while (strTemplate[posStrTemplate] == '*')
		{
			posStrTemplate++;
		}

		//reached end of template
		return !strTemplate[posStrTemplate];
	}

	void destroy()
	{
		closeDicIndex();

		closeDocIndex();

		closeDocNameIndex();

		haWordsRAM.destroy();
		haWordsHDD.destroy();		
	}
};