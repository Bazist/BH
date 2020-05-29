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

#pragma once

#ifndef DICTIONARY_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DICTIONARY_HEADER 145	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "DictionaryWord.h"

class Dictionary
{

public:
	Dictionary()
	{
		Words = new DictionaryWord[32];
		
		Count = 0;
		Size = 32;
	}

	void addWord(const char* word, int weight)
	{
		checkSize();

		strcpy(Words[Count].Word, word);

		Words[Count].Weight = weight;

		Count++;
	}

	void addWord(const char* word, uint32 currLen, int weight)
	{
		checkSize();

		uint32 i=0;

		for(; i < currLen; i++)
		{
			Words[Count].Word[i] = word[i];
		}

		Words[Count].Word[i] = 0;

		Words[Count].Weight = weight;

		Count++;
	}

	bool isWordExists(const char* word)
	{
		for(uint32 i=0; i<Count; i++)
		{
			if(strcmp(Words[i].Word, word) == 0)
			{
				return true;
			}
		}

		return false;
	}

	void addWordsFromFile(const uchar8* alphabet,
						  const char* path,
						  uint32 minLenWord,
						  uint32 maxLenWord,
						  int weight)
	{
		FILE* file = fopen(path, "r");

		if(file)
		{
			char* text = new char[MAX_SHORT + 1];
			uint32 count = fread(text, 1, MAX_SHORT, file);

			text[count] = 0;

			addWords(alphabet, text, minLenWord, maxLenWord, weight);

			fclose(file);

			delete[] text;
		}
	}

	void addWords(const uchar8* alphabet,
				  const char* text,
				  uint32 minLenWord,
				  uint32 maxLenWord,
				  int weight)
	{
		char lowerPhrase[MAX_SHORT];

		uint32 currLen = 0;

		uint32 j = 0;
		uchar8 c = 0;

		while(j < MAX_SHORT)
		{
			uchar8 c = text[j];
			
			if((lowerPhrase[j] = alphabet[c]) ||
				c == (uchar8)'%')
			{
				currLen++;
			}
			else if(currLen > 0)
			{
				//get DocumentsBlock
				char* word = lowerPhrase + j - currLen;

				if(minLenWord <= currLen && currLen <= maxLenWord)
				{
					if(!isWordExists(word))
					{
						addWord(word, weight);
					}
				}
		
				currLen = 0;
			}

			if(c)
			{	
				j++;
			}
			else
			{
				break;
			}
		}
	}

	void sortByWeight(bool isAscending)
	{
		for(uint32 i = 0; i < Count; i++)
		{
			for(uint32 j = i + 1; j < Count; j++)
			{
				if((Words[i].Weight > Words[j].Weight) == isAscending)
				{
					DictionaryWord tempWord = Words[i];
					Words[i] = Words[j];
					Words[j] = tempWord;
				}
			}
		}
	}

	void checkSize()
	{
		if(Count == Size)
		{
			uint32 newSize = Size * 2;
			DictionaryWord* newWords = new DictionaryWord[newSize];

			for(uint32 i=0; i<Size; i++)
			{
				newWords[i] = Words[i];
			}

			delete[] Words;

			Words = newWords;

			Size = newSize;
		}
	}

	DictionaryWord* Words;
	
	uint32 Count;
	uint32 Size;

	void clear()
	{
		Count = 0;
	}

	~Dictionary()
	{
		delete[] Words;
	}
};