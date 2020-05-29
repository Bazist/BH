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

#ifndef _HARRAY_FIX_BASE		 // Allow use of features specific to Windows XP or later.                   
#define _HARRAY_FIX_BASE 0x709 // Change this to the appropriate value to target other versions of Windows.

#endif	

#include "stdafx.h"
#include "BinaryFile.h"

#define _RELEASE 0x1234567

const uint32 REPOSITORY_VERSION = 1;

const uint32 COUNT_TEMPS = 50;

const uint32 BLOCK_ENGINE_BITS = 4; //bits of block
const uint32 BLOCK_ENGINE_SIZE = 16; //size of block
const uint32 BLOCK_ENGINE_SHIFT = 32 - BLOCK_ENGINE_BITS;
const uint32 BLOCK_ENGINE_STEP = 4;

//const uint32 BLOCK_ENGINE_BITS = 16; //bits of block
//const uint32 BLOCK_ENGINE_SIZE = 65536; //size of block
//const uint32 BLOCK_ENGINE_SHIFT = 32 - BLOCK_ENGINE_BITS;
//const uint32 BLOCK_ENGINE_STEP = 16;

const uint32 BRANCH_ENGINE_SIZE = 4; //can be changed

const uchar8 EMPTY_TYPE = 0;
const uchar8 MIN_BRANCH_TYPE1 = 1;
const uchar8 MAX_BRANCH_TYPE1 = BRANCH_ENGINE_SIZE;
const uchar8 MIN_BRANCH_TYPE2 = BRANCH_ENGINE_SIZE + 1;
const uchar8 MAX_BRANCH_TYPE2 = BRANCH_ENGINE_SIZE * 2;
const uchar8 MIN_BLOCK_TYPE = MAX_BRANCH_TYPE2 + 1;
const uchar8 MAX_BLOCK_TYPE = MIN_BLOCK_TYPE + (32 / BLOCK_ENGINE_STEP) - 1;
const uchar8 CURRENT_VALUE_TYPE = MAX_BLOCK_TYPE + 1;
const uchar8 VALUE_MIN_TYPE = CURRENT_VALUE_TYPE + 1;
const uchar8 VALUE_MAX_TYPE = VALUE_MIN_TYPE + 221;
const uchar8 ONLY_CONTENT_TYPE = VALUE_MAX_TYPE + 1;

const uint32 MAX_HARRAY_PARTITIONS = 4;
const uint32 MAX_HARRAY_PARTITION_CAPACITY = 0xFFFFFFFF - 1024;
const uint32 HARRAY_BUFFER_SIZE = 10 * 1024 * 1024; //10mb

struct HArrayPartitionInfo
{
public:
	uint32 ContentPagesCount;
	uint32 BranchPagesCount;
	uint32 BlockPagesCount;

	uint32 LastContentOffset;
	uint32 LastBranchOffset;
	uint32 LastBlockOffset;
};


struct HArrayFixBaseInfo
{
	uint32 Version;

	uint32 KeyLen;
	uint32 ValueLen;

	uint32 HeaderBase;

	HArrayPartitionInfo Partitions[MAX_HARRAY_PARTITIONS];
	uint32 CountPartitions;
};

//struct ContentTypeCell
//{
//	uchar8 Type;
//};

struct HArrayFixPair
{
private:
	HArrayFixPair()
	{
	}

public:
	static uint32 calcArrayMemory(uint32 count, uint32 countKeySegments)
	{
		return (sizeof(HArrayFixPair) + (countKeySegments << 2)) * count;
	}

	static HArrayFixPair* CreateArray(uint32 count, uint32 countKeySegments)
	{
		char* pArray = new char[calcArrayMemory(count, countKeySegments)];

		HArrayFixPair* pCurrentPair = (HArrayFixPair*)pArray;
		uint32* pCurrentKey = (uint32*)(pArray + sizeof(HArrayFixPair) * count);

		for (uint32 i = 0; i < count; i++,
			pCurrentPair++,
			pCurrentKey += countKeySegments)
		{
			pCurrentPair->Key = pCurrentKey;
			pCurrentPair->Value = 0;
		}

		return (HArrayFixPair*)pArray;
	}

	static void DeleteArray(HArrayFixPair* pArray)
	{
		delete[](char*)pArray;
	}

	uint32* Key; //use additional memory
	ulong64 Value;

