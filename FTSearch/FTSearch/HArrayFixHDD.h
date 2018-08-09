#include "HArrayFixBase.h"

struct HArrayPartition
{
public:
	BinaryFile* pContentPagesFile;
	BinaryFile* pBranchPagesFile;
	BinaryFile* pBlockPagesFile;

	uint32 ContentPagesSize;
	uint32 BranchPagesSize;
	uint32 BlockPagesSize;

	uint32 freeBranchCells[MAX_SHORT];
	uint32 countFreeBranchCell;

	uint32 lastContentOffset;
	uint32 lastBranchOffset;
	uint32 lastBlockOffset;
};

class HArrayFixHDD
{
public:
	HArrayFixHDD()
	{
		memset(this, 0, sizeof(HArrayFixHDD));
	}

	HArrayPartition* partitions;
	uint32 countPartitions;

	HArrayPartition* pActivePartition;
	
	BinaryFile* pHeaderFile;
		
	char TableName[1024];
	char Path[1024];

	uint32 HeaderBase;
	uint32 HeaderBits;
	uint32 HeaderSize;

	uint32 KeyLen;
	uint32 ValueLen;
	uint32 RowLen;

	uint32 MAX_SAFE_SHORT;

	void init(const char* path,
			  const char* tableName,
			  uint32 keyLen,
			  uint32 valueLen, 
			  uchar8 headerBase)
	{
		partitions = new HArrayPartition[MAX_HARRAY_PARTITIONS];
		memset(partitions, 0, MAX_HARRAY_PARTITIONS * sizeof(HArrayPartition));

		for (uint32 i = 0; i < MAX_HARRAY_PARTITIONS; i++)
		{
			partitions[i].lastContentOffset = 1;
		}

		pActivePartition = &partitions[0];
		countPartitions = 1;

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
		HeaderSize = (0xFFFFFFFF >> HeaderBits) + 1;

		pActivePartition->countFreeBranchCell = 0;

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

		pActivePartition->ContentPagesSize = 0;
		pActivePartition->BranchPagesSize = 0;
		pActivePartition->BlockPagesSize = 0;
		
		pActivePartition->lastContentOffset = 1;
		pActivePartition->lastBranchOffset = 0;
		pActivePartition->lastBlockOffset = 0;
	}

	void getPath(char* buff, char* fileName, uint32 partition)
	{
		getPath(buff, Path, fileName, TableName, partition);
	}

	void getPath(char* buff, char* fileName, char* tableName, uint32 partition)
	{
		getPath(buff, Path, fileName, tableName, partition);
	}

	static void getPath(char* buff,
						char* path, 
						char* fileName, 
						char* tableName,
						uint32 partition)
	{
		if(tableName[0])
		{
			if (partition)
			{
				sprintf(buff, "%s\\%s_%s_part%u.pg", path, fileName, tableName, partition);
			}
			else
			{
				sprintf(buff, "%s\\%s_%s.pg", path, fileName, tableName);
			}
		}
		else
		{
			if (partition)
			{
				sprintf(buff, "%s\\%s_part%u.pg", path, fileName, partition);
			}
			else
			{
				sprintf(buff, "%s\\%s.pg", path, fileName);
			}
		}
	}

	//header
	inline void readHeaderCellHDD(HeaderCell& headerCell, uint32 position)
	{
		pHeaderFile->read(&headerCell, position * sizeof(HeaderCell), sizeof(HeaderCell));
	}

	inline void readHeaderCellsHDD(HeaderCell* headerCells, uint32 position, uint32 count)
	{
		pHeaderFile->read(headerCells, position * sizeof(HeaderCell), count * sizeof(HeaderCell));
	}

	inline void writeHeaderCellHDD(HeaderCell& headerCell, uint32 position)
	{
		pHeaderFile->write(&headerCell, position * sizeof(HeaderCell), sizeof(HeaderCell));
	}

	//content
	inline void readContentCellHDD(ContentCell& valueCell, uint32 position)
	{
		pActivePartition->pContentPagesFile->readBuffered(&valueCell, position * sizeof(ContentCell), sizeof(ContentCell));
	}

	inline void readContentCellsHDD(ContentCell* valueCells, uint32 position, uint32 count)
	{
		pActivePartition->pContentPagesFile->readBuffered(valueCells, position * sizeof(ContentCell), count * sizeof(ContentCell));
	}

