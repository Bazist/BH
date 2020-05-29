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

#include "StdAfx.h"
#include "FTSInstance.h"

uint32 FTSInstance::indexHtmlFile(const char* name, const char* path)
{
	FILE* file = fopen(path, "r");
	uint32 len = fread(pBuffer, 1, MAX_SIZE_BUFFER, file);
	fclose(file);

	indexHtml(name, strlen(name), (char*)pBuffer, len);

	return len;
}

uint32 FTSInstance::indexTextFile(const char* name, const char* path)
{
	FILE* file = fopen(path, "r");
	uint32 len = fread(pBuffer, 1, MAX_SIZE_BUFFER, file);
	fclose(file);

	indexText(name, strlen(name), (char*)pBuffer, len);

	return len;
}

bool FTSInstance::indexText(const char* name,
							uint32 nameLen,
							char* text,
							uint32 textLen)
{
	if (!checkStartedInstance(true))
		return false;

	initDictionaryRAM();

	//calculate relevant parameters
	uint32 docID = Info.LastNameIDRAM;
	uint32 relevantDocID;
	uint32 relevantBlockSize;

	if(Info.RelevantLevel)
	{
		relevantDocID = (docID << Info.RelevantLevel);
		relevantBlockSize = (textLen >> Info.RelevantLevel) + 1;

		if(relevantBlockSize < MIN_RELEVANT_BLOCK)
		{
			relevantBlockSize = MIN_RELEVANT_BLOCK;
		}
	}

	char* prevWord = 0;
	uint32 prevCurrLen = 0;

	//body document
	uint32 currLen = 0;
	for(uint32 i = 0; i < textLen; i++)
	{
		uchar8 c = text[i];
	
		if(text[i] = alphabet[c])
		{
			currLen++;
		}
		else if(currLen > 0)
		{
			if(currLen >= Configuration.MinLenWord && currLen <= Configuration.MaxLenWord)
			{
				uint32 position = i - currLen;
				char* word = text + position;

				if(Info.CountWordInPhrase == 1)
				{
					if(Info.RelevantLevel)
					{
						indexWord(word, currLen, (relevantDocID | (position / relevantBlockSize)));
					}
					else
					{
						indexWord(word, currLen, docID);
					}
				}
				else
				{
					if((word - prevWord) == prevCurrLen + 1)
					{
						if(prevCurrLen > 8)
						{
							char phraseTemp[258];

							uint32 i=0;

							for(; i < 8; i++)
							{
								phraseTemp[i] = prevWord[i]; 
							}

							phraseTemp[i++] = ' ';

							for(uint32 j = 0; j < currLen; i++, j++)
							{
								phraseTemp[i] = word[j]; 
							}

							indexWord(phraseTemp, i, docID);
						}
						else
						{
							prevWord[prevCurrLen] = ' ';
						
							uint32 phraseLen = prevCurrLen + currLen + 1;
								
							indexWord(prevWord, phraseLen, docID);
						}
					}

					prevWord = word;
					prevCurrLen = currLen;
				}
			}

			currLen = 0;
		}
	}

	//last word
	if(currLen >= Configuration.MinLenWord && currLen <= Configuration.MaxLenWord)
	{
		uint32 position = textLen - currLen;
		char* word = text + position;

		if(Info.RelevantLevel)
		{
			indexWord(word, currLen, (relevantDocID | (position / relevantBlockSize)));
		}
		else
		{
			indexWord(word, currLen, docID);
		}
	}

	documentsName.addName(name,
						  nameLen,
						  Info.LastNameIDRAM,
						  Info.DocumentNameSize);
	
	needSaveIndex = true;

	return checkMemory();
}

bool FTSInstance::indexHtml(const char* name, uint32 nameLen, char* text, uint32 textLen)
{
	if (!checkStartedInstance(true))
		return false;

	initDictionaryRAM();

	bool isText = false;
	uint32 currLen = 0;

	char* prevWord = 0;
	uint32 prevCurrLen = 0;

	//calculate relevant parameters
	uint32 docID = Info.LastNameIDRAM;
	uint32 relevantDocID;
	uint32 relevantBlockSize;

	if(Info.RelevantLevel)
	{
		relevantDocID = (docID << Info.RelevantLevel);
		relevantBlockSize = (textLen >> Info.RelevantLevel) + 1;

		if(relevantBlockSize < MIN_RELEVANT_BLOCK)
		{
			relevantBlockSize = MIN_RELEVANT_BLOCK;
		}
	}

	//body document
	for(uint32 i = 0; i < textLen; i++)
	{
		uchar8 c = text[i];
		if(c == '>')
		{
			isText = true;
			continue;
		}
	
		if(isText)
		{
			if(text[i] = alphabet[c])
			{
				currLen++;
			}
			else if(currLen > 0)
			{
				if(currLen >= Configuration.MinLenWord && currLen <= Configuration.MaxLenWord)
				{
					uint32 position = i - currLen;
					char* word = text + position;

					if(Info.CountWordInPhrase == 1)
					{
						if(Info.RelevantLevel)
						{
							indexWord(word, currLen, (relevantDocID | (position / relevantBlockSize)));
						}
						else
						{
							indexWord(word, currLen, docID);
						}
					}
					else
					{
						if((word - prevWord) == prevCurrLen + 1)
						{
							if(prevCurrLen > 8)
							{
								char phraseTemp[258];

								uint32 i=0;

								for(; i < 8; i++)
								{
									phraseTemp[i] = prevWord[i]; 
								}

								phraseTemp[i++] = ' ';

								for(uint32 j = 0; j < currLen; i++, j++)
								{
									phraseTemp[i] = word[j]; 
								}

								indexWord(phraseTemp, i, docID);
							}
							else
							{
								prevWord[prevCurrLen] = ' ';
						
								uint32 phraseLen = prevCurrLen + currLen + 1;
								
								indexWord(prevWord, phraseLen, docID);
							}
						}

						prevWord = word;
						prevCurrLen = currLen;
					}
				}

				currLen = 0;
			}

			if(c == '<')
			{
				isText = false;
			}
		}
	}

	/*if(Info.LastNameID == 126472)
		Info.LastNameID = 126472;*/

	documentsName.addName(name,
						  nameLen,
						  Info.LastNameIDRAM, 
						  Info.DocumentNameSize);

	needSaveIndex = true;

	return checkMemory();
}

inline void FTSInstance::indexWord(char* word, 
								   uint32& len,
								   uint32 docID)
{
	HArrayVisitor::getPartWords(word, len, tempKey, Configuration.AutoStemmingOn);

	DocumentsBlock* pDocumentsBlock;

	uint32 id = haWordsRAM.getValueByKey(tempKey);

	if(id)
	{
		pDocumentsBlock = pDocumentsBlockPool->getObject(id);
	}
	else
	{
		pDocumentsBlock = pDocumentsBlockPool->newObject(id);

		haWordsRAM.insert(tempKey, id);

		Info.CountWordsRAM++;
	}
	
	pDocumentsBlock->addWord(docID);
}