#include "HArrayFixBase.h"
#include "HArrayVisitor.h"
#include "HArrayTextFile.h"

class HArrayFixRAM
{
public:
	HArrayFixRAM()
	{
		memset(this, 0, sizeof(HArrayFixRAM));
	}

	uint32 ContentPagesCount;
	uint32 BranchPagesCount;
	uint32 BlockPagesCount;

	uint32 ContentPagesSize;
	uint32 BranchPagesSize;
	uint32 BlockPagesSize;

	HeaderCell* pHeader;
	
	/*uint32* pActiveContent;
	ContentTypeCell* pActiveContentType;
	BranchCell* pActiveBranch;
	BlockCell* pActiveBlock;*/
	
	ContentPage** pContentPages;
	BranchPage** pBranchPages;
	BlockPage** pBlockPages;

	uint32 HeaderBase;
	uint32 HeaderBits;
	uint32 HeaderSize;

	uint32 freeBranchCells[MAX_SHORT];
	uint32 countFreeBranchCell;

	char Path[1024];
	char TableName[1024];

	uint32 KeyLen;
	uint32 ValueLen;
	uint32 RowLen;

	uint32 MAX_SAFE_SHORT;

	uint32 lastContentOffset;
	uint32 lastBranchOffset;
	uint32 lastBlockOffset;

	void init(const char* path,
			  const char* tableName,
			  uint32 keyLen,
			  uint32 valueLen, 
			  uchar8 headerBase)
	{
		init(path,
			 tableName,
			 keyLen,
			 valueLen,
			 headerBase,
			 INIT_MAX_PAGES,
			 INIT_MAX_PAGES,
			 INIT_MAX_PAGES);
	}

	void init(const char* path,
			  const char* tableName,
			  uint32 keyLen,
			  uint32 valueLen, 
			  uchar8 headerBase,
			  uint32 contentPagesSize,
			  uint32 branchPagesSize,
			  uint32 blockPagesSize)
	{
		strcpy(Path, path);
		strcpy(TableName, tableName);

		if(keyLen & 0x3)
		{
			KeyLen = keyLen / 4 + 1; //len in blocks
		}
		else
		{
			KeyLen = keyLen / 4;
		}

		if(valueLen & 0x3)
		{
			ValueLen = valueLen / 4 + 1;
		}
		else
		{
			ValueLen = valueLen / 4;
		}

		RowLen = KeyLen + ValueLen;

		HeaderBase = headerBase;
		HeaderBits = 32-headerBase;
		HeaderSize = (0xFFFFFFFF>>HeaderBits) + 1;

		countFreeBranchCell = 0;

		MAX_SAFE_SHORT = MAX_SHORT - keyLen - valueLen - 1;

		pHeader = new HeaderCell[HeaderSize];
		memset(pHeader, 0, HeaderSize * sizeof(HeaderCell));
		
		#ifndef _RELEASE

		for(uint32 i=0; i<COUNT_TEMPS; i++)
		{
			tempValues[i] = 0;
			tempCaptions[i] = 0;
		}

		tempCaptions[0] = "Moves Level1";
		tempCaptions[1] = "Moves Level2";
		tempCaptions[2] = "Moves Level3";
		tempCaptions[3] = "Moves Level4";
		tempCaptions[4] = "Amount Blocks";
		tempCaptions[5] = "Fill Blocks 1..64";
		tempCaptions[6] = "Fill Blocks 64..128";
		tempCaptions[7] = "Fill Blocks 128..192";
		tempCaptions[8] = "Fill Blocks 192..256";
		tempCaptions[9] = "Next blocks";
		tempCaptions[10] = "Short way";
		tempCaptions[11] = "Long way";
		tempCaptions[12] = "CURRENT_VALUE_TYPE";
		tempCaptions[13] = "ONLY_CONTENT_TYPE ";
		tempCaptions[14] = "MAX_BRANCH_TYPE1  ";
		tempCaptions[15] = "MAX_BLOCK_TYPE    ";
		tempCaptions[16] = "Fill Blocks 1..16";
		tempCaptions[17] = "Fill Blocks 16..32";
		tempCaptions[18] = "Fill Blocks 32..48";
		tempCaptions[19] = "Fill Blocks 48..64";
		tempCaptions[20] = "Fill Blocks 1..8";
		tempCaptions[21] = "Next Block Level 3";

		#endif

		pContentPages = new ContentPage*[contentPagesSize];
		pBranchPages = new BranchPage*[branchPagesSize];
		pBlockPages = new BlockPage*[blockPagesSize];

		memset(pContentPages, 0, contentPagesSize * sizeof(ContentPage*));
		memset(pBranchPages, 0, branchPagesSize * sizeof(BranchPage*));
		memset(pBlockPages, 0, blockPagesSize * sizeof(BlockPage*));

		ContentPagesCount = 0;
		BranchPagesCount = 0;
		BlockPagesCount = 0;

		ContentPagesSize = contentPagesSize;
		BranchPagesSize = branchPagesSize;
		BlockPagesSize = blockPagesSize;
		
		lastContentOffset = 1;
		lastBranchOffset = 0;
		lastBlockOffset = 0;
	}

