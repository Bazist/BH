#include "StdAfx.h"
#include "PostSelector.h"

Selector* PostSelector::Selectors[MAX_SHORT];
char* PostSelector::Words[MAX_SHORT];

uint32 PostSelector::CountWords = 0;
