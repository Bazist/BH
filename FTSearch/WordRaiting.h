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

#ifndef WORDRAITING_HEADER		// Allow use of features specific to Windows XP or later.                   
#define WORDRAITING_HEADER 145	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"

class WordRaiting
{

public:
	WordRaiting(uint32 size)
	{
		Size = size;
		dictionary.Count = 0;

		minWeight = 0;
	}

	uint32 minWeight;
	uint32 minWeightIndex;

	Dictionary dictionary;

	uint32 Size;

	void print()
	{
		for (uint32 i = 0; i < dictionary.Count; i++)
		{
			printf("%s => %d \n", dictionary.Words[i].Word, dictionary.Words[i].Weight);
		}
	}

	void addWord(char* word, uint32 weight)
	{
		addWord(word, 0, weight);
	}

	void addWord(uint32 docID, uint32 weight)
	{
		addWord("", docID, weight);
	}

	void addWord(char* word, uint32 docID, uint32 weight)
	{
		if (dictionary.Count < Size)
		{
			DictionaryWord& dicWord = dictionary.Words[dictionary.Count];
			strcpy(dicWord.Word, word);
			dicWord.DocID = docID;
			dicWord.Weight = weight;

			if (weight < minWeight || minWeight == 0)
			{
				minWeight = weight;
				minWeightIndex = dictionary.Count;
			}

			dictionary.Count++;
		}
		else if (weight > minWeight)
		{
			//remove min distance
			DictionaryWord& dicWord = dictionary.Words[minWeightIndex];

			strcpy(dicWord.Word, word);
			dicWord.DocID = docID;
			dicWord.Weight = weight;

			//search next min distance
			minWeight = MAX_INT;
			minWeightIndex = 0;

			for (uint32 i = 0; i < dictionary.Count; i++)
			{
				if (dictionary.Words[i].Weight < minWeight)
				{
					minWeight = dictionary.Words[i].Weight;
					minWeightIndex = i;
				}
			}
		}
	}
};