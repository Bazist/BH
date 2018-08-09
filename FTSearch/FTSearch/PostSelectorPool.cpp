#include "StdAfx.h"
#include "PostSelectorPool.h"

PostSelector** PostSelectorPool::pFreePostSelectorLists[MAX_POOL_PAGES];
PostSelector* PostSelectorPool::pPostSelectorLists[MAX_POOL_PAGES];

uint PostSelectorPool::Count = 0;
uint PostSelectorPool::Size = 0;
char* PostSelectorPool::pLastErrorMessage = 0;
