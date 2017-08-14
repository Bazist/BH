#include "StdAfx.h"
#include <time.h>
#include "FTSInstance.h"

void FTSInstance::logFile(const char* text)
{
	FILE* pFile = fopen("C:\\FTS\\!Log.txt", "a");
	if(pFile)
	{
		//get current time
		time_t rawtime;
		struct tm * timeinfo;

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		char message[1024];
		sprintf(message,
				"%u-%u-%u %u:%u => %s\r\n", 
				timeinfo->tm_mday, 
				timeinfo->tm_mon, 
				timeinfo->tm_year, 
				timeinfo->tm_hour, 
				timeinfo->tm_min, 
				text);
		
		fwrite(message, 1, strlen(message), pFile);

		fclose(pFile);
	}
}

void FTSInstance::logError(const char* text, const char* param)
{
	sprintf(Info.LastErrorMessage, text, param);
	printf(Info.LastErrorMessage);
	printf("\n");

	logFile(Info.LastErrorMessage);

	Info.HasError = true;
}

void FTSInstance::logError(const char* text, uint32 param)
{
	sprintf(Info.LastErrorMessage, text, param);
	printf(Info.LastErrorMessage);
	printf("\n");

	logFile(Info.LastErrorMessage);

	Info.HasError = true;
}

void FTSInstance::logError(const char* text)
{
	sprintf(Info.LastErrorMessage, text);
	printf(Info.LastErrorMessage);
	printf("\n");

	logFile(Info.LastErrorMessage);

	Info.HasError = true;
}

void FTSInstance::logWarning(const char* text, const char* param)
{
	sprintf(Info.LastErrorMessage, text, param);
	printf(Info.LastErrorMessage);
	printf("\n");

	logFile(Info.LastErrorMessage);
}

void FTSInstance::logWarning(const char* text, uint32 param)
{
	sprintf(Info.LastErrorMessage, text, param);
	printf(Info.LastErrorMessage);
	printf("\n");

	logFile(Info.LastErrorMessage);
}

void FTSInstance::logWarning(const char* text)
{
	sprintf(Info.LastErrorMessage, text);
	printf(Info.LastErrorMessage);
	printf("\n");

	logFile(Info.LastErrorMessage);
}

void FTSInstance::checkKeyAndValue(const char* text,
								ulong64 prevKey,
								ulong64 currKey,
								uint32 currValue,
								uint32 maxValue)
{
	if(currValue > maxValue)
	{
		char message[1024];
		sprintf(message, "%s. Max value is exceed. Key: %llu, Value: %u, MaxValue: %u", text, currKey, currValue, maxValue);

		logError(message);
	}

	if(prevKey > currKey)
	{
		char message[1024];
		sprintf(message, "%s. Key is not sorted. Prev Key: %llu, Curr Key: %u", text, prevKey, currKey);

		logError(message);
	}
}

ulong64 FTSInstance::getUsedMemory()
{
	ulong64 bytes = pBlockMemoryPool->getUsedMemory() +
				 pDocumentsBlockPool->getUsedMemory() +
				 pRelevantResultPool->getUsedMemory() +
				 pPostSelectorPool->getUsedMemory() +
				 documentsName.getUsedMemory() +
				 haWordsRAM.getUsedMemory() +
				 sizeof(FTSInstance);

	return bytes;
}

ulong64 FTSInstance::getTotalMemory()
{
	ulong64 bytes = pBlockMemoryPool->getTotalMemory() +
				 pDocumentsBlockPool->getTotalMemory() +
				 pRelevantResultPool->getTotalMemory() +
				 pPostSelectorPool->getTotalMemory() +
				 documentsName.getTotalMemory() +
				 haWordsRAM.getTotalMemory() +
				 sizeof(FTSInstance);

	return bytes;
}

void FTSInstance::printTotalMemory()
{
	printf("===================================================================\n");
	printf("pBlockMemoryPool = %u\n", pBlockMemoryPool->getTotalMemory());
	printf("pDocumentsBlockPool = %u\n", pDocumentsBlockPool->getTotalMemory());
	printf("pRelevantResultPool = %u\n", pRelevantResultPool->getTotalMemory());
	printf("pDocumentsName = %u\n", documentsName.getTotalMemory());
	printf("===================================================================\n");
	/*printf("haWordsRAM = %u\n", haWordsRAM.getTotalMemory());
	printf("haWordsRAM.Header = %u\n", haWordsRAM.getHeaderMemory());
	printf("haWordsRAM.Double = %u\n", haWordsRAM.getDoublePageMemory());
	printf("haWordsRAM.Multiply = %u\n", haWordsRAM.getMultiplyPageMemory());*/
	printf("===================================================================\n");
}

uint32 FTSInstance::getSearchPoolsMemory()
{
	uint32 bytes = pRelevantResultPool->Count;

	return bytes;
}