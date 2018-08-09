#include <stdio.h>
#include "stdafx.h"
#include "FTSInstance.h"

//#ifdef EXPORT_METHOD
#define EXPORT_METHOD extern "C" __declspec(dllexport) 
//#else
//#define EXPORT_METHOD __declspec(dllimport) 
//#endif

EXPORT_METHOD void __stdcall startInstance(FTSConfiguration configuration, bool onlyCheckIndex);

/*EXPORT_METHOD void startDefaultInstance(const char* path,
										uint32 instance,
										bool inMemoryMode,
										uint32 autoStemmingOn,
										uint32 minLenWord,
										uint32 maxLenWord,
										uint32 documentNameSize,
										uint32 countWordInPhrase,
										bool isUseNumberAlphabet);*/

EXPORT_METHOD RelevantResult* __stdcall searchPhrase(uint32 instanceNumber,
											const char* phrase,
											uint32 phraseLen,
											const char* templateName, uint32 templateNameLen, uint32 minPage, uint32 maxPage, uint32 skip);

EXPORT_METHOD RelevantResult* __stdcall searchPhraseRel(uint32 instanceNumber, const char* phrase, uint32 phraseLen, uint32 minPage, uint32 maxPage);

EXPORT_METHOD QueryResult* __stdcall searchQuery(uint32 instanceNumber, Selector** selectors, uint32 countSelectors, uint32 minPage, uint32 maxPage, uint32 skip, bool agregateBySubject);

EXPORT_METHOD RelevantResult* __stdcall searchNewMems(uint32 instanceNumber,
											uint32 startDate1Year,
										    uint32 startDate1Month,
										    uint32 startDate2Year,
										    uint32 startDate2Month,
										    uint32 startDate2Day,
										    uint32 endDate2Year,
										    uint32 endDate2Month,
										    uint32 endDate2Day,
										    uint32 periodInDays);

EXPORT_METHOD void __stdcall releaseRelevantResult(uint32 instanceNumber, RelevantResult* pRelevantResult);

EXPORT_METHOD void __stdcall releaseQueryResult(uint32 instanceNumber, QueryResult* pQueryResult);

EXPORT_METHOD FTSInstanceInfo* __stdcall getInfo(uint32 instanceNumber);

EXPORT_METHOD void __stdcall calculateTrend(uint32 instanceNumber,
								  const char* phrase,
								  uint32 phraseLen,
								  uint32* points,
								  uint32 count,
								  uint32 minPage,
								  uint32 maxPage);

EXPORT_METHOD RelevantResult* __stdcall searchHtmlSeldomWords(uint32 instanceNumber, char* text,	uint32 textLen);

EXPORT_METHOD bool __stdcall indexText(uint32 instanceNumber, const char* name, uint32 nameLen, char* text, uint32 textLen);

EXPORT_METHOD bool __stdcall indexHtml(uint32 instanceNumber, const char* name, uint32 nameLen, char* text, uint32 textLen);

EXPORT_METHOD void __stdcall getDocumentNameByID(uint32 instanceNumber, uint32 id, char* name, uint32 sizeName);

EXPORT_METHOD void __stdcall clearInstance();

EXPORT_METHOD void __stdcall saveIndex(uint32 instanceNumber);

EXPORT_METHOD void __stdcall importIndex(uint32 instanceNumber,
							   const char* importPath,
							   bool isDeleteImportedIndex);

EXPORT_METHOD void __stdcall stopInstance(uint32 instanceNumber);