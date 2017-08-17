#include "StdAfx.h"
#include "BinaryFile.h"

BinaryFile::BinaryFile(const char* fileName, 
					   bool isWritable, 
					   bool isOverwrite)
{
	init(fileName, isWritable, isOverwrite, 0);
}

BinaryFile::BinaryFile(const char* fileName, 
					   bool isWritable, 
					   bool isOverwrite,
					   uint32 bufferSize)
{
	init(fileName, isWritable, isOverwrite, bufferSize);
}

void BinaryFile::init(const char* fileName, 
					  bool isWritable, 
				      bool isOverwrite,
					  uint32 bufferSize)
{
	strcpy(m_fileName, fileName);

	m_isWritable = isWritable;
	m_isOverwrite = isOverwrite;
	m_file = 0;
	m_countReads = 0;
	m_fileSize = 0;

	if(bufferSize)
	{
		m_countPages = 0;
		m_maxCountPages = bufferSize / BIN_FILE_RIGHT_MASK + 1;

		m_pagesSize = 8;
		pPages = new BinaryFilePage*[m_pagesSize];

		memset(pPages, 0, sizeof(BinaryFilePage*) * m_pagesSize);
	}
	else
	{
		m_countPages = 0;
		m_maxCountPages = 0;
		
		m_pagesSize = 0;
		pPages = 0;
	}
}

bool BinaryFile::open()
{
	errno_t r;

	if(!m_isWritable)
		r = fopen_s(&m_file, m_fileName, "rb");
	else if(!m_isOverwrite)
		r = fopen_s(&m_file, m_fileName, "r+b");
	else
		r = fopen_s(&m_file, m_fileName, "w+b");

	if (r == 0)
	{
		m_fileSize = getFileSize();

		return true;
	}
	else
	{
		return false;
	}
}

bool BinaryFile::clear()
{
	close();
	
	BinaryFile::deleteFile(m_fileName);

	m_isWritable = true;
	m_isOverwrite = true;

	return open();
}

void BinaryFile::close()
{
	if(m_file)
	{
		if(m_countPages)
		{
			deletePages(true);
		}

		fclose(m_file);

		m_file = 0;
	}
}

uint32 BinaryFile::read(void* pData, ulong64 position, uint32 length)
{
	_fseeki64 (m_file , position , SEEK_SET);
	uint32 n = fread(pData, 1, length, m_file);

	return n;
}

uint32 BinaryFile::readBuffered(void* pData, ulong64 position, uint32 length)
{
	if (!m_maxCountPages)
	{
		return read(pData, position, length);
	}

	if (position + length > m_fileSize)
	{
		ulong64 allocateSize = m_fileSize - position - length;

		allocate(allocateSize, m_fileSize);

		m_fileSize += allocateSize;
	}

	//return read(pData, position, length);
	
	ulong64 page1 = (position >> BIN_FILE_RIGHT_BITS);
	ulong64 page2 = ((position + length) >> BIN_FILE_RIGHT_BITS);

	uint32 index = (position & BIN_FILE_RIGHT_MASK);

	if(page1 == page2)
	{
		//reallocate page
		if(page1 >= m_pagesSize)
		{
			reallocatePages(page1);
		}

		BinaryFilePage* pPage1 = pPages[page1];

		//read page
		if(!pPage1)
		{
			//delete old pages
			if(m_countPages >= m_maxCountPages)
			{
				deletePages(false);
			}
			
			pPage1 = new BinaryFilePage();

			//printf("read: %d\n", (page1 << 16));

			uint32 n = read(pPage1->pContent, page1 << BIN_FILE_RIGHT_BITS, BIN_FILE_RIGHT_MASK + 1);

			pPages[page1] = pPage1;

			m_countPages++;
		}

		//read item
		char* pDataChar = (char*)pData;

		for(uint32 i=0; i<length; i++, index++)
		{
			pDataChar[i] = pPage1->pContent[index];
		}

		pPage1->CountReads++;
		m_countReads++;
	}
	else
	{
		//reallocate page
		if(page1 >= m_pagesSize)
		{
			reallocatePages(page1);
		}

		BinaryFilePage* pPage1 = pPages[page1];

		//read first page
		if(!pPage1)
		{
			//delete old pages
			if(m_countPages >= m_maxCountPages)
			{
				deletePages(false);
			}

			pPage1 = new BinaryFilePage();

			//printf("read: %d\n", (page1 << 16));

			uint32 n = read(pPage1->pContent, page1 << BIN_FILE_RIGHT_BITS, BIN_FILE_RIGHT_MASK + 1);

			pPages[page1] = pPage1;

			m_countPages++;
		}

		//read item
		char* pDataChar = (char*)pData;

		uint32 i=0;

		for(; index < BIN_FILE_RIGHT_MASK + 1; i++, index++)
		{
			pDataChar[i] = pPage1->pContent[index];
		}

		pPage1->CountReads++;
		m_countReads++;

		//reallocate page
		if(page2 >= m_pagesSize)
		{
			reallocatePages(page2);
		}

		BinaryFilePage* pPage2 = pPages[page2];

		//read second page
		if(!pPage2)
		{
			//delete old pages
			if(m_countPages >= m_maxCountPages)
			{
				deletePages(false);
			}

			pPage2 = new BinaryFilePage();

			//printf("read: %d\n", (page2 << 16));

			uint32 n = read(pPage2->pContent, page2 << BIN_FILE_RIGHT_BITS, BIN_FILE_RIGHT_MASK + 1);

			pPages[page2] = pPage2;

			m_countPages++;
		}

		for(index = 0; i < length; i++, index++)
		{
			pDataChar[i] = pPage2->pContent[index];
		}

		pPage2->CountReads++;
		m_countReads++;
	}
	
	return length;
}

