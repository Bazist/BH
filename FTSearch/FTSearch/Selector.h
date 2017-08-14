#pragma once

#ifndef SELECTOR_HEADER		// Allow use of features specific to Windows XP or later.                   
#define SELECTOR_HEADER 1450	// Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdlib.h> 
#include "stdafx.h"
#include "Dictionary.h"
#include "QueryRow.h"
#include "HArrayVisitor.h"

class Selector : public HArrayVisitor
{

public:
	Selector(char* name)
	{
		strcpy(Name, name);

		RangeType = 0;
		SelectorType = 0;

		pDictionary = new Dictionary();
	}

	void init(uchar8* alphabet,
			  uint32 minLenWord,
			  uint32 maxLenWord)
	{
		pDictionary = new Dictionary();

		if(strcmp(Name, "CustomPhrase") == 0 ||
		   strcmp(Name, "CustomList") == 0)
		{
			pDictionary->addWords(alphabet, MinBound, minLenWord, maxLenWord, 0);
		}
		else if(strcmp(Name, "RangeOrTemplate") == 0)
		{
			//nothing
		}
		else //in file
		{
			pDictionary->addWordsFromFile(alphabet, FilePath, minLenWord, maxLenWord, 0);
		}
	}

	char Name[128];

	char FilePath[256];

	char MinBound[128];
	char MaxBound[128];

	Dictionary* pDictionary;

	uint32 AutoStemmingOn;
	uchar8 RangeType; //0 - no range, 1 - range of numbers, 2 - widecard mask
	uchar8 SelectorType; //1 - filter, 2 - columns, 3 - filter + columns
	uchar8 AgregationType; //1 - Amount, 2 - Min value, 3 - Max value, 4 - Doc Number
	uchar8 AgregationSortType; //1 - Amount, 2 - Min value, 3 - Max value, 4 - Doc Number
	bool IsSortAsc; //0 - desc, 1 - asc
	uchar8 ConditionType;  //1 - Equals to amount words in dictionary,
						  //2 - Less than Operand 2
	                      //3 - Equals than Operand 2
						  //4 - More than Operand 2
	                      //5 - Less or equals than Operand 2
						  //6 - More or equals than Operand 2
	uint32 Operand2;

	char tempWord[128];

	virtual void onStartScan()
	{
	}

	virtual bool onVisit(uint32* key, uint32 value)
	{
		HArrayVisitor::getWord(tempWord, key, AutoStemmingOn);

		if(RangeType == 1) //range of numbers
		{
			uint32 number = atoi(tempWord);

			if(number)
			{
				if(number >= atoi(MinBound))
				{
					if(number <= atoi(MaxBound))
					{
						pDictionary->addWord(tempWord, 0);
					}
					else
					{
						return false;
					}
				}
			}
		}
		else if(RangeType == 2) //widecardmask
		{
			uint32 len1 = strlen(MinBound);
			uint32 len2 = strlen(tempWord);
			
			if(len1 == len2) //like xxx???
			{
				for(uint32 i=0; i < len1; i++)
				{
					if(MinBound[i] != tempWord[i] && MinBound[i] != '?')
					{
						return true;
					}
				}

				pDictionary->addWord(tempWord, 0);
			}
			else if(len1 < len2) //like xxx??xx*
			{
				uint32 i=0;

				for(; i < len1 - 1; i++)
				{
					if(MinBound[i] != tempWord[i] && MinBound[i] != '?')
					{
						return true;
					}
				}

				if(MinBound[i] == '*')
				{
					pDictionary->addWord(tempWord, 0);
				}
			}
		}

		return true;
	}

	virtual void onEndScan()
	{
	}
	
	void handleResult(QueryRow& queryRow,
					  char** words,
					  uint32 countWordsInSelector)
	{
		if(SelectorType == 1 || SelectorType == 3) //filter;
		{
			//get operand 1
			uint32 operand1;

			switch(AgregationType)
			{
				case 1: //amount words
					operand1 = countWordsInSelector;
					break;
				
				case 2: //min value
					operand1 = atoi(words[0]);
					break;

				case 3: //max value
					operand1 = atoi(words[countWordsInSelector - 1]);
					break;

				default: //if(AgregationType == 4) //doc number
					operand1 = queryRow.DocNumber;
			}

			//condition
			switch(ConditionType)
			{
				case 1: //1 - Equals to amount words in dictionary,
					queryRow.IsExcluded = !(operand1 == pDictionary->Count);
					break;
				
				case 2:  //2 - Less than Operand 2
					queryRow.IsExcluded = !(operand1 < Operand2);
					break;

				case 3: //3 - Equals than Operand 2
					queryRow.IsExcluded = !(operand1 == Operand2);
					break;
				
				case 4: //4 - More than Operand 2
					queryRow.IsExcluded = !(operand1 > Operand2);
					break;
			
				case 5: //5 - Less or equals than Operand 2
					queryRow.IsExcluded = !(operand1 <= Operand2);
					break;
				
				case 6: //5 - More or equals than Operand 2
					queryRow.IsExcluded = !(operand1 >= Operand2);
					break;

				default:
					queryRow.IsExcluded = true;
					break;
			}
			
			queryRow.CountFilterSelectors++;
		}
		
		if(SelectorType == 2 || SelectorType == 3) //columns
		{
			for(int i=0; i < countWordsInSelector && i < 10; i++) //max 10 words
			{
				strcat(queryRow.Text, words[i]);
				strcat(queryRow.Text, " ");
			}

			strcat(queryRow.Text, ";");
			
			queryRow.CountColumnSelectors++;
		}
		
		if(AgregationSortType)
		{
			switch(AgregationSortType)
			{
				case 1: //Amount
					queryRow.Weight = countWordsInSelector;
					break;
				
				case 2: //Min value
					queryRow.Weight = atoi(words[0]);
					break;
			
				case 3: //Max value
					queryRow.Weight = atoi(words[countWordsInSelector - 1]);
					break;
			
				case 4: //Doc Number
					queryRow.Weight = queryRow.DocNumber; 
					break;
			
				case 5: //Will be cal later by amount grouped subjects
					queryRow.IsWeightCalcByGroupedSubjects = true;
					break;
			}
			
			queryRow.CountSortSelectors++;
		}
	}

	~Selector()
	{
		delete pDictionary;
	}
};