#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <map>
#include "FTSInstance.h"
#include "DocumentsDict.h"
#include "Selector.h"
#include "QueryResult.h"

//#include "iconv.h"

using namespace std;

const uint COUNT_KEYS = 1000000;

uint* keys;

/*BinaryFile bf("c:\\fts\\test.bin", true, true);

	bf.open();

	ulong size = 0;
	uchar buff[1024];

	while(true)
	{
		bf.write(buff, 1024);

		size += 1024;

		uint pos = bf.getPosition();

		if(size != pos)
			pos = pos;
	}

	bf.close();*/

void testUtf2()
{
	setlocale( LC_ALL,"Russian" );

	FILE* file = _wfopen(L"D:\\Part1\\fb2-132108-141328\\132108.fb2", L"r");

	wchar_t* buff = new wchar_t[50000];
	fgetws(buff, 50000, file);
	
	char* str = new char[50000];
	wcstombs(str, buff, 50000);

	printf(str);
}

void test1251()
{
	FILE* file = fopen("D:\\Part1\\fb2-132108-141328\\132108.fb2", "r");

	char* buff = new char[50000];
	fread(buff, 1, 50000, file);

	printf(buff);
}

//69882.fb2

bool isOk = false;

void testFind(clock_t start, ulong& indexedSize, FTSInstance* pFTS, char* directory)
{
	WIN32_FIND_DATA searchData;

	memset(&searchData, 0, sizeof(WIN32_FIND_DATA));

	char currDirectory[1024];
	sprintf(currDirectory, "%s\\*.*", directory);

	HANDLE hFile = FindFirstFile(currDirectory, &searchData);
  
	while(hFile != INVALID_HANDLE_VALUE)
	{
		if(searchData.cFileName[0] != '.')
		{
			if(searchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//subdirectories
				char subDirectory[1024];
				sprintf(subDirectory, "%s\\%s", directory, searchData.cFileName);

				testFind(start, indexedSize, pFTS, subDirectory);
			}
			else
			{
				if(isOk)
				{
					//subdirectories
					char subDirectory[1024];
					sprintf(subDirectory, "%s\\%s", directory, searchData.cFileName);

					uint len = pFTS->indexHtmlFile(searchData.cFileName, subDirectory);

					indexedSize += len;

					uint secs = (uint)(clock() - start) / 1000;
					uint speed = 0;
				
					if(secs)
					{
						speed = indexedSize / secs;
					}

					printf("\rUsed memory: %u. Speed: %u b. Found file: %s", pFTS->getUsedMemory(), speed, searchData.cFileName);
				}

				if(strcmp(searchData.cFileName, "111950.fb2") == 0)
				{
					isOk = true;
				}
			}
		}

		if(FindNextFile(hFile, &searchData) == FALSE)
			break;
	}

	FindClose(hFile);

	return;
}

int testHArrayInt()
{
	//HArrayLongHDD sentence;
	////20/21/22/23/24 /25 /26 /
	////19/14/9 /6 /3.7/2.7/2.4/
	//clock_t start, finish;

	//FILE* file = fopen("c:\\rand.bin", "rb");

	//keys = new uint[COUNT_KEYS];

	//for(uint i=1; i<COUNT_KEYS; i++)
	//{
	//	fread(&keys[i], 4, 1, file);
	//}
	//
	//fclose(file);

	//sentence.init("c:\\FTS\\header.pg", "c:\\FTS\\double.pg", "c:\\FTS\\multiply.pg", 20, 12);

	////uint* keys = new uint[COUNT_KEYS];
	////uint* values = new uint[COUNT_KEYS];
	////
	////for(uint i=0; i<COUNT_KEYS; i++)
	////{
	////	keys[i] = i;
	////	values[i] = i;
	////	count1 += i;
	////}
	//
	////map<uint, uint> m;

	//start = clock();

	////368 mb //average for 15
	////372 mb //average for 10
	////383 mb //average for 5
	////423 mb //average for 1

	////( array(4 bytes value + 2 bytes key) + block(4bytes value + 2 bytes key)
	////12 bytes

	////init(keys, values, COUNT_KEYS);

	//for(uint i=0; i<COUNT_KEYS; i++)
	//{
	//	if(i==506074)
	//		i = i;
	//	
	//	printf( "%2.1d\n", i);
	//	
	//	sentence.insert(keys[i], keys[i]);

	//	//insert(i, i);
	//	//m[keys[i]] = keys[i];
	//	
	//	//4688210
	//	
	//	//if(i>9918351)
	//	//for(uint j=0; j<=i; j++)
	//	//{
	//	//	if(getValueByKey(keys[j]) != keys[j])
	//	//	{
	//	//		printf( "Error: %2.1d %2.1d\n", getValueByKey(j), j);
	//	//		return 0;
	//	//	}
	//	//}
	//}

	//finish = clock();

	////sentence.rebuild(COUNT_KEYS);

	////delete[] values;

	//printf( "Filled: %2.1d msec\n", (finish - start) );

	//start = clock();

	//for(uint i=0; i<COUNT_KEYS; i++)
	//{
	//	if(sentence.getValueByKey(keys[i]) != keys[i])
	//	{
	//		printf( "Error: %2.1d %2.1d\n", sentence.getValueByKey(keys[i]), keys[i]);
	//	}
	//}
	//
	//finish = clock();

	//HArrayLongPair* buffer = new HArrayLongPair[COUNT_KEYS];

	//uint ccc = sentence.getKeysAndValuesByRange(buffer, COUNT_KEYS, 0, MAX_INT);

	//for(uint j = 0; j<ccc; j++)
	//{
	//	if(buffer[j].Key != buffer[j].Value)
	//		buffer[j].Key = 5;
	//}

	//delete[] keys;
	//
	//printf( "Searches: %2.1d msec\n", (finish - start) );
	///*printf( "CountDoublePage: %2.1d\n", sentence.CountDoublePage );
	//printf( "CountMultiplyPage: %2.1d\n", sentence.CountMultiplyPage );*/
	//printf( "Double size pages: %2.1d\n", sentence.getDoublePageMemory());
	//printf( "Multiply size pages: %2.1d\n", sentence.getMultiplyPageMemory());
	//printf( "Total pages: %2.1d\n", sentence.getTotalMemory());
	//
	//uint* buff = new uint[COUNT_KEYS];

	//start = clock();

	//uint count = sentence.getValuesByRange(buff, COUNT_KEYS, 1, 0xFFFFFFFF);
	//
	//finish = clock();

	//printf( "Range: %2.1d msec\n", (finish - start) );

	//delete[] buff;

	//sentence.destroy();

	////2.8 1.3

	//start = clock();

	////uint* buffer = new uint[COUNT_KEYS];

	////int idx = getValuesByRange(buffer, COUNT_KEYS, 0, COUNT_KEYS - 1);
	////
	////delete[] buffer;

	////finish = clock();

	////printf( "Search by range: %2.1d msec\n", (finish - start) );

	////destroy();

	////printf( "List: %2.1d bytes\n", COUNT_KEYS*4*2);

	return 0;
}