	inline void writeContentCellHDD(ContentCell& valueCell, uint32 position)
	{
		pActivePartition->pContentPagesFile->writeBuffered(&valueCell, position * sizeof(ContentCell), sizeof(ContentCell));
	}

	inline void writeContentCellsHDD(ContentCell* valueCells, uint32 position, uint32 count)
	{
		pActivePartition->pContentPagesFile->writeBuffered(valueCells, position * sizeof(ContentCell), count * sizeof(ContentCell));
	}

	//branch
	inline void readBranchCellHDD(BranchCell& valueCell, uint32 position)
	{
		pActivePartition->pBranchPagesFile->readBuffered(&valueCell, position * sizeof(BranchCell), sizeof(BranchCell));
	}

	inline void writeBranchCellHDD(BranchCell& valueCell, uint32 position)
	{
		pActivePartition->pBranchPagesFile->writeBuffered(&valueCell, position * sizeof(BranchCell), sizeof(BranchCell));
	}

	//block
	inline void readBlockCellHDD(BlockCell& valueCell, uint32 position)
	{
		pActivePartition->pBlockPagesFile->readBuffered(&valueCell, position * sizeof(BlockCell), sizeof(BlockCell));
	}

	inline void readBlockCellsHDD(BlockCell* valueCells, uint32 position, uint32 count)
	{
		pActivePartition->pBlockPagesFile->readBuffered(valueCells, position * sizeof(BlockCell), count * sizeof(BlockCell));
	}

	inline void writeBlockCellHDD(BlockCell& valueCell, uint32 position)
	{
		pActivePartition->pBlockPagesFile->writeBuffered(&valueCell, position * sizeof(BlockCell), sizeof(BlockCell));
	}

	ulong64 getHeaderSize()
	{
		return HeaderSize * sizeof(HeaderCell);
	}

	ulong64 getContentSize()
	{
		return 0; //ContentPagesCount * sizeof(ContentPage);
	}

	ulong64 getBranchSize()
	{
		return 0; //BranchPagesCount * sizeof(BranchPage);
	}

	ulong64 getBlockSize()
	{
		return 0; //BlockPagesCount * sizeof(BlockPage);
	}
	
	ulong64 getSize()
	{
		return	getHeaderSize() + 
				getContentSize() + 
				getBranchSize() +
				getBlockSize();
	}
	
	void openOrCreate()
	{
		openOrCreate(0);
	}

	void openOrCreate(uint32 bufferSize)
	{
		char infoPath[1024];
		
		getPath(infoPath, "ha_info", 0);
		
		if(BinaryFile::existsFile(infoPath))
		{
			open(bufferSize);
		}
		else
		{
			create(bufferSize, 1);
		}
	}

	void create()
	{
		create(0, 1);
	}

	void create(uint32 bufferSize, uint32 reservePartitions)
	{
		saveInfo();
		
		char headerPath[1024];
		char contentPath[1024];
		char branchPath[1024];
		char blockPath[1024];

		getPath(headerPath, "ha_header", 0);
		
		uint32 fileBufferSize = (bufferSize - (HeaderSize * sizeof(HeaderCell)));// / 3;

		//header
		pHeaderFile = new BinaryFile(headerPath, true, true, HeaderSize);
		if(pHeaderFile->open())
		{
			pHeaderFile->allocate(HeaderSize * sizeof(HeaderCell), 0);
		}

		for (uint32 i = 0; i < reservePartitions; i++)
		{
			getPath(contentPath, "ha_content", i + 1);
			getPath(branchPath, "ha_branch", i + 1);
			getPath(blockPath, "ha_block", i + 1);

			partitions[i].pContentPagesFile = new BinaryFile(contentPath, true, true, fileBufferSize);
			partitions[i].pContentPagesFile->open();

			partitions[i].pBranchPagesFile = new BinaryFile(branchPath, true, true, fileBufferSize);
			partitions[i].pBranchPagesFile->open();

			partitions[i].pBlockPagesFile = new BinaryFile(blockPath, true, true, fileBufferSize);
			partitions[i].pBlockPagesFile->open();
		}

		countPartitions = reservePartitions;
	}

	void open()
	{
		open(0);
	}

