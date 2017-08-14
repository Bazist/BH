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

		minWeight = MAX_INT;
	}

	uint32 minWeight;
	uint32 minWeightIndex;
	
	Dictionary dictionary;
	
	uint32 Size;
	
	void print()
	{
		for(uint32 i=0; i < dictionary.Count; i++)
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
		if(dictionary.Count < Size)
		{
			DictionaryWord& dicWord = dictionary.Words[dictionary.Count];
			strcpy(dicWord.Word, word);
			dicWord.DocID = docID;
			dicWord.Weight = weight;

			if(weight < minWeight)
			{
				minWeight = weight;
				minWeightIndex = dictionary.Count;
			}

			dictionary.Count++;
		}
		else if(weight > minWeight)
		{
			//remove min distance
			DictionaryWord& dicWord = dictionary.Words[minWeightIndex];

			strcpy(dicWord.Word, word);
			dicWord.DocID = docID;
			dicWord.Weight = weight;

			//search next min distance
			minWeight = MAX_INT;
			minWeightIndex = 0;

			for(uint32 i=0; i < dictionary.Count; i++)
			{
				if(dictionary.Words[i].Weight < minWeight)
				{
					minWeight = dictionary.Words[i].Weight;
					minWeightIndex = i;
				}
			}
		}
	}
};