	/*void getPath(char* buff, char* fileName)
	{
		getPath(buff, Path, fileName, TableName);
	}

	static void getPath(char* buff,
						char* path, 
						char* fileName, 
						char* tableName)
	{
		if(tableName[0])
		{
			sprintf(buff, "%s\\%s_%s.pg", path, fileName, tableName);
		}
		else
		{
			sprintf(buff, "%s\\%s.pg", path, fileName);
		}
	}*/

	ulong64 getHeaderSize()
	{
		return HeaderSize * sizeof(HeaderCell);
	}

	ulong64 getContentSize()
	{
		return ContentPagesCount * sizeof(ContentPage);
	}

	ulong64 getBranchSize()
	{
		return BranchPagesCount * sizeof(BranchPage);
	}

	ulong64 getBlockSize()
	{
		return BlockPagesCount * sizeof(BlockPage);
	}
	
	ulong64 getUsedMemory()
	{
		return	getHeaderSize() + 
				getContentSize() + 
				getBranchSize() +
				getBlockSize();
	}

	ulong64 getTotalMemory()
	{
		return	getHeaderSize() + 
				getContentSize() + 
				getBranchSize() +
				getBlockSize();
	}

	void clear()
	{
		uint32 keyLen = KeyLen * 4;
		uint32 valueLen = ValueLen * 4;
		uint32 headerBase = HeaderBase;
		
		char path[1024];
		strcpy(path, Path);

		char tableName[1024];
		strcpy(tableName, TableName);

		destroy();

		init(path, tableName, keyLen, valueLen, headerBase);
	}

	/*void saveInfo()
	{
		char infoPath[1024];
		
		getPath(infoPath, "ha_info");

		BinaryFile* pInfoFile = new BinaryFile(infoPath, true, true);
		if(pInfoFile)
		{
			if(pInfoFile->open())
			{
				HArrayFixBaseInfo info;

				info.Version = 1;
				info.KeyLen = KeyLen * 4;
				info.ValueLen = ValueLen * 4;
				info.HeaderBase = HeaderBase;
				info.CountPartitions = 1;
				info.Partitions[0].ContentPagesCount = ContentPagesCount;
				info.Partitions[0].BranchPagesCount = BranchPagesCount;
				info.Partitions[0].BlockPagesCount = BlockPagesCount;
				info.Partitions[0].LastContentOffset = lastContentOffset;
				info.Partitions[0].LastBranchOffset = lastBranchOffset;
				info.Partitions[0].LastBlockOffset = lastBlockOffset;

				pInfoFile->write(&info, 0, sizeof(HArrayFixBaseInfo));
				pInfoFile->close();

				delete pInfoFile;
			}
		}
	}*/

