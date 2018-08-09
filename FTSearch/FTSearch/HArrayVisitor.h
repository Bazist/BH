#pragma once

#ifndef HARRAYVISITOR_HEADER		// Allow use of features specific to Windows XP or later.                   
#define HARRAYVISITOR_HEADER 1451	// Change this to the appropriate value to target other versions of Windows.

#endif

class HArrayVisitor
{
public:
	virtual void onStartScan()
	{
	}

	virtual bool onVisit(uint32* key, ulong64 value)
	{
		return false;
	}

	virtual void onEndScan()
	{
	}

	static void getWord(char* word,
					   const uint32* key,
					   uint32 maxKeyLen)
	{
		uint32 len = 0;

		uint32* words = (uint32*)word;
		
		uint32 i = 0;
		uint32 j = 0;

		while(j < maxKeyLen)
		{
			if(key[i] & 0xFF000000)
			{
				words[i] = key[i];

				j+=4;
				i++;
			}
			else
			{
				break;
			}
		}

		if(j < maxKeyLen)
		{
			if(key[i] & 0x00FF0000)
			{
				word[j++] = (char)(key[i] << 8 >> 24);	
			}

			if(key[i] & 0x0000FF00)
			{
				word[j++] = (char)(key[i] << 16 >> 24);
			}

			if(key[i] & 0x000000FF)
			{
				word[j++] = (char)(key[i] << 24 >> 24);
			}
		}

		word[j++] = 0;
	}

	static void getPartWords(const char* word,
							 uint32 wordLen,
							 uint32* key,
							 uint32 maxKeyLen)
	{
		//Fill key
		uint32* words = (uint32*)word;

		if (wordLen > maxKeyLen)
		{
			wordLen = maxKeyLen;
		}

		//main len
		uint32 count = wordLen >> 2; // the same count = len / 4

		uint32 i;
		for (i = 0; i < count; i++)
		{
			key[i] = words[i];
		}

		//rest len, clear
		uint32 maxLen = maxKeyLen >> 2; //the same maxKeyLen /= 4

		for (uint32 i = count; i < maxLen; i++)
		{
			key[i] = 0;
		}

		//fill last digits
		for (i = count << 2; i < wordLen; i++)
		{
			key[count] = (key[count] << 8) | (uchar8)word[i];
		}
	}
};

