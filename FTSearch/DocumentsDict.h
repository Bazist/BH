#pragma once

#ifndef DOCUMENTS_DICT_HEADER		// Allow use of features specific to Windows XP or later.                   
#define DOCUMENTS_DICT_HEADER 3333	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "stdafx.h"
#include "BinaryFile.h"

class DocumentsDict
{
public:
	DocumentsDict()
	{
		PrevKey = 0;
		PrevValue = 0;

		CurrentHeaderPosition = 0;
	}

	void resetPrevValues()
	{
		PrevKey = 0;
		PrevValue = 0;

		IsKeysSorted = false;
		IsValuesSorted = false;
	}

	void writeValue(uchar8* pBuffer,
				  uint32& currentSize, 
				  ulong64 value, 
				  ulong64 prevValue)
	{
		ulong64 deltaValue = value - prevValue; 
		if(deltaValue < MAX_CHAR)
		{
			//*pCurrentHeader |= 1<<(CurrentHeaderPosition*2);
				
			pBuffer[currentSize++] = deltaValue;
		}
		else if(deltaValue < MAX_SHORT)
		{
			*pCurrentHeader |= 1<<(CurrentHeaderPosition*2);

			pBuffer[currentSize++] = (deltaValue>>8);
			pBuffer[currentSize++] = (deltaValue&0xFF);
		}
		else if(deltaValue < MAX_INT)
		{
			*pCurrentHeader |= 2<<(CurrentHeaderPosition*2);
			
			pBuffer[currentSize++] = (deltaValue<<32>>56);
			pBuffer[currentSize++] = (deltaValue<<40>>56);
			pBuffer[currentSize++] = (deltaValue<<48>>56); //((uchar8*)&deltaValue)[2];
			pBuffer[currentSize++] = (deltaValue&0xFF);
		}
		else
		{
			*pCurrentHeader |= 3<<(CurrentHeaderPosition*2);
			
			pBuffer[currentSize++] = (deltaValue>>56);
			pBuffer[currentSize++] = (deltaValue<<24>>56);
			pBuffer[currentSize++] = (deltaValue<<32>>56);
			pBuffer[currentSize++] = (deltaValue<<40>>56);
			pBuffer[currentSize++] = (deltaValue<<48>>56);
			pBuffer[currentSize++] = (deltaValue&0xFF);
		}

		if(CurrentHeaderPosition == 3)
		{
			CurrentHeaderPosition = 0;
		}
		else
		{
			CurrentHeaderPosition++;
		}
	}

	void writeKeyValue(BinaryFile* pBinaryFile,
					  uchar8* pBuffer,
					  uint32 maxLength,
					  uint32& buffFilledLength,
					  ulong64 key, 
					  ulong64 value)
	{
		//create header
		if(!CurrentHeaderPosition)
		{
			pCurrentHeader = &pBuffer[buffFilledLength++];
			*pCurrentHeader = 0;
		}
		
		writeValue(pBuffer, buffFilledLength, key, PrevKey);
		writeValue(pBuffer, buffFilledLength, value, PrevValue);

		if(IsKeysSorted) //delta compression
		{
			PrevKey = key;
		}
		else
		{
			PrevKey = 0;
		}

		if(IsValuesSorted) //delta compression
		{
			PrevValue = value;
		}
		else
		{
			PrevValue = 0;
		}

		if(buffFilledLength > (maxLength - 50)
		   && !CurrentHeaderPosition)
		{
			//save to file
			pBinaryFile->write(pBuffer, buffFilledLength);

			buffFilledLength = 0;
		}
	}

	bool readValue(BinaryFile* pBinaryFile,
					uchar8* pBuffer,
					uint32 maxLength,
					uint32& position,
					uint32& currPosition,
					ulong64& value,
					ulong64 prevValue)
	{
		if(!CurrentHeaderPosition)
		{
			pCurrentHeader = &pBuffer[currPosition++];
		}

		//get type
		uint32 header = *pCurrentHeader;
		uint32 type =  header<<(30 - CurrentHeaderPosition*2)>>30;

		//get size
		uint32 valueSize;
		switch(type)
		{
			case 0:
				valueSize = 1;
				break;
			case 1:
				valueSize = 2;
				break;
			case 2:
				valueSize = 4;
				break;
			case 3:
				valueSize = 6;
				break;
		}
					
		//get value
		value = 0;
		for(uint32 j=0; j < valueSize; j++)
		{
			value = (value<<8) | pBuffer[currPosition++];
		}

		if(CurrentHeaderPosition == 3)
		{
			CurrentHeaderPosition = 0;
		}
		else
		{
			CurrentHeaderPosition++;
		}

		value += prevValue;
		
		return true;
	}

	bool readKeyValue(BinaryFile* pBinaryFile,
					  uchar8* pBuffer,
					  uint32 maxLength,
					  uint32& position,
					  uint32& currPosition,
					  ulong64& key, 
					  ulong64& value,
					  uint32 index)
	{
		if(index == 0 || 
		   (currPosition > 950 && !CurrentHeaderPosition))
		{
			position += currPosition;
			
			uint32 len = pBinaryFile->read(pBuffer, position, maxLength);
			
			currPosition = 0;
		}

		bool result;
		//read key
		result = readValue(pBinaryFile,
							pBuffer,
							maxLength,
							position,
							currPosition,
							key,
							PrevKey);

		if(IsKeysSorted) //delta compression
		{
			PrevKey = key;
		}
		else
		{
			PrevKey = 0;
		}

		//read value
		result = readValue(pBinaryFile,
							pBuffer,
							maxLength,
							position,
							currPosition,
							value,
							PrevValue);

		if(IsValuesSorted) //delta compression
		{
			PrevValue = value;
		}
		else
		{
			PrevValue = 0;
		}

		return true;
	}

	ulong64 PrevKey;
	ulong64 PrevValue;

	uchar8* pCurrentHeader;
	
	uchar8 CurrentHeaderPosition;

	bool IsKeysSorted;
	bool IsValuesSorted;

	~DocumentsDict()
	{
		
	}
};