uint32 BinaryFile::read(void* pData, uint32 length)
{
	uint32 n = fread(pData, 1, length, m_file);

	return n;
}

uint32 BinaryFile::write(const void* pData, ulong64 position, uint32 length)
{
	uint32 n = _fseeki64 (m_file, position, SEEK_SET);
	n = fwrite(pData, 1, length, m_file);

	return n;
}

uint32 BinaryFile::writeBuffered(const void* pData, ulong64 position, uint32 length)
{
	if (!m_maxCountPages)
	{
		return write(pData, position, length);
	}

	if (position + length > m_fileSize)
	{
		ulong64 allocateSize = m_fileSize - position - length;

		allocate(allocateSize, m_fileSize);

		m_fileSize += allocateSize;
	}

	//write(pData, position, length);
	//return;

	ulong64 page1 = (position >> BIN_FILE_RIGHT_BITS);
	ulong64 page2 = ((position + length) >> BIN_FILE_RIGHT_BITS);

	uint32 index = (position & BIN_FILE_RIGHT_MASK);

	if(page1 == page2)
	{
		//reallocate page
		if(page1 >= m_pagesSize)
		{
			reallocatePages(page1);
		}

		BinaryFilePage* pPage1 = pPages[page1];

		//read page
		if(!pPage1)
		{
			//delete old pages
			if(m_countPages >= m_maxCountPages)
			{
				deletePages(false);
			}
			
			pPage1 = new BinaryFilePage();

			//printf("read: %d\n", (page1 << 16));

			uint32 n = read(pPage1->pContent, page1 << BIN_FILE_RIGHT_BITS, BIN_FILE_RIGHT_MASK + 1);

			pPages[page1] = pPage1;

			m_countPages++;
		}

		//save item
		char* pDataChar = (char*)pData;

		for(uint32 i=0; i<length; i++, index++)
		{
			pPage1->pContent[index] = pDataChar[i];
		}

		pPage1->NeedSave = true;

		pPage1->CountReads++;
		m_countReads++;
	}
	else
	{
		BinaryFilePage* pPage1 = pPages[page1];

		//reallocate page
		if(page1 >= m_pagesSize)
		{
			reallocatePages(page1);
		}

		//read first page
		if(!pPage1)
		{
			//delete old pages
			if(m_countPages >= m_maxCountPages)
			{
				deletePages(false);
			}

			pPage1 = new BinaryFilePage();

			//printf("read: %d\n", (page1 << 16));

			uint32 n = read(pPage1->pContent, page1 << BIN_FILE_RIGHT_BITS, BIN_FILE_RIGHT_MASK + 1);

			pPages[page1] = pPage1;

			m_countPages++;
		}

		//write item
		char* pDataChar = (char*)pData;

		uint32 i=0;

		for(; index < BIN_FILE_RIGHT_MASK + 1; i++, index++)
		{
			pPage1->pContent[index] = pDataChar[i];
		}

		pPage1->NeedSave = true;

		pPage1->CountReads++;
		m_countReads++;

		//reallocate page
		if(page2 >= m_pagesSize)
		{
			reallocatePages(page2);
		}

		BinaryFilePage* pPage2 = pPages[page2];

		//read second page
		if(!pPage2)
		{
			//delete old pages
			if(m_countPages >= m_maxCountPages)
			{
				deletePages(false);
			}

			pPage2 = new BinaryFilePage();

			//printf("read: %d\n", (page2 << 16));

			uint32 n = read(pPage2->pContent, page2 << BIN_FILE_RIGHT_BITS, BIN_FILE_RIGHT_MASK + 1);

			pPages[page2] = pPage2;

			m_countPages++;
		}

		for(index = 0; i < length; i++, index++)
		{
			pPage2->pContent[index] = pDataChar[i];
		}

		pPage2->NeedSave = true;

		pPage1->CountReads++;
		m_countReads++;
	}

	return 0;
}

