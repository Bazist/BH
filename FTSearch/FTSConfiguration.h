/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

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

		WordsHeaderBase = 24;
		DocmentNameSize = 64;

		IsUseUkranianAlphabet = true;
		IsUseRussianAlphabet = true;
		IsUseEnglishAlphabet = true;
		IsUseNumberAlphabet = true;

		LimitTopResults = 100;
		LimitUsedMemory = 8000000000; //8000 má by default

		MemoryMode = 2;

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
	
	//const uint32 IN_MEMORY_MODE = 1;
	//const uint32 HDD_MEMORY_MODE = 2;
	//const uint32 HDD_BUFFERED_MEMORY_MODE = 3;
	uint32 MemoryMode;
	
	bool IsUseUkranianAlphabet;
	bool IsUseRussianAlphabet;
	bool IsUseEnglishAlphabet;
	bool IsUseNumberAlphabet;

	bool IsCreateNewInstanceOnUpdate;

	bool AutoSaveIndex;

	//bool IS_ANY_SEQUENCE_WORDS_IN_QUERY = false;

	uint32 getCountKeySegments()
	{
		return AutoStemmingOn >> 2;
	}

	~FTSConfiguration()
	{
	}
};