void testFVMas(FTSInstance* pFTS)
{
	printf("=====================FVMas====================\n");

	clock_t start, finish;

	 start = clock();

	char path[1024];
	char* buff[912];
	uint lens[912];
	
	for(uint i=0; i<912; i++)
	{
		//printf("%d\n", i); 
		sprintf(path, "C:\\FTS\\FVMAS_Pages\\FVMAS_Page_%d.html", i+1);
		FILE* file = fopen(path, "rb");

		//char* buff = "<bla>lazarus<tt>two free<four><><r>ghj<hj>";
		buff[i] = new char[100000];
		lens[i] = fread(buff[i], 1, 100000, file);

		fclose(file);
	}

	finish = clock();

	printf( "Disc I/O done: %2.1d msec\n", (finish - start) );

	start = clock();

	//49416

	for(uint i=0; i<912; i++)
	{
		//printf("%d\n", i); 
		//sprintf(path, "C:\\FTS\\FVMAS_Pages\\FVMAS_Page_%d.html", i);
		char name[1024];
		sprintf(name, "FVMAS Page: %u", i + 1);

		pFTS->indexHtml(name, buff[i], lens[i]);
	}
	
	finish = clock();

	printf( "Index done: %2.1d msec\n", (finish - start) );
	
	//start = clock();
	for(uint i=0; i<912; i++)
	{
		delete[] buff[i];
	}

	//RelevantResult* pResult = 0;
	//
	//for(uint i=0; i<1000; i++)
	//	pResult = pFTS->searchPhrase("результат выборки");
	//
	//finish = clock();

	//printf( "Search done (1000 searches): %2.1d msec\n", (finish - start) );

	//printf( "Best matches:\n");

	//for(uint j=0; j<pResult->pMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pMatches->pNumbers[j]);

	//printf( "Other matches:\n");

	//for(uint j=0; j<pResult->pOtherMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pOtherMatches->pNumbers[j]);
	//
	//pFTS->destroy();

	//
	///*
	//printf( "Count: %2.1d mb\n", count / 1000000 );

	//printf( "CountDoublePage: %2.1d\n", sentence.CountDoublePage );
	//printf( "CountMultiplyPage: %2.1d\n", sentence.CountMultiplyPage );

	//*/

	//delete pFTS;
}

void testFVMasN(FTSInstance* pFTS, uint count)
{
	printf("=====================FVMas====================\n");

	clock_t start, finish;

	 start = clock();

	char path[1024];
	char* buff[912];
	uint lens[912];
	
	for(uint i=0; i<912; i++)
	{
		//printf("%d\n", i); 
		sprintf(path, "C:\\FTS\\FVMAS_Pages\\FVMAS_Page_%d.html", i+1);
		FILE* file = fopen(path, "rb");

		//char* buff = "<bla>lazarus<tt>two free<four><><r>ghj<hj>";
		buff[i] = new char[100000];
		lens[i] = fread(buff[i], 1, 100000, file);

		fclose(file);
	}

	finish = clock();

	printf( "Disc I/O done: %2.1d msec\n", (finish - start) );

	start = clock();

	//49416

	for(uint j=0; j<count; j++)
	{
		printf( "===> %u, %s\n", j, pFTS->getInfo()->LastErrorMessage);

		for(uint i=0; i<912; i++)
		{
			//printf("%d\n", i); 
			//sprintf(path, "C:\\FTS\\FVMAS_Pages\\FVMAS_Page_%d.html", i);
			char name[1024];
			sprintf(name, "FVMAS Page: %u", i);

			pFTS->indexHtml(name, buff[i], lens[i]);
		}
	}
	
	finish = clock();

	printf( "Index done: %2.1d msec\n", (finish - start) );
	
	//start = clock();
	for(uint i=0; i<912; i++)
	{
		delete[] buff[i];
	}

	//RelevantResult* pResult = 0;
	//
	//for(uint i=0; i<1000; i++)
	//	pResult = pFTS->searchPhrase("результат выборки");
	//
	//finish = clock();

	//printf( "Search done (1000 searches): %2.1d msec\n", (finish - start) );

	//printf( "Best matches:\n");

	//for(uint j=0; j<pResult->pMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pMatches->pNumbers[j]);

	//printf( "Other matches:\n");

	//for(uint j=0; j<pResult->pOtherMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pOtherMatches->pNumbers[j]);
	//
	//pFTS->destroy();

	//
	///*
	//printf( "Count: %2.1d mb\n", count / 1000000 );

	//printf( "CountDoublePage: %2.1d\n", sentence.CountDoublePage );
	//printf( "CountMultiplyPage: %2.1d\n", sentence.CountMultiplyPage );

	//*/

	//delete pFTS;
}

//void testImport(FTSInstance* pFTS)
//{
//	printf("=====================Import====================\n");
//
//	clock_t start, finish;
//
//	start = clock();
//
//	pFTS->importIndex("C:\\FTS\\Indexes\\dictionary.idx", "C:\\FTS\\Indexes\\document.idx", "C:\\FTS\\Indexes\\documentName.idx", false);
//
//	finish = clock();
//
//	printf( "Import index done: %2.1d msec\n", (finish - start) );
//}

void testNED(FTSInstance* pFTS)
{
	printf("=====================NED=======================\n");

	clock_t start, finish;

	start = clock();

	char path[1024];
	char* buff[1600];
	uint lens[1600];
	
	for(uint i=0; i<1600; i++)
	{
		//printf("%d\n", i); 
		sprintf(path, "C:\\FTS\\NED_Pages\\NED_Page_%d.html", i+1);
		FILE* file = fopen(path, "rb");

		//char* buff = "<bla>lazarus<tt>two free<four><><r>ghj<hj>";
		buff[i] = new char[100000];
		lens[i] = fread(buff[i], 1, 100000, file);

		fclose(file);
	}

	finish = clock();

	printf( "Disc I/O done: %2.1d msec\n", (finish - start) );

	start = clock();

	//49416

	for(uint i=0; i<1600; i++)
	{
		//printf("%d\n", i); 
		//sprintf(path, "C:\\FTS\\FVMAS_Pages\\FVMAS_Page_%d.html", i);
		
		char name[1024];
		sprintf(name, "Ned Page: %d", i);

		pFTS->indexHtml(name, buff[i], lens[i]);
	}
	
	finish = clock();

	//pFTS->getInfo().print();

	printf( "Index done: %2.1d msec\n", (finish - start) );
	
	//start = clock();


	//RelevantResult* pResult = 0;
	//
	//for(uint i=0; i<1000; i++)
	//	pResult = pFTS->searchPhrase("результат выборки");
	//
	//finish = clock();

	//printf( "Search done (1000 searches): %2.1d msec\n", (finish - start) );

	//printf( "Best matches:\n");

	//for(uint j=0; j<pResult->pMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pMatches->pNumbers[j]);

	//printf( "Other matches:\n");

	//for(uint j=0; j<pResult->pOtherMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pOtherMatches->pNumbers[j]);
	//
	//pFTS->destroy();

	//
	///*
	//printf( "Count: %2.1d mb\n", count / 1000000 );

	//printf( "CountDoublePage: %2.1d\n", sentence.CountDoublePage );
	//printf( "CountMultiplyPage: %2.1d\n", sentence.CountMultiplyPage );

	//*/

	//delete pFTS;
}