	void open(uint32 bufferSize)
	{
		char infoPath[1024];
		char headerPath[1024];
		char contentPath[1024];
		char branchPath[1024];
		char blockPath[1024];

		getPath(infoPath, "ha_info", 0);
		getPath(headerPath, "ha_header", 0);
		
		HArrayFixBaseInfo info;

		uint32 fileBufferSize;
		
		if(bufferSize)
		{
			fileBufferSize = (bufferSize - (HeaderSize * sizeof(HeaderCell)));// / 3;
		}
		else
		{
			fileBufferSize = 0;
		}

		//header
		BinaryFile* pInfoFile = new BinaryFile(infoPath, false, false);
		if(pInfoFile->open())
		{
			pInfoFile->read(&info, 0, sizeof(HArrayFixBaseInfo));

			init(Path,
				 TableName,
				 info.KeyLen,
				 info.ValueLen,
				 info.HeaderBase);

			countPartitions = info.CountPartitions;

			for (uint32 i = 0; i < info.CountPartitions; i++)
			{
				partitions[i].ContentPagesSize = info.Partitions[i].ContentPagesCount;
				partitions[i].BranchPagesSize = info.Partitions[i].BranchPagesCount;
				partitions[i].BlockPagesSize = info.Partitions[i].BlockPagesCount;

				partitions[i].lastContentOffset = info.Partitions[i].LastContentOffset;
				partitions[i].lastBranchOffset = info.Partitions[i].LastBranchOffset;
				partitions[i].lastBlockOffset = info.Partitions[i].LastBlockOffset;
			}

			pInfoFile->close();
			delete pInfoFile;
		}

		//header
		pHeaderFile = new BinaryFile(headerPath, true, false, HeaderSize);
		pHeaderFile->open();

		for (uint32 i = 0; i < info.CountPartitions; i++)
		{
			getPath(contentPath, "ha_content", i+1);
			getPath(branchPath, "ha_branch", i+1);
			getPath(blockPath, "ha_block", i+1);

			//content
			partitions[i].pContentPagesFile = new BinaryFile(contentPath, true, false, fileBufferSize);
			partitions[i].pContentPagesFile->open();

			//barnches
			partitions[i].pBranchPagesFile = new BinaryFile(branchPath, true, false, fileBufferSize);
			partitions[i].pBranchPagesFile->open();

			//blocks
			partitions[i].pBlockPagesFile = new BinaryFile(blockPath, true, false, fileBufferSize);
			partitions[i].pBlockPagesFile->open();
		}
	}

	void flush()
	{
		//flush files
		if(pHeaderFile)
		{
			pHeaderFile->flush();
		}

		for (uint32 i = 0; i < countPartitions; i++)
		{
			if (partitions[i].pContentPagesFile)
			{
				partitions[i].pContentPagesFile->flush();
			}

			if (partitions[i].pBranchPagesFile)
			{
				partitions[i].pBranchPagesFile->flush();
			}

			if (partitions[i].pBlockPagesFile)
			{
				partitions[i].pBlockPagesFile->flush();
			}
		}

		saveInfo();
	}

