// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
					

#include <tchar.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifdef _WIN32
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef unsigned __int64 ulong64;
typedef unsigned __int16 ushort16;
typedef unsigned __int8 uchar8;
typedef unsigned __int8 ucode8;
#endif

#ifdef linux
typedef int32_t int32;
typedef uint32_t uint32;
typedef uint64_t ulong64;
typedef uint16_t ushort16;
typedef uint8_t uchar8;
typedef uint8_t ucode8;
#endif


const uint32 BIN_FILE_RIGHT_BITS = 12; //4096 Byte page
const uint32 BIN_FILE_RIGHT_MASK = (1<<BIN_FILE_RIGHT_BITS) - 1;

const uint32 MAX_SHORT = 65536;
const uint32 MAX_INT = 0xFFFFFFFF;
const uint32 MAX_CHAR = 256;

const uint32 MAX_PAGES = 10000;
const uint32 PAGE_SIZE = MAX_SHORT;
const uchar8 BLOCK_SIZE = 16;
const uchar8 SAFE_BLOCK_SIZE = BLOCK_SIZE - 8;
const uint32 INIT_MAX_PAGES = 8;

const uint32 MAX_FOUND_DOCUMENTS_IN_QUERY = 25;
const uint32 MAX_WORDS_IN_QUERY = 10;
// TODO: reference additional headers your program requires here

const uint32 MAX_POOL_PAGES = 10000;

const uint32 MAX_ROW_LEN = 16;
const uint32 DOC_NAME_HEADER_SIZE = 8;
const uint32 MIN_RELEVANT_BLOCK = 64;

const uint32 BIN_FILE_BUFFER_SIZE = 1024 * 1024 * 1024;