void testNEDN(FTSInstance* pFTS, uint count)
{
	printf("=====================NED=======================\n");

	clock_t start, finish;

	start = clock();

	char path[1024];
	char* buff[1600];
	uint lens[1600];
	
	for(uint i=0; i<1600; i++)
	{
		//printf("%d\n", i); 
		sprintf(path, "C:\\FTS\\NED_Pages\\NED_Page_%d.html", i+1);
		FILE* file = fopen(path, "rb");

		//char* buff = "<bla>lazarus<tt>two free<four><><r>ghj<hj>";
		buff[i] = new char[100000];
		lens[i] = fread(buff[i], 1, 100000, file);

		fclose(file);
	}

	finish = clock();

	printf( "Disc I/O done: %2.1d msec\n", (finish - start) );

	start = clock();

	//49416
	for(uint j=0; j<count; j++)
	{
		printf( "===> %u, %s\n", j, pFTS->getInfo()->LastErrorMessage);

		for(uint i=0; i<1600; i++)
		{
			//printf("%d\n", i); 
			//sprintf(path, "C:\\FTS\\FVMAS_Pages\\FVMAS_Page_%d.html", i);
		
			char name[1024];
			sprintf(name, "Ned Page: %d", i);

			pFTS->indexHtml(name, buff[i], lens[i]);
		}
	}
	
	finish = clock();

	//pFTS->getInfo().print();

	printf( "Index done: %2.1d msec\n", (finish - start) );
	
	//start = clock();


	//RelevantResult* pResult = 0;
	//
	//for(uint i=0; i<1000; i++)
	//	pResult = pFTS->searchPhrase("результат выборки");
	//
	//finish = clock();

	//printf( "Search done (1000 searches): %2.1d msec\n", (finish - start) );

	//printf( "Best matches:\n");

	//for(uint j=0; j<pResult->pMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pMatches->pNumbers[j]);

	//printf( "Other matches:\n");

	//for(uint j=0; j<pResult->pOtherMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pOtherMatches->pNumbers[j]);
	//
	//pFTS->destroy();

	//
	///*
	//printf( "Count: %2.1d mb\n", count / 1000000 );

	//printf( "CountDoublePage: %2.1d\n", sentence.CountDoublePage );
	//printf( "CountMultiplyPage: %2.1d\n", sentence.CountMultiplyPage );

	//*/

	//delete pFTS;
}

void testTolstoy(FTSInstance* pFTS)
{
	printf("======================Tolstoy=====================\n");

	clock_t start, finish;

	start = clock();

	char path[1024];
	char* buff[5];
	uint lens[5];
	
	uint i=1;
	
	for(uint i=0; i<1; i++)
	{
		//printf("%d\n", i); 
		sprintf(path, "C:\\FTS\\voyna_i_mir_%d.txt", i+1);
		FILE* file = fopen(path, "rb");

		//char* buff = "<bla>lazarus<tt>two free<four><><r>ghj<hj>";
		buff[i] = new char[10000000];
		lens[i] = fread(buff[i], 1, 10000000, file);

		fclose(file);
	}

	finish = clock();

	printf( "Disc I/O done: %2.1d msec\n", (finish - start) );

	start = clock();

	//49416

	for(uint i=0; i<50; i++)
	{
		//printf("%d\n", i); 
		//sprintf(path, "C:\\FTS\\FVMAS_Pages\\FVMAS_Page_%d.html", i);
		char name[1024];
		sprintf(name, "Tolstoy Page: %d", i);

		pFTS->indexText(name, buff[0], lens[0]);
	}
	
	finish = clock();

	//pFTS->getInfo().print();

	printf( "Index done: %2.1d msec\n", (finish - start) );

	//start = clock();


	//RelevantResult* pResult = 0;
	//
	//for(uint i=0; i<1000; i++)
	//	pResult = pFTS->searchPhrase("результат выборки");
	//
	//finish = clock();

	//printf( "Search done (1000 searches): %2.1d msec\n", (finish - start) );

	//printf( "Best matches:\n");

	//for(uint j=0; j<pResult->pMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pMatches->pNumbers[j]);

	//printf( "Other matches:\n");

	//for(uint j=0; j<pResult->pOtherMatches->Count; j++)
	//	printf("Page: %d\n", pResult->pOtherMatches->pNumbers[j]);
	//
	//pFTS->destroy();

	//
	///*
	//printf( "Count: %2.1d mb\n", count / 1000000 );

	//printf( "CountDoublePage: %2.1d\n", sentence.CountDoublePage );
	//printf( "CountMultiplyPage: %2.1d\n", sentence.CountMultiplyPage );

	//*/

	//delete pFTS;
}

void test()
{
	DocumentsDict d1;
	
	BinaryFile* bf1 = new BinaryFile("test.txt", true, true);
	
	uint pos = 0;
	uchar buffer[1024];
	bf1->open();
	d1.writeKeyValue(bf1, buffer, 1024, pos, 555555555555, 555555555555);
	d1.writeKeyValue(bf1, buffer, 1024, pos, 555555555556, 555555555556);
	d1.writeKeyValue(bf1, buffer, 1024, pos, 555555555557, 555555555557);

	bf1->write(buffer, pos);
	bf1->close();

	pos = 0;
	uint currPos = 0;
	ulong key = 0;
	ulong value = 0;
	uint maxLength = 1024;
	
	BinaryFile* bf2 = new BinaryFile("test.txt", false, false);
	
	DocumentsDict d2;
	bf2->open();
	d2.readKeyValue(bf2, buffer, maxLength, pos, currPos, key, value, 0);
	d2.readKeyValue(bf2, buffer, maxLength, pos, currPos, key, value, 1);
	d2.readKeyValue(bf2, buffer, maxLength, pos, currPos, key, value, 2);
	bf2->close();
}