	void saveInfo()
	{
		char infoPath[1024];
		
		getPath(infoPath, "ha_info", 0);

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
				info.CountPartitions = countPartitions;

				for (uint32 i = 0; i < countPartitions; i++)
				{
					info.Partitions[i].ContentPagesCount = (partitions[i].lastContentOffset | MAX_SHORT) / MAX_SHORT; //alingment on MAX_SHORT
					info.Partitions[i].BranchPagesCount = (partitions[i].lastBranchOffset | MAX_SHORT) / MAX_SHORT;
					info.Partitions[i].BlockPagesCount = (partitions[i].lastBlockOffset | MAX_SHORT) / MAX_SHORT;
					info.Partitions[i].LastContentOffset = partitions[i].lastContentOffset;
					info.Partitions[i].LastBranchOffset = partitions[i].lastBranchOffset;
					info.Partitions[i].LastBlockOffset = partitions[i].lastBlockOffset;
				}

				pInfoFile->write(&info, 0, sizeof(HArrayFixBaseInfo));
				pInfoFile->close();

				delete pInfoFile;
			}
		}
	}
	
	void clear()
	{
		for (uint32 i = 0; i < countPartitions; i++)
		{
			//members
			partitions[i].ContentPagesSize = 0;
			partitions[i].BranchPagesSize = 0;
			partitions[i].BlockPagesSize = 0;

			partitions[i].lastContentOffset = 1;
			partitions[i].lastBranchOffset = 0;
			partitions[i].lastBlockOffset = 0;
		}

		//recreate
		close();

		create();
	}

	static void deleteFiles(char* path, char* tableName)
	{
		//clear files
		char infoPath[1024];
		char headerPath[1024];

		getPath(infoPath, path, "ha_info", tableName, 0);
		getPath(headerPath, path, "ha_header", tableName, 0);

		BinaryFile::deleteFile(infoPath);
		BinaryFile::deleteFile(headerPath);

		for (uint32 i = 0; i < MAX_HARRAY_PARTITIONS; i++)
		{
			char contentPath[1024];
			char branchPath[1024];
			char blockPath[1024];

			getPath(contentPath, path, "ha_content", tableName, i+1);
			getPath(branchPath, path, "ha_branch", tableName, i+1);
			getPath(blockPath, path, "ha_block", tableName, i+1);

			BinaryFile::deleteFile(contentPath);
			BinaryFile::deleteFile(branchPath);
			BinaryFile::deleteFile(blockPath);
		}
	}

	static void renameFiles(char* path, char* oldTableName, char* newTableName)
	{
		//clear files
		char oldInfoPath[1024];
		char oldHeaderPath[1024];
		
		getPath(oldInfoPath, path, "ha_info", oldTableName, 0);
		getPath(oldHeaderPath, path, "ha_header", oldTableName, 0);

		char newInfoPath[1024];
		char newHeaderPath[1024];
		
		getPath(newInfoPath, path, "ha_info", newTableName, 0);
		getPath(newHeaderPath, path, "ha_header", newTableName, 0);
				
		BinaryFile::renameFile(oldInfoPath, newInfoPath);
		BinaryFile::renameFile(oldHeaderPath, newHeaderPath);
		
		for (uint32 i = 0; i < MAX_HARRAY_PARTITIONS; i++)
		{
			char oldContentPath[1024];
			char oldBranchPath[1024];
			char oldBlockPath[1024];

			getPath(oldContentPath, path, "ha_content", oldTableName, i+1);
			getPath(oldBranchPath, path, "ha_branch", oldTableName, i+1);
			getPath(oldBlockPath, path, "ha_block", oldTableName, i+1);

			char newContentPath[1024];
			char newBranchPath[1024];
			char newBlockPath[1024];

			getPath(newContentPath, path, "ha_content", newTableName, i+1);
			getPath(newBranchPath, path, "ha_branch", newTableName, i+1);
			getPath(newBlockPath, path, "ha_block", newTableName, i+1);

			BinaryFile::renameFile(oldContentPath, newContentPath);
			BinaryFile::renameFile(oldBranchPath, newBranchPath);
			BinaryFile::renameFile(oldBlockPath, newBlockPath);
		}
	}

	void close()
	{
		flush();

		if(pHeaderFile)
		{
			pHeaderFile->close();

			delete pHeaderFile;

			pHeaderFile = 0;
		}

		for (uint32 i = 0; i < countPartitions; i++)
		{
			if (partitions[i].pContentPagesFile)
			{
				partitions[i].pContentPagesFile->close();

				delete partitions[i].pContentPagesFile;

				partitions[i].pContentPagesFile = 0;
			}

			if (partitions[i].pBranchPagesFile)
			{
				partitions[i].pBranchPagesFile->close();

				delete partitions[i].pBranchPagesFile;

				partitions[i].pBranchPagesFile = 0;
			}

			if (partitions[i].pBlockPagesFile)
			{
				partitions[i].pBlockPagesFile->close();

				delete partitions[i].pBlockPagesFile;

				partitions[i].pBlockPagesFile = 0;
			}
		}
	}

	static uint32 getRecoveryIndexState(char* path, char* tempTableName)
	{
		char infoPathTemp[1024];
		char headerPathTemp[1024];
		char contentPathTemp[1024];
		char branchPathTemp[1024];
		char blockPathTemp[1024];

		getPath(infoPathTemp, path, "ha_info", tempTableName, 0);
		getPath(headerPathTemp, path, "ha_header", tempTableName, 0);
		getPath(contentPathTemp, path, "ha_content", tempTableName, 0);
		getPath(branchPathTemp, path, "ha_branch", tempTableName, 0);
		getPath(blockPathTemp, path, "ha_block", tempTableName, 0);

		bool isInfoPathTempExists = BinaryFile::existsFile(infoPathTemp);
		bool isHeaderPathTempExists = BinaryFile::existsFile(headerPathTemp);
		bool isContentPathTempExists = BinaryFile::existsFile(contentPathTemp);
		bool isBranchPathTempExists = BinaryFile::existsFile(branchPathTemp);
		bool isBlockPathTempExists = BinaryFile::existsFile(blockPathTemp);

		if(!isInfoPathTempExists && 
		   !isHeaderPathTempExists &&
		   !isContentPathTempExists &&
		   !isBranchPathTempExists &&
		   !isBlockPathTempExists)
		{
			//1. if there are no temp files, than Ok
			return 0;
		}
		else if(isInfoPathTempExists && 
				isHeaderPathTempExists &&
				isContentPathTempExists &&
				isBranchPathTempExists &&
				isBlockPathTempExists)
		{
			//2. if there are all temp files, they are corrupted, just delete them
			return 2;
		}
		else
		{
			//3. if there are some temp files, they are ok, rename rest of them
			return 1;
		}
	}

	//0 - Normal
	//1 - Commit
	//2 - Rollback

	static void recoveryIndex(char* path,
							  char* tableName,
							  char* tempTableName,
							  uint32 recoveryIndexState)
	{
		if(recoveryIndexState == 0)
		{
			//1. if there are no temp files, than Ok
			return;
		}

		char infoPathTemp[1024];
		char headerPathTemp[1024];
		char contentPathTemp[1024];
		char branchPathTemp[1024];
		char blockPathTemp[1024];

		getPath(infoPathTemp, path, "ha_info", tempTableName, 0);
		getPath(headerPathTemp, path, "ha_header", tempTableName, 0);
		getPath(contentPathTemp, path, "ha_content", tempTableName, 0);
		getPath(branchPathTemp, path, "ha_branch", tempTableName, 0);
		getPath(blockPathTemp, path, "ha_block", tempTableName, 0);

		if(recoveryIndexState == 2)
		{
			//2. if there are all temp files, they are corrupted, just delete them
			BinaryFile::deleteFile(infoPathTemp);
			BinaryFile::deleteFile(headerPathTemp);
			BinaryFile::deleteFile(contentPathTemp);
			BinaryFile::deleteFile(branchPathTemp);
			BinaryFile::deleteFile(blockPathTemp);
		}
		else if(recoveryIndexState == 1)
		{
			char infoPath[1024];
			char headerPath[1024];
			char contentPath[1024];
			char branchPath[1024];
			char blockPath[1024];

			getPath(infoPath, path, "ha_info", tableName, 0);
			getPath(headerPath, path, "ha_header", tableName, 0);
			getPath(contentPath, path, "ha_content", tableName, 0);
			getPath(branchPath, path, "ha_branch", tableName, 0);
			getPath(blockPath, path, "ha_block", tableName, 0);

			bool isInfoPathTempExists = BinaryFile::existsFile(infoPathTemp);
			bool isHeaderPathTempExists = BinaryFile::existsFile(headerPathTemp);
			bool isContentPathTempExists = BinaryFile::existsFile(contentPathTemp);
			bool isBranchPathTempExists = BinaryFile::existsFile(branchPathTemp);
			bool isBlockPathTempExists = BinaryFile::existsFile(blockPathTemp);

			//3. if there are some temp files, they are ok, rename rest of them
			if(isInfoPathTempExists)
			{
				BinaryFile::renameFile(infoPathTemp, infoPath);
			}

			if(isHeaderPathTempExists)
			{
				BinaryFile::renameFile(headerPathTemp, headerPath);
			}

			if(isContentPathTempExists)
			{
				BinaryFile::renameFile(contentPathTemp, contentPath);
			}

			if(isBranchPathTempExists)
			{
				BinaryFile::renameFile(branchPathTemp, branchPath);
			}

			if(isBlockPathTempExists)
			{
				BinaryFile::renameFile(blockPathTemp, blockPath);
			}
		}
	}

	//types: 0-empty, 1..4 branches, 5 value, 6..9 blocks offset, 10 empty branch, 11 value