	void load()
	{
		destroy();

		HArrayTextFile file;

		file.init(Path, TableName, KeyLen);
				
		HArrayFixPair* pairs = HArrayFixPair::CreateArray(1000000, 3);

		file.open();

		ulong64 blockNumber = 0;
		uint32 wordInBlock = 0;

		while (true)
		{
			//get portion
			uint32 count = file.getKeysAndValuesByPortions(pairs, 1000000, blockNumber, wordInBlock);

			if (!count)
				break;

			//insert portion of keys
			for (uint32 i = 0; i < count; i++)
			{
				this->insert(pairs[i].Key, pairs[i].Value);
			}
		}
		
		file.close();

		HArrayFixPair::DeleteArray(pairs);
	}

	void save()
	{
		HArrayTextFile file;

		file.init(Path, TableName, KeyLen);

		file.create();

		InsertToTextFileVisitor visitor(&file);
		
		this->scanByVisitor(&visitor);

		file.close();
	}

	/*
	void load()
	{
		char path[1024];
		strcpy(path, Path);

		destroy();

		char infoPath[1024];
		char headerPath[1024];
		char contentPath[1024];
		char branchPath[1024];
		char blockPath[1024];

		getPath(infoPath, "ha_dic");
		getPath(headerPath, "ha_header");
		getPath(contentPath, "ha_content_part1");
		getPath(branchPath, "ha_branch_part1");
		getPath(blockPath, "ha_block_part1");

		HArrayFixBaseInfo info;

		//header
		BinaryFile* pInfoFile = new BinaryFile(infoPath, false, false);
		if(pInfoFile->open())
		{
			pInfoFile->read(&info, 0, sizeof(HArrayFixBaseInfo));

			init(Path,
				 TableName,
				 info.KeyLen,
				 info.ValueLen,
				 info.HeaderBase,
				 info.Partitions[0].ContentPagesCount + 1,
				 info.Partitions[0].BranchPagesCount + 1,
				 info.Partitions[0].BlockPagesCount + 1);

			ContentPagesCount = info.Partitions[0].ContentPagesCount;
			BranchPagesCount = info.Partitions[0].BranchPagesCount;
			BlockPagesCount = info.Partitions[0].BlockPagesCount;

			lastContentOffset = info.Partitions[0].LastContentOffset;
			lastBranchOffset = info.Partitions[0].LastBranchOffset;
			lastBlockOffset = info.Partitions[0].LastBlockOffset;
						
			pInfoFile->close();
			delete pInfoFile;
		}

		//header
		BinaryFile* pHeaderFile = new BinaryFile(headerPath, false, false);
		if(pHeaderFile->open())
		{
			pHeaderFile->read(pHeader, HeaderSize*sizeof(HeaderCell));
			pHeaderFile->close();
			delete pHeaderFile;
		}
		
		//content
		BinaryFile* pContentPagesFile = new BinaryFile(contentPath, false, false);
		if(pContentPagesFile->open())
		{
			for(uint32 i=0; i<ContentPagesCount; i++)
			{
				pContentPages[i] = new ContentPage();
				pContentPagesFile->read(pContentPages[i], sizeof(ContentPage));
			}

			pContentPagesFile->close();
			delete pContentPagesFile;
		}
		
		//barnches
		BinaryFile* pBranchPagesFile = new BinaryFile(branchPath, false, false);
		if(pBranchPagesFile->open())
		{
			for(uint32 i=0; i<BranchPagesCount; i++)
			{
				pBranchPages[i] = new BranchPage();
				pBranchPagesFile->read(pBranchPages[i], sizeof(BranchPage));
			}

			pBranchPagesFile->close();
			delete pBranchPagesFile;
		}

		//blocks
		BinaryFile* pBlockPagesFile = new BinaryFile(blockPath, false, false);
		if(pBlockPagesFile->open())
		{
			for(uint32 i=0; i<BlockPagesCount; i++)
			{
				pBlockPages[i] = new BlockPage();
				pBlockPagesFile->read(pBlockPages[i], sizeof(BlockPage));
			}

			pBlockPagesFile->close();
			delete pBlockPagesFile;
		}
	}
	
	void save()
	{
		saveInfo();
		
		char headerPath[1024];
		char contentPath[1024];
		char branchPath[1024];
		char blockPath[1024];

		getPath(headerPath, "ha_header");
		getPath(contentPath, "ha_content_part1");
		getPath(branchPath, "ha_branch_part1");
		getPath(blockPath, "ha_block_part1");

		//header
		BinaryFile* pHeaderFile = new BinaryFile(headerPath, true, true);
		if(pHeaderFile->open())
		{
			pHeaderFile->write(pHeader, HeaderSize*sizeof(HeaderCell));
			pHeaderFile->close();
			delete pHeaderFile;
		}
		
		//content
		BinaryFile* pContentPagesFile = new BinaryFile(contentPath, true, true);
		if(pContentPagesFile->open())
		{
			for(uint32 i=0; i<ContentPagesCount; i++)
			{
				pContentPagesFile->write(pContentPages[i], sizeof(ContentPage));
			}

			pContentPagesFile->close();
			delete pContentPagesFile;
		}
		
		//barnches
		BinaryFile* pBranchPagesFile = new BinaryFile(branchPath, true, true);
		if(pBranchPagesFile->open())
		{
			for(uint32 i=0; i<BranchPagesCount; i++)
			{
				pBranchPagesFile->write(pBranchPages[i], sizeof(BranchPage));
			}

			pBranchPagesFile->close();
			delete pBranchPagesFile;
		}

		//blocks
		BinaryFile* pBlockPagesFile = new BinaryFile(blockPath, true, true);
		if(pBlockPagesFile->open())
		{
			for(uint32 i=0; i<BlockPagesCount; i++)
			{
				pBlockPagesFile->write(pBlockPages[i], sizeof(BlockPage));
			}

			pBlockPagesFile->close();
			delete pBlockPagesFile;
		}
	}
	*/