void test01_IndexRAM()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	pFTS->startInstance(conf);
	
	testFVMas(pFTS);

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	uint id = pResult->Matches[0];
	if(id == 574)
	{
		printf("Test 01. OK\n");
	}
	else
	{
		printf("Test 01. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test02_IndexHDD()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	pFTS->startInstance(conf);
	
	testFVMas(pFTS);

	pFTS->saveIndex();
	pFTS->stopInstance();

	pFTS->startInstance(conf);

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	uint id = pResult->Matches[0];
	if(id == 574)
	{
		printf("Test 02. OK\n");
	}
	else
	{
		printf("Test 02. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test03_Index2RAM()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	pFTS->startInstance(conf);

	testFVMas(pFTS);

	testNED(pFTS);

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);

	uint id1 = pResult->Matches[0];

	pResult = pFTS->searchPhrase("антимишка");
	pFTS->releaseResult(pResult);
	
	uint id2 = pResult->Matches[0];

	if(id1 == 574 && id2 == 1815)
	{
		printf("Test 03. OK\n");
	}
	else
	{
		printf("Test 03. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test04_Index2HDD()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	pFTS->startInstance(conf);
	
	testFVMas(pFTS);

	pFTS->saveIndex();

	testNED(pFTS);

	pFTS->saveIndex();

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	uint id1 = pResult->Matches[0];

	pResult = pFTS->searchPhrase("антимишка");
	pFTS->releaseResult(pResult);
	
	uint id2 = pResult->Matches[0];

	if(id1 == 574 && id2 == 1815)
	{
		printf("Test 04. OK\n");
	}
	else
	{
		printf("Test 04. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test05_IndexImport2HDD()
{
	FTSInstance::clearInstance("C:\\FTS\\IndexesImport");

	//first instance
	FTSInstance* pFTS1 = new FTSInstance();

	FTSConfiguration conf1;
	strcpy(conf1.IndexPath, "C:\\FTS\\IndexesImport");
	
	pFTS1->startInstance(conf1);

	testNED(pFTS1);

	pFTS1->saveIndex();
	pFTS1->stopInstance();

	//second instance
	FTSInstance::clearInstance("c:\\FTS");

	FTSInstance* pFTS2 = new FTSInstance();

	FTSConfiguration conf2;
	strcpy(conf1.IndexPath, "C:\\FTS");

	pFTS2->startInstance(conf2);

	testFVMas(pFTS2);
	
	pFTS2->saveIndex();

	pFTS2->importIndex("C:\\FTS\\IndexesImport");

	//import
	//pFTS2->importIndex("C:\\FTS2", true);
	
	//check
	RelevantResult* pResult = 0;
		
	pResult = pFTS2->searchPhrase("базист");
	pFTS2->releaseResult(pResult);
	
	uint id1 = pResult->Matches[0];

	pResult = pFTS2->searchPhrase("антимишка");
	pFTS2->releaseResult(pResult);
	
	uint id2 = pResult->Matches[0];

	if(id1 == 574  && id2 == 1815)
	{
		printf("Test 05. OK\n");
	}
	else
	{
		printf("Test 05. Failed\n");
	}

	pFTS2->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test06_WordWithLen10HDD()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	conf.AUTO_STEMMING_ON = 10;
	
	pFTS->startInstance(conf);

	char name[100];
	sprintf(name, "pikosecpik");

	testFVMas(pFTS);
	pFTS->indexText("test", name, 10);
	/*pFTS->saveIndex();
	pFTS->stopInstance();

	pFTS->startInstance(conf);*/
	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("pikosecpik");
	uint id = pResult->Matches[0];
	pFTS->getDocumentNameByID(id, name, 100);

	if(*(uint*)name == *(uint*)"test")
	{
		printf("Test 06. OK\n");
	}
	else
	{
		printf("Test 06. Failed\n");
	}
	
	pFTS->releaseResult(pResult);
	pFTS->stopInstance();
}

void test07_Index10HDD()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	pFTS->startInstance(conf);

	for(uint j=0; j<20; j++)
	{
		testFVMas(pFTS);
		pFTS->saveIndex();
	}

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	uint id = pResult->Matches[0];
	if(id == 574)
	{
		printf("Test 07. OK\n");
	}
	else
	{
		printf("Test 07. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test08_RelevantLevel()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	conf.RELEVANT_LEVEL = 8;

	pFTS->startInstance(conf);

	testFVMas(pFTS);
	pFTS->saveIndex();
	
	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("оперативно работает");
	pFTS->releaseResult(pResult);

	uint id = pResult->Matches[0];

	if(id == 122)
	{
		printf("Test 08. OK\n");
	}
	else
	{
		printf("Test 08. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test09_GetDocNameRAM()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	pFTS->startInstance(conf);

	char name[100];
	sprintf(name, "pikosec");

	testFVMas(pFTS);
	pFTS->indexText("test", name, 7);

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("pikosec");
	uint id = pResult->Matches[0];
	pFTS->getDocumentNameByID(id, name, 100);

	if(*(uint*)name == *(uint*)"test")
	{
		printf("Test 09. OK\n");
	}
	else
	{
		printf("Test 09. Failed\n");
	}
	
	pFTS->releaseResult(pResult);
	pFTS->stopInstance();
}

void test10_GetDocNameHDD()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	pFTS->startInstance(conf);

	char name[100];
	sprintf(name, "pikosec");

	testFVMas(pFTS);
	pFTS->indexText("test", name, 7);
	pFTS->saveIndex();
	pFTS->stopInstance();

	pFTS->startInstance(conf);
	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("pikosec");
	uint id = pResult->Matches[0];
	pFTS->getDocumentNameByID(id, name, 100);

	if(*(uint*)name == *(uint*)"test")
	{
		printf("Test 10. OK\n");
	}
	else
	{
		printf("Test 10. Failed\n");
	}
	
	pFTS->releaseResult(pResult);
	pFTS->stopInstance();
}

void test11_StartStartStopStopInstance()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	pFTS->startInstance(conf);

	RelevantResult* rr = pFTS->searchPhrase("blah");

	testFVMas(pFTS);
	pFTS->saveIndex();
	pFTS->stopInstance();

	//start start
	pFTS->startInstance(conf);
	pFTS->startInstance(conf);

	pFTS->stopInstance();
	pFTS->stopInstance();

	printf("Test 11. OK\n");
}

void test12_IndexPhraseRAM()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	pFTS->startInstance(conf);

	testFVMas(pFTS);

	RelevantResult* pResult = 0;
	
	for(uint i=0; i<100; i++)
	{
		pResult = pFTS->searchPhrase("базист обосрался");
		if(pResult->CountMatches != 23)
		{
			printf("Test 12. Failed\n");
			pFTS->stopInstance();
			pFTS->releaseResult(pResult);
			return;
		}

		pFTS->releaseResult(pResult);
	}
	
	printf("Test 12. OK\n");
	
	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test13_Index2HDDDiffInst()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	conf.IN_MEMORY_MODE = true;

	pFTS->startInstance(conf);

	testFVMas(pFTS);
	pFTS->saveIndex();
	pFTS->stopInstance();

	delete pFTS;

	//second inst
	pFTS = new FTSInstance();
	pFTS->startInstance(conf);
	
	testNED(pFTS);

	pFTS->saveIndex();
	pFTS->stopInstance();

	delete pFTS;

	//third inst
	pFTS = new FTSInstance();
	pFTS->startInstance(conf);

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	uint id1 = pResult->Matches[0];

	pResult = pFTS->searchPhrase("антимишка");
	pFTS->releaseResult(pResult);
	
	uint id2 = pResult->Matches[0];

	if(id1 == 574 && id2 == 1815)
	{
		printf("Test 13. OK\n");
	}
	else
	{
		printf("Test 13. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test14_CheckPools()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	pFTS->startInstance(conf);

	testFVMas(pFTS);

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	if(pFTS->getSearchPoolsMemory() == 0)
	{
		printf("Test 14. OK\n");
	}
	else
	{
		printf("Test 14. Failed\n");
	}

	pFTS->stopInstance();
}

void test15_InMemory()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS1 = new FTSInstance();

	FTSConfiguration conf1;
	
	pFTS1->startInstance(conf1);

	testFVMas(pFTS1);

	pFTS1->saveIndex();

	pFTS1->stopInstance();

	//instance
	FTSInstance* pFTS2 = new FTSInstance();

	FTSConfiguration conf2;
	conf2.IN_MEMORY_MODE = true;

	pFTS2->startInstance(conf2);
	
	RelevantResult* pResult = 0;
		
	pResult = pFTS2->searchPhrase("базист");
	pFTS2->releaseResult(pResult);
	
	uint id1 = pResult->Matches[0];

	if(id1 == 574)
	{
		printf("Test 15. OK\n");
	}
	else
	{
		printf("Test 15. Failed\n");
	}

	pFTS2->stopInstance();

	delete pFTS2;
}

void test16_GetDocNameInMemory()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS1 = new FTSInstance();

	FTSConfiguration conf1;
	
	pFTS1->startInstance(conf1);

	char name[100];
	sprintf(name, "pikosec");

	testFVMas(pFTS1);
	pFTS1->indexText("test", name, 7);

	pFTS1->saveIndex();

	pFTS1->stopInstance();

	//instance
	FTSInstance* pFTS2 = new FTSInstance();

	FTSConfiguration conf2;
	conf2.IN_MEMORY_MODE = true;

	pFTS2->startInstance(conf2);
	
	RelevantResult* pResult = 0;
		
	pResult = pFTS2->searchPhrase("pikosec");
	uint id = pResult->Matches[0];
	pFTS2->getDocumentNameByID(id, name, 100);

	if(*(uint*)name == *(uint*)"test")
	{
		printf("Test 16. OK\n");
	}
	else
	{
		printf("Test 16. Failed\n");
	}

	pFTS2->stopInstance();

	delete pFTS2;
}

void test17_RecoveryIndexBegin()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	pFTS->startInstance(conf);

	testFVMas(pFTS);

	pFTS->saveIndex();

	testNED(pFTS);

	//Reset computer here !
	pFTS->saveIndex();
}

void test17_RecoveryIndexAfterReset()
{
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	pFTS->startInstance(conf);

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	uint id1 = pResult->Matches[0];

	if(id1 == 574)
	{
		printf("Test 17. OK\n");
	}
	else
	{
		printf("Test 17. Failed\n");
	}
}

void test50HighLoadIndex()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	pFTS->startInstance(conf);

	testFVMasN(pFTS, 500);
	testNEDN(pFTS, 500);
		
	pFTS->saveIndex();
	pFTS->stopInstance();
}

void test51HighLoadSearch()
{
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	

	pFTS->startInstance(conf);
	/*pFTS->clearInstance();
	
	testFVMas(pFTS);*/

	clock_t start, finish;
	start = clock();

	uint count = 0;
	
	//for(uint i=0; i<1000; i++)
	//{
		RelevantResult* rr;
		rr = pFTS->searchPhrase("зюзя бойфренд");
		
		for(uint i=0; i < rr->CountMatches; i++)
		{
			char name[100];
			uint id = rr->Matches[i];
			pFTS->getDocumentNameByID(id, name, 100);

			printf("%s\n", name);
		}

		count += rr->CountMatches;
		pFTS->releaseResult(rr);
	//}
	
	finish = clock();

	printf( "=== Speed: %2.1d msec ===\n", (finish - start) );
	printf( "=== Count: %u ===\n", count);
	
	pFTS->stopInstance();
}

#define U_MAX_BYTES 4

unsigned short u_getc(FILE *stream, char *bytes) {
    /* mask values for bit pattern of first byte in multi-byte
     UTF-8 sequences: 
       192 - 110xxxxx - for U+0080 to U+07FF 
       224 - 1110xxxx - for U+0800 to U+FFFF 
       240 - 11110xxx - for U+010000 to U+1FFFFF */
    static unsigned short mask[] = {192, 224, 240}; 

    unsigned short i, j; 

    /* initialize buffer */
    memset(bytes, 0, U_MAX_BYTES + 1); 

    /* read first byte into buffer */
    bytes[0] = getc(stream);
    if (bytes[0] == EOF) { 
        return 0; 
    } 

    /* check how many more bytes need to be read for
     character */
    i = 0;
    if ((bytes[0] & mask[0]) == mask[0]) i++;
    if ((bytes[0] & mask[1]) == mask[1]) i++;
    if ((bytes[0] & mask[2]) == mask[2]) i++;

    /* read subsequent character bytes */
    j = 0;
    while (j < i) {
        j++;
        bytes[j] = getc(stream);
    }

    /* return number of bytes read into buffer */
    return i + 1;
}

uint testDir(char* path)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	TCHAR patter[MAX_PATH];
    TCHAR fileName[MAX_PATH];
	
	//memset(patter, 0x00, MAX_PATH);
	
	_stprintf(patter, TEXT("%s\\*"), path);
	
	hFind = FindFirstFile(patter, &FindFileData);
	
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		printf ("FindFirstFile failed (%d)\n", GetLastError());
		return 1;
	} 
	else 
	{
		do
		{
			//ignore current and parent directories
			if(_tcscmp(FindFileData.cFileName, TEXT("."))==0 || _tcscmp(FindFileData.cFileName, TEXT(".."))==0)
				continue;

			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				_stprintf(patter, TEXT("%s\\%s"), path, FindFileData.cFileName);

				//ignore directories
				testDir(patter);

			}
			else
			{
				//list the Files

				_stprintf(fileName, TEXT("%s\\%s"), path, FindFileData.cFileName);
                
				printf("%s\r", fileName);

				/*setlocale( LC_ALL,"rus_rus.866" );

				FILE* file = _wfopen(L"i:\\dou\\1\\post_1.html", L"r");

				wchar_t* buff = new wchar_t[50000];
				fgetws(buff, 50000, file);
	
				char* str = new char[50000];
				wcstombs(str, buff, 50000);

				str = str;*/
			}
		}
		while (FindNextFile(hFind, &FindFileData));
		
		FindClose(hFind);
	}
}

void test52MatchSearch()
{
	HArrayFixHDD ram;

	ram.init("C:\\FTS", "blah", 8, 4, 16);

	ram.openOrCreate(4000000);

	char* keys = new char[1000000 * 8];

	strcmp(keys, keys);

	for(uint i=0; i<1000000; i++)
	{
		sprintf(&keys[i * 8], "%08d", rand() * i);
	}

	clock_t start, finish;
	start = clock();

	for(uint i=0; i<1000000; i++)
	{
		//if(i%1000 == 0)
		//	printf("\r%d", i);

		/*if(i==468184)
			i=i;*/

		//sprintf(&keys[0 * 12], "%012d", rand() * i);

		ram.insert((uint*)&(keys[i*8]), i);
	}

	ram.close();
	//ram.save();

	finish = clock();


	printf( "=== Speed: %2.1d msec ===\n", (finish - start) );

	return;

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	conf.IN_MEMORY_MODE = false;
	conf.AUTO_STEMMING_ON = 20;
	conf.COUNT_WORD_IN_PHRASE = 2;
	
	pFTS->startInstance(conf);

	char* html[10];

	html[0] = "белый";html[1] = "синий";html[2] = "черный";html[3] = "красный";html[4] = "желтый";html[5] = "оранжевый";html[6] = "зеленый";html[7] = "бежевый";html[8] = "фиолетовый";html[9] = "малиновый";
	
	//strcpy(html, "одинадцать два три четыре пять синий красный белы зеленый желтый белый суп восемь жигуль");

	//pFTS->indexText("registered", html, strlen(html));
	RelevantResult* pResult;

	//clock_t start, finish;
	start = clock();

	/*for(uint s=0; s<10; s++)
	{
		for(uint i=0; i<10; i++)
		{
			for(uint j=0; j<10; j++)
			{*/
				char buff[100];
				//sprintf(buff, "%s %s", html[i], html[j]);
				sprintf(buff, "синий экран");
	
				pResult = pFTS->searchPhrase(buff, strlen(buff), 80, pFTS->getInfo()->LastNameIDRAM);
	/*		}
		}
	}*/

	finish = clock();

	printf( "=== Speed: %2.1d msec ===\n", (finish - start) );

	//return;

	//pFTS->searchHtmlSeldomWords(html, strlen(html));*/

	//pFTS->saveIndex();

	//pFTS->clearInstance();

	//search
	//80
	//1623345

	//velo
	//1623346
	//1725888
		
	//pFTS->indexText("registered", buff, strlen(buff));

	//sprintf(buff, "all");

	//RelevantResult* pResult = pFTS->searchPhraseRel(buff, strlen(buff), 80, pFTS->getInfo()->LastNameIDRAM);
	
	//testFVMas(pFTS);*/

	uint count = 0;
	
	uint points[500];

	//pFTS->calculateTrend("гагарин", points, 500, 80, 152875);

	//Dictionary dics[2];

	//dics[0].addWord("машина", 10);
	//dics[0].addWord("гагарин", 10);
	//dics[1].addWord("экран", 10);

	//dics[0].addWord("морале", 10);
	//dics[0].addWord("киевом", 10);
	//dics[0].addWord("киеву", 10);
	//dics[0].addWord("пашэ", 10);
	
	//dics[1].addWord("охлобыстин", 10);
	//dics[2].addWord("банкинг", 10);
	//dics[1].addWordsFromFile(pFTS->alphabet, "c:\\FTS\\Dics\\mat.txt", 10);

	//uint points[100];
	//RelevantResult* pResult = pFTS->searchPhraseRel("obama", 5, 1663675, 1668369);

	//pResult = pFTS->searchPhraseRel("байк", 7, 1000, 50000);
	//wr.print();

	//rr = pFTS->searchMatch(dics, 2);
	//pFTS->relevantMatch(dics[1]);
	//rr = pFTS->searchPhrase("путин");

	//1100

	//0.027
	//pFTS->searchDistances(dics, 1);
	//pFTS->buildWiki();

	//return;
	for(uint i=0; i < pResult->CountMatches; i++)
	{
		char name[100];
		
		pFTS->getDocumentNameByID(pResult->Matches[i], name, 100);

		printf("%s\n", name);
	}

	//count += rr->CountMatches;
	//pFTS->releaseResult(rr);
	
	finish = clock();

	printf( "=== Speed: %2.1d msec ===\n", (finish - start) );
	printf( "=== Count: %u ===\n", count);
	
	pFTS->stopInstance();
}

void test53MatchRel()
{
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	conf.IN_MEMORY_MODE = true;
	//conf.AUTO_STEMMING_ON = 20;
	//conf.COUNT_WORD_IN_PHRASE = 2;
	
	pFTS->startInstance(conf);

	RelevantResult* pResult;

	char buff[100];
	sprintf(buff, "поисковик спутник");
	
	pResult = pFTS->searchPhraseRel(buff, strlen(buff), 80, pFTS->getInfo()->LastNameIDRAM);
	
	for(uint i=0; i < pResult->CountMatches; i++)
	{
		char name[100];
		
		pFTS->getDocumentNameByID(pResult->Matches[i], name, 100);

		printf("%s\n", name);
	}

	pFTS->stopInstance();
}

void test54MatchQuery()
{
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	conf.IN_MEMORY_MODE = true;
	//conf.AUTO_STEMMING_ON = 20;
	//conf.COUNT_WORD_IN_PHRASE = 2;
	
	pFTS->startInstance(conf);

	Selector* selectors[4];

	uint countSelectors = 0;

	/*selectors[countSelectors].SelectorType = 1;
	selectors[countSelectors].Dictionary.addWord("синий", 0);
	selectors[countSelectors].Dictionary.addWord("зеленый", 0);

	countSelectors++;*/

	selectors[countSelectors] = new Selector("Test");
	selectors[countSelectors]->SelectorType = 4;
	selectors[countSelectors]->RangeType = 1; //numbers
	selectors[countSelectors]->AutoStemmingOn = 8;
	strcpy(selectors[countSelectors]->MinBound, "555");
	strcpy(selectors[countSelectors]->MaxBound, "565");

	countSelectors++;

	/*
	selectors[countSelectors].SelectorType = 2;
	selectors[countSelectors].Dictionary.addWord("синий", 0);
	selectors[countSelectors].Dictionary.addWord("зеленый", 0);

	countSelectors++;

	selectors[countSelectors].SelectorType = 3;
	selectors[countSelectors].Dictionary.addWord("синий", 0);
	selectors[countSelectors].Dictionary.addWord("зеленый", 0);

	countSelectors++;
	*/

	clock_t start, finish;
	start = clock();

	QueryResult* pResult = pFTS->searchQuery(selectors, countSelectors, 80, 1500000);

	finish = clock();

	printf( "=== Speed: %2.1d msec ===\n", (finish - start) );
	
	for(uint i=0; i < pResult->CountRows; i++)
	{
		char name[100];
		
		pFTS->getDocumentNameByID(pResult->QueryRows[i].DocNumber, name, 100);

		printf("%s | %s\n", name, pResult->QueryRows[i].Text);
	}

	pFTS->stopInstance();
}

void test18Index3letters()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS1 = new FTSInstance();

	FTSConfiguration conf1;
	conf1.MIN_LEN_WORD = 3;

	pFTS1->startInstance(conf1);

	char name[100];
	sprintf(name, "zzr");

	testFVMas(pFTS1);
	pFTS1->indexText("test", name, 3);

	pFTS1->saveIndex();

	pFTS1->stopInstance();

	//instance
	FTSInstance* pFTS2 = new FTSInstance();

	FTSConfiguration conf2;
	conf2.IN_MEMORY_MODE = true;

	pFTS2->startInstance(conf2);
	
	RelevantResult* pResult = 0;
		
	pResult = pFTS2->searchPhrase("zzr");
	uint id = pResult->Matches[0];
	pFTS2->getDocumentNameByID(id, name, 100);

	if(*(uint*)name == *(uint*)"test")
	{
		printf("Test 18. OK\n");
	}
	else
	{
		printf("Test 18. Failed\n");
	}

	pFTS2->stopInstance();

	delete pFTS2;
}

void test19LogFile()
{
	FTSInstance* pFTS = new FTSInstance();
	
	BinaryFile::deleteFile("c:\\FTS\\!Log.txt");

	pFTS->logFile("hello");

	if(BinaryFile::existsFile("c:\\FTS\\!Log.txt"))
	{
		printf("Test 19. OK\n");
	}
	else
	{
		printf("Test 19. Failed\n");
	}

	BinaryFile::deleteFile("c:\\FTS\\!Log.txt");
}

void test20_TwoPartsAndMerge()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	pFTS->startInstance(conf);

	//save fvmas
	testFVMas(pFTS);

	pFTS->saveIndex();

	//save ned
	testNED(pFTS);

	pFTS->Configuration.MAX_SIZE_BUFFER = 1000;

	pFTS->saveIndex();

	RelevantResult* pResult = 0;
		
	pResult = pFTS->searchPhrase("базист");
	pFTS->releaseResult(pResult);
	
	uint id1 = pResult->Matches[0];

	pResult = pFTS->searchPhrase("антимишка");
	pFTS->releaseResult(pResult);
	
	uint id2 = pResult->Matches[0];

	if(id1 == 574 && id2 == 1815)
	{
		printf("Test 20. OK\n");
	}
	else
	{
		printf("Test 20. Failed\n");
	}

	pFTS->stopInstance();

	/*for(uint j=0; j<pResult->CountMatches && j < 100; j++)
	{
		uint id = pResult->Matches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	//start = clock();
	//finish = clock();
	//printf( "=== Save index done: %2.1d msec ===\n", (finish - start) );
}

void test21_Tolstoy()
{
	FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	pFTS->startInstance(conf);
	
	testTolstoy(pFTS);
	
	pFTS->stopInstance();
}

void test20()
{
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	
	pFTS->startInstance(conf);
	
	pFTS->printTotalMemory();

	pFTS->stopInstance();
}

void test21()
{
	//FTSInstance::clearInstance("c:\\fts");

	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;

	conf.IN_MEMORY_MODE = false;

	pFTS->startInstance(conf);
	
	/*char text[100];
	strcpy(text, "bbbccc");
	pFTS->indexText("slavik", text, 6);
*/
	RelevantResult* pResult = pFTS->searchPhrase("hello");

	for(uint i=0; i<pResult->CountMatches; i++)
	{
		/*if(pResult->Matches[i] == 971463)
		{
			pResult->Matches[0] = pResult->Matches[0];
		}*/

		uint id = pResult->Matches[i];

		char name[1024];
		pFTS->getDocumentNameByID(id, name, 100);

		bool x = pFTS->hasDocumentName(name, strlen(name));

		printf("Find: %s\n", name);
	}

	uint id = pResult->Matches[0];

	char name[1024];
	pFTS->getDocumentNameByID(id, name, 100);

	printf("Find: %s\n", name);

	ulong indexedSize = 0;

	//testFind(clock(), indexedSize, pFTS, "d:\\Part\\");

	//pFTS->saveIndex();
	
	pFTS->stopInstance();
}

