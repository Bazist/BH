#include "StdAfx.h"
#include "FTSInstance.h"

uchar8* FTSInstance::pBuffer = 0;
uint32* FTSInstance::pUsedDocNumbers = 0;
uint32* FTSInstance::pResultDocNumbers = 0;

BlockMemoryPool* FTSInstance::pBlockMemoryPool = 0;
DocumentsBlockPool* FTSInstance::pDocumentsBlockPool = 0;
DocumentsInfoPool* FTSInstance::pDocumentsInfoPool = 0;
RelevantResultPool* FTSInstance::pRelevantResultPool = 0;
PostSelectorPool* FTSInstance::pPostSelectorPool = 0;

char FTSInstance::LastErrorMessage[1024]; //last error message in instance