	//types: 0-empty, 1..4 branches, 5 value, 6..9 blocks offset, 10 empty branch, 11 value
#ifndef _RELEASE

	uint32 tempValues[COUNT_TEMPS];
	char* tempCaptions[COUNT_TEMPS];

#endif

	void reallocateContentPages()
	{
		uint32 newSizeContentPages = ContentPagesSize * 2;
		ContentPage** pTempContentPages = new ContentPage*[newSizeContentPages];
			
		uint32 j=0;
		for(; j < ContentPagesSize ; j++)
		{
			pTempContentPages[j] = pContentPages[j];
		}

		for(; j < newSizeContentPages ; j++)
		{
			pTempContentPages[j] = 0;
		}

		delete[] pContentPages;
		pContentPages = pTempContentPages;

		ContentPagesSize = newSizeContentPages;
	}

	void reallocateBranchPages()
	{
		uint32 newSizeBranchPages = BranchPagesSize * 2;
		BranchPage** pTempBranchPages = new BranchPage*[newSizeBranchPages];
			
		uint32 j=0;
		for(; j < BranchPagesSize ; j++)
		{
			pTempBranchPages[j] = pBranchPages[j];
		}

		for(; j < newSizeBranchPages ; j++)
		{
			pTempBranchPages[j] = 0;
		}

		delete[] pBranchPages;
		pBranchPages = pTempBranchPages;

		BranchPagesSize = newSizeBranchPages;
	}

	void reallocateBlockPages()
	{
		uint32 newSizeBlockPages = BlockPagesSize * 2;
		BlockPage** pTempBlockPages = new BlockPage*[newSizeBlockPages];
			
		uint32 j=0;
		for(; j < BlockPagesSize ; j++)
		{
			pTempBlockPages[j] = pBlockPages[j];
		}

		for(; j < newSizeBlockPages ; j++)
		{
			pTempBlockPages[j] = 0;
		}

		delete[] pBlockPages;
		pBlockPages = pTempBlockPages;

		BlockPagesSize = newSizeBlockPages;
	}
	