void test22_IndexImport3HDD()
{
	char str[256];

	FTSInstance::clearInstance("C:\\FTS\\Test1");

	//first instance
	FTSInstance* pFTS1 = new FTSInstance();

	FTSConfiguration conf1;
	strcpy(conf1.IndexPath, "C:\\FTS\\Test1");
	
	pFTS1->startInstance(conf1);

	strcpy(str, "one two three");
	pFTS1->indexText("1", str, strlen(str));
	
	pFTS1->saveIndex();
	pFTS1->stopInstance();

	//second instance
	FTSInstance::clearInstance("c:\\FTS\\Test2");

	FTSInstance* pFTS2 = new FTSInstance();

	FTSConfiguration conf2;
	strcpy(conf2.IndexPath, "C:\\FTS\\Test2");

	pFTS2->startInstance(conf2);
	
	strcpy(str, "one four");

	pFTS2->indexText("2", str, strlen(str));
	
	pFTS2->saveIndex();
	pFTS2->stopInstance();

	//third instance
	FTSInstance::clearInstance("c:\\FTS\\Test3");

	FTSInstance* pFTS3 = new FTSInstance();

	FTSConfiguration conf3;
	strcpy(conf3.IndexPath, "C:\\FTS\\Test3");

	pFTS3->startInstance(conf3);

	strcpy(str, "one two five");

	pFTS3->indexText("3", str, strlen(str));
	
	pFTS3->saveIndex();
	pFTS3->stopInstance();

	//merge indexes
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	strcpy(conf.IndexPath, "C:\\FTS\\Test1");
	
	pFTS->startInstance(conf);

	pFTS->importIndex("C:\\FTS\\Test2");
	pFTS->importIndex("C:\\FTS\\Test3");
	
	pFTS->saveIndex();
	pFTS->stopInstance();

	//open indexes
	pFTS = new FTSInstance();

	strcpy(conf.IndexPath, "C:\\FTS\\Test1");
	
	pFTS->startInstance(conf);
		
	//check
	RelevantResult* pResult = 0;
	
	bool result = true;

	pResult = pFTS->searchPhrase("one"); //1 2 3
	if(pResult->Matches[0] != 80 ||
		pResult->Matches[1] != 81 ||
		pResult->Matches[2] != 82)
	{
		result = false;
	}

	pResult = pFTS->searchPhrase("two"); //1 3

	if(pResult->Matches[0] != 80 ||
		pResult->Matches[1] != 82)
	{
		result = false;
	}

	pResult = pFTS->searchPhrase("three"); //1

	if(pResult->Matches[0] != 80)
	{
		result = false;
	}

	pResult = pFTS->searchPhrase("four"); //2

	if(pResult->Matches[0] != 81)
	{
		result = false;
	}

	pResult = pFTS->searchPhrase("five"); //3

	if(pResult->Matches[0] != 82)
	{
		result = false;
	}
	
	if(result)
	{
		printf("Test 22. OK\n");
	}
	else
	{
		printf("Test 22. Failed\n");
	}

	pFTS->stopInstance();

}