uint32 BinaryFile::write(const void* pData, uint32 length)
{
	return fwrite(pData, 1, length, m_file);
}

bool BinaryFile::readInt(uint32* pValue, 
						 ulong64 position)
{
	_fseeki64 (m_file , position , SEEK_SET);
	size_t n = fread (pValue, 4, 1, m_file);

	return (n > 0);
}

bool BinaryFile::readLong(ulong64* pValue, 
						  ulong64 position)
{
	_fseeki64 (m_file , position , SEEK_SET);
	size_t n = fread (pValue, 8, 1, m_file);

	return (n > 0);
}

bool BinaryFile::readInt(uint32* pValue)
{
	size_t n = fread (pValue, 4, 1, m_file);

	return (n > 0);
}

bool BinaryFile::readLong(ulong64* pValue)
{
	size_t n = fread (pValue, 8, 1, m_file);

	return (n > 0);
}

bool BinaryFile::readInts(uint32* pValues, 
						  uint32 length)
{
	size_t n = fread (pValues, 4, length, m_file);

	return (n > 0);
}

void BinaryFile::writeInt(const uint32* pValue)
{
	fwrite (pValue, 4, 1, m_file);
}

void BinaryFile::writeLong(const ulong64* pValue)
{
	fwrite (pValue, 8, 1, m_file);
}

void BinaryFile::writeInt(const uint32* pValue, 
						  const ulong64 position)
{
	_fseeki64 (m_file , position , SEEK_SET);
	fwrite (pValue, 4, 1, m_file);
}

void BinaryFile::writeLong(const ulong64* pValue, 
						   const ulong64 position)
{
	_fseeki64 (m_file , position , SEEK_SET);
	fwrite (pValue, 8, 1, m_file);
}

void BinaryFile::writeInts(const uint32* pValues, 
						   const uint32 length)
{
	fwrite (pValues, 4, length, m_file);
}

void BinaryFile::deletePages(bool isAll)
{
	//printf("flush: ");

	if(m_countPages)
	{
		uint32 averageCountReads = m_countReads / m_countPages;

		for(ulong64 i=0; i < m_pagesSize; i++)
		{
			BinaryFilePage* pPage = pPages[i];

			if(pPage)
			{
				if((pPage->CountReads <= averageCountReads) || isAll)
				{
					if(pPage->NeedSave)
					{
						//printf("%d ", page);

						uint32 n = write(pPage->pContent, i << BIN_FILE_RIGHT_BITS, BIN_FILE_RIGHT_MASK + 1);
					}

					delete pPage;

					pPages[i] = 0;

					m_countPages--;
				}
				else
				{
					pPage->CountReads = 0;
				}
			}
		}

		//printf("\n");

		m_countReads = 0;
	}
}

void BinaryFile::reallocatePages(uint32 page)
{
	uint32 newSizeContentPages = m_pagesSize;

	while(page >= newSizeContentPages)
	{
		newSizeContentPages *= 2;
	}

	BinaryFilePage** pTempPages = new BinaryFilePage*[newSizeContentPages];
		
	uint32 j=0;
	for(; j < m_pagesSize ; j++)
	{
		pTempPages[j] = pPages[j];
	}

	for(; j < newSizeContentPages ; j++)
	{
		pTempPages[j] = 0;
	}

	delete[] pPages;
	pPages = pTempPages;

	m_pagesSize = newSizeContentPages;
}

void BinaryFile::setPosition(ulong64 position)
{
	_fseeki64 (m_file, position, SEEK_SET);
}

ulong64 BinaryFile::getPosition()
{
	return _ftelli64(m_file);
}

const char* BinaryFile::getFilePath()
{
	return m_fileName;
}

BinaryFile::~BinaryFile()
{
	if(m_countPages)
	{
		deletePages(true);
	}

	delete[] pPages;
}