	//INSERT =============================================================================================================

	bool insert(uint32* key, ulong64 value);
	
	//void insertVar(uint32* key, uint32 keyLen, uint32* value, uint32 valLen);

	//GET =============================================================================================================

	ulong64 getValueByKey(uint32* key);

	//uint32* getValueByVarKey(uint32* key, uint32 keyLen);
	
	//RANGE =============================================================================================================
	void getValuesByRangeFromBlock(ulong64* values, 
									uint32& count,
									uint32 size,
									uint32 contentOffset,
									uint32 keyOffset,
									uint32 blockOffset,
									uint32* minKey,
									uint32* maxKey);
	
	void getValuesByRange(ulong64* values, 
								uint32& count,
								uint32 size,
								uint32 keyOffset, 
								uint32 contentOffset,
								uint32* minKey,
								uint32* maxKey);
	
	uint32 getValuesByRange(ulong64* values, 
						 uint32 size, 
						 uint32* minKey, 
						 uint32* maxKey);
	
	//RANGE keys and values =============================================================================================================
	void sortItems(HArrayFixPair* pairs,
				   int startIndex,
				   int endIndex);
	
	void getKeysAndValuesByRangeFromBlock(HArrayFixPair* pairs,
										  uint32& count,
										  uint32 size,
										  uint32 contentOffset,
										  uint32 keyOffset,
										  uint32 blockOffset,
										  uint32* minKey,
										  uint32* maxKey);
	
	void getKeysAndValuesByRange(HArrayFixPair* pairs,
								 uint32& count,
								 uint32 size,
								 uint32 keyOffset, 
								 uint32 contentOffset,
								 uint32* minKey,
								 uint32* maxKey);
	
	uint32 getKeysAndValuesByRange(HArrayFixPair* pairs,
								 uint32 size, 
								 uint32* minKey, 
								 uint32* maxKey);
	
	//scan by selector
	bool getKeysAndValuesByRangeFromBlock(HArrayVisitor* pVisitor,
										  uint32* key,
										  uint32 contentOffset,
										  uint32 keyOffset,
										  uint32 blockOffset,
										  uint32* startKey,
										  uint32* endKey);

	bool getKeysAndValuesByRange(HArrayVisitor* pVisitor,
								uint32* key,
								uint32 keyOffset, 
								uint32 contentOffset,
								uint32* startKey,
								uint32* endKey);

	void scanByVisitor(HArrayVisitor* pVisitor);

	//TEMPLATE ====================================================================================================

	void getValuesByTemplateFromBlock(ulong64* values,
									uint32& count,
									uint32 size,
									uint32 contentOffset,
									uint32 keyOffset,
									uint32 blockOffset,
									uint32* minKey,
									uint32* maxKey);
	
	void getValuesByTemplate(ulong64* values, 
							uint32& count,
							uint32 size,
							uint32 keyOffset, 
							uint32 contentOffset,
							uint32* minKey,
							uint32* maxKey);
	
	uint32 getValuesByTemplate(ulong64* values, 
							uint32 size, 
							uint32* minKey, 
							uint32* maxKey);

	//=============================================================================================================

	void destroy()
	{
		if(pHeader)
		{
			delete[] pHeader;
			pHeader = 0;
		}

		if(pContentPages)
		{
			for(uint32 i=0; i<ContentPagesCount; i++)
			{
				delete pContentPages[i];
			}

			delete[] pContentPages;
			pContentPages = 0;
		}

		if(pBranchPages)
		{
			for(uint32 i=0; i<BranchPagesCount; i++)
			{
				delete pBranchPages[i];
			}
			
			delete[] pBranchPages;
			pBranchPages = 0;
		}

		if(pBlockPages)
		{
			for(uint32 i=0; i<BlockPagesCount; i++)
			{
				delete pBlockPages[i];
			}

			delete[] pBlockPages;
			pBlockPages = 0;
		}
	}
};