void mergeIndexes(char* path1, char* path2)
{
	//first instance
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	strcpy(conf.IndexPath, path1);
	
	conf.IN_MEMORY_MODE = false;
	pFTS->startInstance(conf);

	printf("Start: %s => RAM:%d HDD:%d\n", path2, pFTS->getInfo()->LastNameIDRAM, pFTS->getInfo()->LastNameIDHDD);

	pFTS->importIndex(path2);

	printf("End: %s => RAM:%d HDD:%d\n", path2, pFTS->getInfo()->LastNameIDRAM, pFTS->getInfo()->LastNameIDHDD);

	pFTS->stopInstance();
}

void checkIndex(char* path1)
{
	//first instance
	FTSInstance* pFTS = new FTSInstance();

	FTSConfiguration conf;
	strcpy(conf.IndexPath, path1);
	
	conf.IN_MEMORY_MODE = true;
	pFTS->startInstance(conf, true);

	pFTS->stopInstance();

	/*HArrayFixPair key;

	pFTS->getPartWords("базбазба", 8, key.Key);

	char word[256];
	key.getWord(word);*/

	
}

void mergeInstance1()
{
	mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\skodaclub_wb");
	mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\velokiev_wb");
	mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\dou_wb");
	mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\lambda_wb");
	mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\gamedev_wb");
}