#ifndef _RELEASE

	uint32 tempValues[COUNT_TEMPS];
	char* tempCaptions[COUNT_TEMPS];

#endif

	void insert(uint32* key, ulong64 value);
	
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
	
	uint32 getKeysAndValuesByPortions(HArrayFixPair* pairs,
								  uint32 size, 
								  uint32& startHeader,
								  bool& isBufferNotEnough);

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
	void checkCapacity(char* error)
	{
		error[0] = 0;

		for (uint32 i = 0; i < countPartitions; i++)
		{
			if (partitions[i].lastContentOffset >= MAX_HARRAY_PARTITION_CAPACITY ||
				partitions[i].lastBranchOffset >= MAX_HARRAY_PARTITION_CAPACITY ||
				partitions[i].lastBlockOffset >= MAX_HARRAY_PARTITION_CAPACITY)
			{
				splitPartition(i, error);

				if (strlen(error))
				{
					return;
				}
			}
		}
	}

	void splitPartition(uint32 partition, char* error)
	{
		if (countPartitions + 1 >= MAX_HARRAY_PARTITIONS)
		{
			sprintf(error, "%s", "Max partitions for dictionary exceeded.");
			return;
		}

		//create new HArray
		HArrayFixHDD haWordsHDDTemp;

		haWordsHDDTemp.init(Path,
							"tempPartition",
							KeyLen * 4,
							ValueLen * 4,
							HeaderBase);

		uint32 newPartition = countPartitions;

		haWordsHDDTemp.create(BIN_FILE_BUFFER_SIZE, countPartitions + 1);

		uint32 switcher = 0;

		HeaderCell* headerCells = new HeaderCell[HeaderSize];
		readHeaderCellsHDD(headerCells, 0, HeaderSize);
		
		//mark in header new partitions
		for (uint32 i = 0; i < HeaderSize; i++)
		{
			if (*(uint32*)&headerCells[i])
			{
				if (headerCells[i].Partition == partition)
				{
					if (switcher & 0x1) //split partition on two partitions (partition + newPartition)
					{
						headerCells[i].Partition = newPartition;
					}
					
					switcher++;
				}

				*(uint32*)&headerCells[i] = 0;

				haWordsHDDTemp.writeHeaderCellHDD(headerCells[i], i);
			}
		}

		delete[] headerCells;

		if (switcher < 2)
		{
			sprintf(error, "%s", "Partition cannot be splited.");
			return;
		}

		//insert keys
		HArrayFixPair* pKeysAndValuesHDD = HArrayFixPair::CreateArray(HARRAY_BUFFER_SIZE, KeyLen);
		uint32 startHeaderHDD = 0;

		while (true)
		{
			bool isBufferNotEnough = false;

			uint32 countHDD = getKeysAndValuesByPortions(pKeysAndValuesHDD,
														HARRAY_BUFFER_SIZE,
														startHeaderHDD,
														isBufferNotEnough);

			if (isBufferNotEnough)
			{
				sprintf(error, "The buffer MAX_SIZE_BUFFER is not enough to get portion from HDD.");

				break;
			}

			if (countHDD)
			{
				for (uint32 i = 0; i < countHDD; i++)
				{
					haWordsHDDTemp.insert(pKeysAndValuesHDD[i].Key,
										  pKeysAndValuesHDD[i].Value);
				}
			}
			else
			{
				break;
			}
		}

		HArrayFixPair::DeleteArray(pKeysAndValuesHDD);

		//close harrays
		close();

		haWordsHDDTemp.flush();

		haWordsHDDTemp.close();
		
		//delete old files
		HArrayFixHDD::deleteFiles(Path, TableName);

		//rename tables
		HArrayFixHDD::renameFiles(Path, "tempPartition", TableName);

		//open harray
		open(BIN_FILE_BUFFER_SIZE);
	
		////test
		//startHeaderHDD = 0;
		//bool isBufferNotEnough = false;

		//HArrayFixPair* pKeysAndValuesHDD2 = HArrayFixPair::CreateArray(HARRAY_BUFFER_SIZE, KeyLen);

		//uint32 countHDD = getKeysAndValuesByPortions(pKeysAndValuesHDD2,
		//	10000000,
		//	startHeaderHDD,
		//	isBufferNotEnough);

		//for (int i = 0; i < countHDD; i++)
		//{
		//	if (pKeysAndValuesHDD[i].Key[0] != pKeysAndValuesHDD2[i].Key[0])
		//	{
		//		printf("error");
		//	}
		//}

		//countHDD = countHDD;
	}

	void destroy()
	{
		close();

		if (partitions)
		{
			delete[] partitions;
		}

		memset(this, 0, sizeof(HArrayFixHDD));
	}
};