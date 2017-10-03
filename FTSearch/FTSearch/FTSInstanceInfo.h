#pragma once

#ifndef FTSINSTANCEINFO_HEADER		// Allow use of features specific to Windows XP or later.                   
#define FTSINSTANCEINFO_HEADER 10	// Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdio.h>
#include "stdafx.h"

const uint32 CODE_VERSION = 8;
const uint32 UNIQUE_IDENTIFIER = 0xBA515D; //bazist :)

class FTSInstanceInfo_old
{
public:
	uint32 Version;

	uint32 LastNameIDRAM;
	uint32 LastNameIDHDD;

	uint32 CountWordsRAM;
	uint32 CountWordsHDD;

	ulong64 UsedMemory;
	ulong64 TotalMemory;

	uint32 WordsHeaderBase;

	uint32 DocumentNameSize;

	char* LastErrorMessage;
	bool HasError;

	uint32 ControlValue;

	uint32 RelevantLevel;

	uint32 CountWordInPhrase;
	uint32 Reserved2;
	uint32 Reserved3;
	uint32 Reserved4;
	uint32 Reserved5;
};

class FTSInstanceInfo
{
public:
	uint32 Version;

	uint32 LastNameIDRAM;
	uint32 LastNameIDHDD;
	
	ulong64 CountWordsRAM;
	ulong64 CountWordsHDD;

	ulong64 UsedMemory;
	ulong64 TotalMemory;
	
	uint32 WordsHeaderBase;
	
	uint32 DocumentNameSize;

	char* LastErrorMessage;
	bool HasError;

	uint32 ControlValue;
	
	uint32 RelevantLevel;
	
	uint32 CountWordInPhrase;

	uint32 Reserved1;
	uint32 Reserved2;

	void init(uint32 wordsHeaderBase,
			  uint32 documentNameSize,
			  uint32 relevantLevel,
			  uint32 countWordInPhrase,
			  char* lastErrorMessage)
	{
		WordsHeaderBase = wordsHeaderBase;
		DocumentNameSize = documentNameSize;
		RelevantLevel = relevantLevel;
		CountWordInPhrase = countWordInPhrase;
		LastErrorMessage = lastErrorMessage;
		
		clear();		
	}

	void print()
	{
		printf("Version: %d\n", Version);

		printf("LastNameIDRAM: %d\n", LastNameIDRAM);
		printf("LastNameIDHDD: %d\n", LastNameIDHDD);
		
		printf("Count words RAM: %d\n", CountWordsRAM);
		printf("Count words HDD: %d\n", CountWordsHDD);

		printf("Used Memory: %d bytes\n", UsedMemory);
		printf("Total Memory: %d bytes\n", TotalMemory);

		printf("DocumentNameSize: %u bytes\n", DocumentNameSize);

		printf("LastErrorMessage %s\n", LastErrorMessage);
	}

	void clear()
	{
		UsedMemory = 0;
		TotalMemory = 0;

		Version = CODE_VERSION;

		CountWordsRAM = 0;
		CountWordsHDD = 0;
		
		LastNameIDRAM = sizeof(UNIQUE_IDENTIFIER) + sizeof(Version) + sizeof(FTSInstanceInfo);
		LastNameIDHDD = sizeof(UNIQUE_IDENTIFIER) + sizeof(Version) + sizeof(FTSInstanceInfo);

		LastErrorMessage[0] = 0;
		HasError = false;

		ControlValue = 0;
	}

	void upgradeInfo(const char* path)
	{
		BinaryFile* file = new BinaryFile("I:\\FTS_Merged\\Instance23\\document.idx", true, false);
		file->open();

		FTSInstanceInfo_old info;
		FTSInstanceInfo info2;

		uint32 uniqueIdentifier1;
		file->readInt(&uniqueIdentifier1);

		uint32 codeVersion1;
		file->readInt(&codeVersion1);

		file->read(&info, sizeof(FTSInstanceInfo_old));

		info2.ControlValue = info.ControlValue;
		info2.CountWordInPhrase = info.CountWordInPhrase;
		info2.CountWordsHDD = info.CountWordsHDD;
		info2.CountWordsRAM = info.CountWordsRAM;
		info2.DocumentNameSize = info.DocumentNameSize;
		info2.HasError = info.HasError;
		info2.LastErrorMessage = info.LastErrorMessage;
		info2.LastNameIDHDD = info.LastNameIDHDD;
		info2.LastNameIDRAM = info.LastNameIDRAM;
		info2.RelevantLevel = info.RelevantLevel;
		info2.TotalMemory = info.TotalMemory;
		info2.UsedMemory = info.UsedMemory;
		info2.Version = info.Version;
		info2.WordsHeaderBase = info.WordsHeaderBase;

		file->setPosition(0);
		file->writeInt(&uniqueIdentifier1);
		file->writeInt(&codeVersion1);
		file->write(&info2, sizeof(FTSInstanceInfo));

		file->close();
	}

	~FTSInstanceInfo()
	{
	}
};