void mergeInstance2()
{
	mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\sevinfo");
}

void mergeInstance3()
{
	//mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes2\\sevinfo1");
	mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes2\\habr");
}

void tests()
{
	//testDir("i:\\dou");

	//test21();

	//checkIndex("c:\\FTS");
	//mergeInstance1();
	//mergeInstance2();
	//mergeInstance3();

	//mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\dou");
	//mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\sql");
	//mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\librusec");
	//mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\SkodaClub");

	//mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\velokiev");

	//mergeIndexes("c:\\FTS", "C:\\FTS\\Indexes\\gamedev");

	//test01_IndexRAM();
	//test02_IndexHDD();
	//test03_Index2RAM();
	//test04_Index2HDD();
	//test05_IndexImport2HDD();
	//test06_WordWithLen10HDD();
	//test07_Index10HDD();
	//test08_RelevantLevel();
	//test09_GetDocNameRAM();
	//test10_GetDocNameHDD();
	//test11_StartStartStopStopInstance();
	//test12_IndexPhraseRAM();
	//test13_Index2HDDDiffInst();
	//test14_CheckPools();
	//test15_InMemory();
	//test16_GetDocNameInMemory();
	////test17_RecoveryIndexBegin();
	////test17_RecoveryIndexAfterReset();
	//test18Index3letters();
	//test19LogFile();
	//test20_TwoPartsAndMerge();
	//test21_Tolstoy();
	//test21();
	//test22_IndexImport3HDD();

	////test50HighLoadIndex();
	////test51HighLoadSearch();
	//test52MatchSearch();

	test54MatchQuery();

	//test53_TwoPartsAndMerge();

	system("pause");
}

