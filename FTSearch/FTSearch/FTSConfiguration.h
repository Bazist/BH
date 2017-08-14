#include <stdio.h>
#include "stdafx.h"

struct FTSConfiguration
{
public:
	FTSConfiguration()
	{
		MinLenWord = 3;
		MaxLenWord = 64;
		AutoStemmingOn = 8; //change HArrayFixPair

		MaxSizeBuffer = 10000000;
		
		WordsHeaderBase = 24;
		DocmentNameSize = 64;

		IsUseUkranianAlphabet = true;
		IsUseRussianAlphabet = true;
		IsUseEnglishAlphabet = true;
		IsUseNumberAlphabet = true;

		LimitTopResults = 100;
		LimitUsedMemory = 8000000000; //8000 má by default

		InMemoryMode = false;

		RelevantLevel = 0;

		CountWordInPhrase = 1;

		InstanceNumber = 1;

		IsCreateNewInstanceOnUpdate = true;

		IsCustomPath = false;

		AutoSaveIndex = true;
	}

	void getIndexPath(char* buff)
	{
		if (!IsCustomPath)
		{
			sprintf(buff, "%s\\Instance%u", IndexPath, InstanceNumber);
		}
		else
		{
			strcpy(buff, IndexPath);
		}
	}

	void setIndexPath(const char* path)
	{
		strcpy(IndexPath, path);

		IsCustomPath = true;
	}

	void setIndexPath(const char* path, uint32 instanceNumber)
	{
		sprintf(IndexPath, "%s\\Instance%u", path, instanceNumber);

		IsCustomPath = true;
	}

	void getPath(char* buff, char* fileName)
	{
		char indexPath[1024];
		getIndexPath(indexPath);

		sprintf(buff, "%s\\%s.idx", indexPath, fileName);
	}

	void getDocumentPath(char* buff)
	{
		getPath(buff, "document");
	}

	void getDocumentNamePath(char* buff)
	{
		getPath(buff, "documentName");
	}

	void getDocumentTempPath(char* buff)
	{
		getPath(buff, "document_temp");
	}

	void getDocumentNameTempPath(char* buff)
	{
		getPath(buff, "documentName_temp");
	}

	/*char* DictionaryPath;
	char* DocumentPath;
	char* DocumentNamePath;

	char* DocumentPathTemp;*/

	char IndexPath[1024];
	
	uint32 InstanceNumber;

	uint32 MinLenWord;
	uint32 MaxLenWord;
	uint32 AutoStemmingOn;

	uint32 MaxSizeBuffer;
	
	uint32 WordsHeaderBase;
	
	uint32 DocmentNameSize;

	uint32 LimitTopResults;
	ulong64 LimitUsedMemory;
	
	//Level 0 - All words should be in document. A distance between words doesn't matter.
	//Level 1 - A document split on two parts. Words should be in one of part.
	//Level 2 - A document split on four parts. Words should be in one of part.
	//Level 3 - A document split on eight parts. Words should be in one of part.
	//etc...

	uint32 RelevantLevel;

	uint32 CountWordInPhrase;

	bool IsCustomPath;
	
	bool InMemoryMode;

	bool IsUseUkranianAlphabet;
	bool IsUseRussianAlphabet;
	bool IsUseEnglishAlphabet;
	bool IsUseNumberAlphabet;

	bool IsCreateNewInstanceOnUpdate;

	bool AutoSaveIndex;

	//bool IS_ANY_SEQUENCE_WORDS_IN_QUERY = false;

	~FTSConfiguration()
	{
	}
};