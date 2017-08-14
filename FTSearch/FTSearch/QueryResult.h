#pragma once

#ifndef DOCUMENTCELL_HEADER			// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTCELL_HEADER 1457	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "QueryRow.h"

class QueryResult
{
public:
	QueryResult(uint32 size)
	{
		CountRows = 0;
		Size = size;

		minWeight = MAX_INT;

		QueryRows = new QueryRow[size];
	}

	QueryRow* QueryRows;
	
	uint32 minWeight;
	uint32 minWeightIndex;

	uint32 CountRows;
	uint32 Size;

	void sort(bool isAscending)
	{
		for(uint32 i = 0; i < CountRows; i++)
		{
			for(uint32 j = i + 1; j < CountRows; j++)
			{
				if((QueryRows[i].Weight > QueryRows[j].Weight) == isAscending)
				{
					QueryRow tempQueryRow = QueryRows[i];
					QueryRows[i] = QueryRows[j];
					QueryRows[j] = tempQueryRow;
				}
			}
		}
	}

	void addQueryRow(QueryRow& queryRow, bool agregateBySubject)
	{
		////avoid dublicates
		//for(uint32 i=0; i < CountRows; i++)
		//{
		//	if(strcmp(queryRow.DocName,
		//	   QueryRows[i].DocName) == 0)
		//	{
		//		return;
		//	}
		//}

		if(agregateBySubject)
		{
			for(uint32 i=0; i < CountRows; i++)
			{
				for(uint32 j = 0, d = 0; d < 2 && queryRow.DocName[j]; j++)
				{
					if(queryRow.DocName[j] != QueryRows[i].DocName[j])
					{
						goto NOT_EQUALS_BY_SUBJECT;
					}
					else if(queryRow.DocName[j] == '-')
					{
						d++;
					}
				}

				//check weight
				if(QueryRows[i].Weight < queryRow.Weight)
				{
					QueryRows[i] = queryRow;

					if(i == minWeightIndex)
					{
						//search next min distance
						minWeight = MAX_INT;
						minWeightIndex = 0;

						for(uint32 k=0; k < CountRows; k++)
						{
							if(QueryRows[k].Weight < minWeight)
							{
								minWeight = QueryRows[k].Weight;
								minWeightIndex = i;
							}
						}
					}
				}
				else if(QueryRows[i].IsWeightCalcByGroupedSubjects)
				{
					QueryRows[i].Weight++;
				}

				return;

				NOT_EQUALS_BY_SUBJECT:;
			}
		}

		addQueryRow(queryRow);
	}

	void addQueryRow(QueryRow& queryRow)
	{
		if(CountRows < Size)
		{
			QueryRows[CountRows] = queryRow; //copy
		
			if(queryRow.Weight < minWeight)
			{
				minWeight = queryRow.Weight;
				minWeightIndex = CountRows;
			}

			CountRows++;
		}
		else if(queryRow.Weight > minWeight)
		{
			QueryRows[minWeightIndex] = queryRow; //copy

			//search next min distance
			minWeight = MAX_INT;
			minWeightIndex = 0;

			for(uint32 i=0; i < CountRows; i++)
			{
				if(QueryRows[i].Weight < minWeight)
				{
					minWeight = QueryRows[i].Weight;
					minWeightIndex = i;
				}
			}
		}
	}
	
	~QueryResult()
	{
		delete[] QueryRows;
	}
};