int _tmain(int argc, _TCHAR* argv[])
{
	clock_t start, finish;

	tests();
	//testHArrayInt();

	return 0;

	/*conf.DictionaryPath = "C:\\FTS2\\Dictionary.idx";
	conf.DocumentPath = "C:\\FTS2\\Document.idx";
	conf.DocumentNamePath = "C:\\FTS2\\DocumentName.idx";
	conf.DocumentPathTemp = "C:\\FTS2\\Document_Temp.idx";
*/
	
	/*testTolstoy(pFTS);
	pFTS->saveIndex();*/

	/*char buff[1024];
	sprintf(buff, "%s", "киевский политехнический институт");

	pFTS->indexText("bla", buff, strlen(buff));*/

	//save fvmas
	
	/*start = clock();

	pFTS->rebuildIndex();

	finish = clock();

	printf( "Reindex: %2.1d msec\n", (finish - start) );*/

	//add ned in memory
	/*testTolstoy(pFTS);

	pFTS->saveIndex();*/

	//testImport(pFTS);

	//testNED(pFTS);
	
	//pFTS->saveIndex();

	start = clock();
	
	//pFTS->saveIndex();
	
	finish = clock();

	printf( "=== Additional index done: %2.1d msec ===\n", (finish - start) );

	//ulong code = pFTS->getCode("", 5);


	start = clock();

	
		
	finish = clock();

	printf( "Search done (10000 searches): %2.1d msec\n", (finish - start) );
	
	
	printf( "Best matches:\n");

	

	printf( "Other matches:\n");

	/*for(uint j=0; j<pResult->CountOtherMatches && j < 100; j++)
	{
		uint id =  pResult->OtherMatches[j];
		char name[1024];
		pFTS->getDocumentNameByID(id, name, 1024);

		printf("%s\n", name);
	}*/
	
	return 0;
};