	//1 segment = 4 bytes
	int compareKeys(HArrayFixPair& pair, uint32 countKeySegments)
	{
		for (uint32 i = 0; i < countKeySegments; i++)
		{
			if (Key[i] < pair.Key[i])
				return -1;

			if (Key[i] > pair.Key[i])
				return 1;
		}

		return 0;
	}

	int compareKeys(uint32* key, uint32 countKeySegments)
	{
		for (uint32 i = 0; i < countKeySegments; i++)
		{
			if (Key[i] < key[i])
				return -1;

			if (Key[i] > key[i])
				return 1;
		}

		return 0;
	}

	void copyTo(HArrayFixPair& pair, uint32 countKeySegments)
	{
		for (uint32 i = 0; i < countKeySegments; i++)
		{
			pair.Key[i] = Key[i];
		}

		pair.Value = Value;
	}

	void copyFrom(uint32* key, ulong64 value, uint32 countKeySegments)
	{
		for (uint32 i = 0; i < countKeySegments; i++)
		{
			Key[i] = key[i];
		}

		Value = value;
	}

	bool isWord(char* word)
	{
		int len = strlen(word);

		uint32* segments = (uint32*)word;

		for (uint32 i = 0; i < len / 4; i++)
		{
			if (segments[i] != Key[i])
				return false;
		}

		return true;
	}

	~HArrayFixPair()
	{
	}
};

typedef int(*HArrayFixPairComparator)(void* pData, uint32 i, uint32 pivot, uint32 countKeySegments);
typedef void(*HArrayFixPairSwaper)(void* pData, HArrayFixPair* tempKey, uint32 i, uint32 j, uint32 countKeySegments);

class HArrayFixPairUtils
{
public:
	static void sort(uint32* pArray,
					uint32 left,
					uint32 right,
					bool isAsc,
					uint32 countKeySegments,
					HArrayFixPairComparator comparator,
					HArrayFixPairSwaper swaper,
					void* pData)
	{
		int i = left, j = right;	// поставить указатели на исходные места

		HArrayFixPair* tempKey = HArrayFixPair::CreateArray(1, countKeySegments);

		uint32 pivot = pArray[(left + right) / 2];

		while (i <= j)
		{
			if (isAsc)
			{
				while (comparator(pData, i, pivot, countKeySegments) < 0)
				{
					i++;
				}

				while (comparator(pData, j, pivot, countKeySegments) > 0)
				{
					j--;
				}
			}
			else
			{
				while (comparator(pData, i, pivot, countKeySegments) > 0)
				{
					i++;
				}

				while (comparator(pData, j, pivot, countKeySegments) < 0)
				{
					j--;
				}
			}

			if (i <= j)
			{
				swaper(pData, tempKey, i, j, countKeySegments);

				i++;
				j--;
			}
		}

		HArrayFixPair::DeleteArray(tempKey);

		// рекурсивные вызовы, если есть, что сортировать 
		if (left < j)
		{
			sort(pArray, left, j, isAsc, countKeySegments, comparator, swaper, pData);
		}

		if(i < right)
		{
			sort(pArray, i, right, isAsc, countKeySegments, comparator, swaper, pData);
		}
	}
};

//we need split offset on 4 bytes to achive sizeof 5 bytes
struct HeaderCell
{
	uchar8 Offset1;
	uchar8 Offset2;
	uchar8 Offset3;
	uchar8 Offset4;
	uchar8 Partition;
};

struct BranchCell
{
	uint32 Values[BRANCH_ENGINE_SIZE];
	uint32 Offsets[BRANCH_ENGINE_SIZE];
};

struct BlockCell
{
	uchar8 Type;
	uint32 Offset;
	uint32 ValueOrOffset;
};

struct ContentCell
{
	uchar8 Value1;
	uchar8 Value2;
	uchar8 Value3;
	uchar8 Value4;
	uchar8 Type;
};

class ContentPage
{
public:
	ContentCell pContent[MAX_SHORT];
};

class BranchPage
{
public:
	BranchCell pBranch[MAX_SHORT];
};

class BlockPage
{
public:
	BlockPage()
	{
		for (uint32 i = 0; i < MAX_SHORT; i++)
		{
			pBlock[i].Type = 0;
		}
	}

	BlockCell pBlock[MAX_SHORT];
};