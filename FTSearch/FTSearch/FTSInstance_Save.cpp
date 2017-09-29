#include "StdAfx.h"
#include "FTSInstance.h"

void FTSInstance::createIndex()
{
	uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;
	HArrayFixPair* pKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);

	char documentPath[1024];
	char documentNamePath[1024];
	
	Configuration.getDocumentPath(documentPath);
	Configuration.getDocumentNamePath(documentNamePath);
	
	pDocFile = new BinaryFile(documentPath, true, true);
	pDocNameFile = new BinaryFile(documentNamePath, true, true);

	if(!pDocFile->open())
	{
		logError("File %s is not open.", documentPath);
		
		delete pDocFile;
		
		delete pDocNameFile;

		HArrayFixPair::DeleteArray(pKeysAndValuesRAM);

		return;
	}
		
	if(!pDocNameFile->open())
	{
		logError("File %s is not open.", documentNamePath);
		
		pDocFile->close();
		delete pDocFile;

		delete pDocNameFile;

		HArrayFixPair::DeleteArray(pKeysAndValuesRAM);

		return;
	}

	//save version and info
	pDocFile->setPosition(getDocHeaderSize());
	
	pDocNameFile->writeInt(&UNIQUE_IDENTIFIER);
	pDocNameFile->writeInt(&Info.Version);
	
	uint32 count;
	
	//save haWordsRAM
	count = haWordsRAM.getKeysAndValuesByRange(pKeysAndValuesRAM,
												Info.CountWordsRAM,
												0, 
												0);
	
	//check errors ===========================================
	if(count != Info.CountWordsRAM)
	{
		logError("createIndex.haWordsRAM. Wrong count.");
	}

	if(count)
	{
		for(uint32 i=0; i < count - 1; i++)
		{
			if(pKeysAndValuesRAM[i].compareKeys(pKeysAndValuesRAM[i+1], countKeySegments) != -1)
			{
				logError("createIndex.haWordsRAM. Array is not sorted.");
			}
		}
	}
	//========================================================
	
	uint32 buffFilledLength = 0;

	uint32 controlValue = 0;

	//clear hdd before filling
	for(uint32 i=0; i < Info.CountWordsRAM; i++)
	{
		/*if(key == 141261)
		{
			i = i;
		}*/

		ulong64 position = pDocFile->getPosition() + buffFilledLength;

		/*if(isWord("базист", pKeysAndValues[i].Key[0], pKeysAndValues[i].Key[1]))
		{
			position = position;
		}*/

		//update position to save pages later
		haWordsRAM.insert(pKeysAndValuesRAM[i].Key,
						  position);

		//check errors ===========================================
		for(uint32 j=0; j < haWordsRAM.KeyLen; j++)
		{
			controlValue += pKeysAndValuesRAM[i].Key[j];
		}

		controlValue += position;
		//========================================================
		
		//save doc
		uint32 id = pKeysAndValuesRAM[i].Value;

		DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

		pDocumentsBlock->writeBlocksToFile(pDocFile,
											(uchar8*)pBuffer,
											MAX_SIZE_BUFFER,
											buffFilledLength, 
											0,
											0); 

		pBuffer[buffFilledLength++] = 0; //null terminated
		
		/*if(Configuration.MemoryMode != IN_MEMORY_MODE)
		{
			pDocumentsBlock->clear();
			pDocumentsBlockPool->releaseObject(pDocumentsBlock);
		}*/

		Info.CountWordsHDD++;
	}

	if (Configuration.MemoryMode != IN_MEMORY_MODE)
	{
		pDocumentsBlockPool->releaseObjects();
	}

	Info.ControlValue = controlValue;

	////test hdd 1 =================================================
	//uint32 prevPos = 0;

	//for(uint32 j=0; j<Info.CountWordsHDD; j++)
	//{
	//	uint32 pos = haWordsHDD.getValueByKey(pKeysAndValues[j].Key);

	//	if(pos < prevPos)
	//	{
	//		logError("Wrong HDD");
	//	}

	//	prevPos = pos;
	//}

	////==========================================================

	////test hdd 2 =================================================

	//HArrayFixPair* pKeysAndValuesHDD = new HArrayFixPair[Info.CountWordsHDD];

	////save haWordsRAM
	//count = haWordsHDD.getKeysAndValuesByRange(pKeysAndValuesHDD,
	//											Info.CountWordsHDD,
	//											0, 
	//											0);

	//for(uint32 j=0; j < count; j++)
	//{
	//	if(pKeysAndValues[j].compareKeys(pKeysAndValuesHDD[j]) != 0 ||
	//	   pKeysAndValues[j].Value != pKeysAndValuesHDD[j].Value)
	//	{
	//		logError("Wrong HDD");
	//	}
	//}

	//HArrayFixPair::DeleteArray(pKeysAndValuesHDD);

	/*minKey[0] = 7561572;
	minKey[1] = 0;

	uint32 value = haWordsHDD.getValueByKey(minKey);*/

	////==========================================================

	if(buffFilledLength > 0)
	{
		pDocFile->write(pBuffer, buffFilledLength);
		buffFilledLength = 0;
	}

	documentsName.writeBlocksToFile(pDocNameFile,
									(uchar8*)pBuffer,
									MAX_SIZE_BUFFER,
									buffFilledLength);

	//save dictionary pages
	haWordsHDD.close();

	haWordsRAM.save();

	haWordsHDD.open();

	Info.CountWordsHDD = Info.CountWordsRAM;

	//rollback changes in ram
	
	if(Configuration.MemoryMode == IN_MEMORY_MODE)
	{
		//rollback update
		for(uint32 i=0; i < Info.CountWordsRAM; i++)
		{
			haWordsRAM.insert(pKeysAndValuesRAM[i].Key,
							  pKeysAndValuesRAM[i].Value);
		}
	}
	else
	{
		haWordsRAM.clear();
		
		Info.CountWordsRAM = 0;

		documentsName.clear();
		//documentsName.FilePosition = pDocNameFile->getPosition();
	}
	
	//save header
	Info.LastNameIDHDD = Info.LastNameIDRAM;

	pDocFile->setPosition(0);
	pDocFile->writeInt(&UNIQUE_IDENTIFIER);
	pDocFile->writeInt(&Info.Version);
	pDocFile->write(&Info, sizeof(Info));

	pDocNameFile->flush();
	pDocFile->flush();

	//destroy
	memset(pBuffer, 0, MAX_SIZE_BUFFER);

	HArrayFixPair::DeleteArray(pKeysAndValuesRAM);
}

void FTSInstance::updateIndex()
{
	//init
	HArrayTextFile haWordsHDDTemp;
	uchar8* pSourceBuffer = 0;
	HArrayFixPair* pKeysAndValuesRAM = 0;
	HArrayFixPair* pKeysAndValuesHDD = 0;

	uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;
	pKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);

	char documentPath[1024];
	char documentNamePath[1024];
	char documentPathTemp[1024];
	char documentNamePathTemp[1024];

	Configuration.getDocumentPath(documentPath);
	Configuration.getDocumentNamePath(documentNamePath);

	Configuration.getDocumentTempPath(documentPathTemp);
	Configuration.getDocumentNameTempPath(documentNamePathTemp);

	pDocFile = new BinaryFile(documentPath, false, false); //only read
	pDocNameFile = new BinaryFile(documentNamePath, false, false); //only read

	BinaryFile* pDocFileTemp = new BinaryFile(documentPathTemp, true, true); //create new
	BinaryFile* pDocNameFileTemp = new BinaryFile(documentNamePathTemp, true, true); //create new
	
	//open doc files
	if(!pDocFile->open())
	{
		logError("File %s is not open.", documentPath);
		goto destroy;
	}

	if(!pDocNameFile->open())
	{
		logError("File %s is not open.", documentNamePath);
		goto destroy;
	}

	if(!pDocFileTemp->open())
	{
		logError("File %s is not open.", documentPathTemp);
		goto destroy;
	}

	if(!pDocNameFileTemp->open())
	{
		logError("File %s is not open.", documentNamePathTemp);
		goto destroy;
	}

	//save version and info will be later
	uint32 docHeaderSize = getDocHeaderSize();
	pDocFileTemp->setPosition(docHeaderSize);

	//========================================================
	pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];
	uchar8* pDestBuffer = (uchar8*)pBuffer;
	
	ulong64 sourceFilePosition = docHeaderSize; //version + unique identifier + info
	uint32 sourceBuffPosition = 0;
	uint32 sourceBuffLength = 0;

	uint32 destBuffPosition = 0;
	uint32 destBuffLength = MAX_SIZE_BUFFER;

	//get data from ram
	printf("Get keys from RAM...\n");

	uint32 count = haWordsRAM.getKeysAndValuesByRange(pKeysAndValuesRAM,
													Info.CountWordsRAM, 
													0, 
													0);

	//check errors ===========================================
	if(count != Info.CountWordsRAM)
	{
		logError("createIndex.haWordsRAM. Wrong count.");
		goto destroy;
	}

	if(count)
	{
		for(uint32 i=0; i < count - 1; i++)
		{
			if(pKeysAndValuesRAM[i].compareKeys(pKeysAndValuesRAM[i+1], countKeySegments) != -1)
			{
				logError("createIndex.haWordsRAM. Array is not sorted.");
				goto destroy;
			}
		}
	}

	//========================================================
	haWordsHDD.open();
	//if(Configuration.MemoryMode == IN_MEMORY_MODE)
	//{
	//	haWordsHDD.open();
	//}
	//else
	//{
	//	haWordsHDD.open(); //1 gb on all
	//}

	//get data by portions from HDD
	pKeysAndValuesHDD = HArrayFixPair::CreateArray(MAX_SIZE_BUFFER, countKeySegments);

	uint32 currKeyRAM = 0;

	ulong64 blockNumber = 0;
	uint32 wordInBlock = 0;

	char indexPath[1024];
	Configuration.getIndexPath(indexPath);

	//create temp HDD table
	haWordsHDDTemp.init(indexPath,
						"temp", 
						haWordsRAM.KeyLen);

	haWordsHDDTemp.create();

	//haWordsHDDTemp.create(0, haWordsRAM.countPartitions);

	//copy schema partitions
	/*if (haWordsHDD.countPartitions > 1)
	{
		HeaderCell* headerCells = new HeaderCell[haWordsHDD.HeaderSize];
		haWordsHDD.readHeaderCellsHDD(headerCells, 0, haWordsHDD.HeaderSize);

		for (uint32 i = 0; i < haWordsHDD.HeaderSize; i++)
		{
			if (headerCells[i].Partition)
			{
				*(uint32*)&headerCells[i] = 0;

				haWordsHDDTemp.writeHeaderCellHDD(headerCells[i], i);
			}
		}

		delete[] headerCells;
	}*/

	uint32 oldControlValue = 0;
	uint32 newControlValue = 0;

	while(true)
	{
		//get data
		//bool isBufferNotEnough = false;

		uint32 countHDD = haWordsHDD.getKeysAndValuesByPortions(pKeysAndValuesHDD,
															  MAX_SIZE_BUFFER, 
															  blockNumber,
															  wordInBlock);

		/*if(isBufferNotEnough)
		{
			logError("The buffer MAX_SIZE_BUFFER is not enough to get portion from HDD.");
			goto destroy;
		}*/

		//check errors ===========================================
		if(countHDD)
		{
			for(uint32 i=0; i < countHDD - 1; i++)
			{
				if(pKeysAndValuesHDD[i].compareKeys(pKeysAndValuesHDD[i+1], countKeySegments) != -1)
				{
					logError("createIndex.haWordsHDD. Array is not sorted.");
					goto destroy;
				}
			}
		}

		//========================================================

		if(countHDD == 0 && currKeyRAM == Info.CountWordsRAM)
		{
			//exit
			break;
		}

		uint32 currKeyHDD = 0;

		while(true)
		{
			//merge keys
			printf("\rMerge keys: %u/%u %u/%u", currKeyRAM, Info.CountWordsRAM, currKeyHDD, countHDD);

			int compareResult;
			
			if(currKeyRAM < Info.CountWordsRAM && currKeyHDD < countHDD)
			{
				compareResult = pKeysAndValuesRAM[currKeyRAM].compareKeys(pKeysAndValuesHDD[currKeyHDD], countKeySegments);
			}
			else if(currKeyRAM < Info.CountWordsRAM && countHDD == 0)
			{
				compareResult = -1; //exists only on RAM and no new portions
			}
			else if(currKeyHDD < countHDD)
			{
				compareResult = 1; //exists only on HDD
			}
			else
			{
				break;
			}

			////check capacity, split partitions ===============================
			//char error[1024];
			//haWordsHDDTemp.checkCapacity(error);

			//if (strlen(error))
			//{
			//	logError(error);
			//	goto destroy;
			//}

			//check errors ===========================================
			if(compareResult == 0 || compareResult == 1)
			{
				for(uint32 j=0; j < haWordsRAM.KeyLen; j++)
				{
					oldControlValue += pKeysAndValuesHDD[currKeyHDD].Key[j];
				}

				oldControlValue += pKeysAndValuesHDD[currKeyHDD].Value;
			}
			//========================================================

			if(compareResult == -1) //KEY IS EXISTS ONLY ON RAM ===================================================
			{
				//get positions

				uint32 id = pKeysAndValuesRAM[currKeyRAM].Value;

				DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);

				/*if(isWord("базист", pKeysAndValues[i].Key[0], pKeysAndValues[i].Key[1]))
				{
					docPosition = docPosition;
				}*/

				ulong64 docTempPosition = pDocFileTemp->getPosition() + destBuffPosition;
		
				//insert or update
				haWordsHDDTemp.insert(pKeysAndValuesRAM[currKeyRAM].Key, docTempPosition);
				
				//check errors ===========================================
				for(uint32 i=0; i < haWordsRAM.KeyLen; i++)
				{
					newControlValue += pKeysAndValuesRAM[currKeyRAM].Key[i];
				}

				newControlValue += docTempPosition;
				//========================================================
				
				uint32 lastDocNumber = 0;

				//Save blocks from RAM
				pDocumentsBlock->writeBlocksToFile(pDocFileTemp, 
												   pDestBuffer, 
												   MAX_SIZE_BUFFER, 
												   destBuffPosition,
												   0,
												   lastDocNumber); 
		
				pDestBuffer[destBuffPosition++] = 0; //null terminated
		
				/*if(Configuration.MemoryMode != IN_MEMORY_MODE)
				{
					pDocumentsBlock->clear();
					pDocumentsBlockPool->releaseObject(pDocumentsBlock);
				}*/

				Info.CountWordsHDD++;
				
				currKeyRAM++;
			}
			else if(compareResult == 0) //KEY IS EXISTS ON RAM AND HDD ===================================================
			{
				//get positions
				uint32 id = pKeysAndValuesRAM[currKeyRAM].Value;

				DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->getObject(id);
		
				uint32 docPosition = pKeysAndValuesHDD[currKeyHDD].Value;

				/*if(isWord("базист", pKeysAndValues[i].Key[0], pKeysAndValues[i].Key[1]))
				{
					docPosition = docPosition;
				}*/

				ulong64 docTempPosition = pDocFileTemp->getPosition() + destBuffPosition;
		
				//update
				haWordsHDDTemp.insert(pKeysAndValuesHDD[currKeyHDD].Key, docTempPosition);
				
				//check errors ===========================================
				for(uint32 i=0; i < haWordsRAM.KeyLen; i++)
				{
					newControlValue += pKeysAndValuesHDD[currKeyHDD].Key[i];
				}

				newControlValue += docTempPosition;
				//========================================================

				//insert or update
				uint32 lastDocNumber = 0;

				//A. Read and save blocks from HDD
				if(docPosition == sourceFilePosition + sourceBuffPosition)
				{
					moveDocFileBlocks(pDocFile, 
									  pDocFileTemp, 
									  pSourceBuffer, 
									  pDestBuffer, 
									  sourceFilePosition, 
									  sourceBuffPosition, 
									  sourceBuffLength, 
									  destBuffPosition, 
									  destBuffLength,
									  MAX_SIZE_BUFFER,
									  0,
									  lastDocNumber,
									  Info.LastNameIDRAM);
				}
				else
				{
					logError("Read corrupted.");
					goto destroy;
				}
				
				//B. Save blocks from RAM
				pDocumentsBlock->writeBlocksToFile(pDocFileTemp, 
												   pDestBuffer, 
												   MAX_SIZE_BUFFER, 
												   destBuffPosition,
												   0,
												   lastDocNumber); 
		
				pDestBuffer[destBuffPosition++] = 0; //null terminated
		
				/*if(Configuration.MemoryMode != IN_MEMORY_MODE)
				{
					pDocumentsBlock->clear();
					pDocumentsBlockPool->releaseObject(pDocumentsBlock);
				}*/

				currKeyRAM++;
				currKeyHDD++;
			}
			else //KEY IS EXISTS ONLY ON HDD ===================================================
			{
				//get positions
				uint32 docPosition = pKeysAndValuesHDD[currKeyHDD].Value;

				/*if(isWord("базист", pKeysAndValues[i].Key[0], pKeysAndValues[i].Key[1]))
				{
					docPosition = docPosition;
				}*/

				ulong64 docTempPosition = pDocFileTemp->getPosition() + destBuffPosition;
		
				//update
				haWordsHDDTemp.insert(pKeysAndValuesHDD[currKeyHDD].Key, docTempPosition);

				//check errors ===========================================
				for(uint32 i=0; i < haWordsRAM.KeyLen; i++)
				{
					newControlValue += pKeysAndValuesHDD[currKeyHDD].Key[i];
				}

				newControlValue += docTempPosition;
				//========================================================

				uint32 lastDocNumber = 0;

				//Read and save blocks from HDD
				if(docPosition == sourceFilePosition + sourceBuffPosition)
				{
					moveDocFileBlocks(pDocFile, 
									  pDocFileTemp, 
									  pSourceBuffer, 
									  pDestBuffer, 
									  sourceFilePosition, 
									  sourceBuffPosition, 
									  sourceBuffLength, 
									  destBuffPosition, 
									  destBuffLength,
									  MAX_SIZE_BUFFER,
									  0,
									  lastDocNumber,
									  Info.LastNameIDRAM);
				}
				else
				{
					logError("Read corrupted.");
					goto destroy;
				}
				
				pDestBuffer[destBuffPosition++] = 0; //null terminated
		
				currKeyHDD++;
			}
		}

		printf("\n");
	}

	if (Configuration.MemoryMode != IN_MEMORY_MODE)
	{
		pDocumentsBlockPool->releaseObjects();
	}

	if(destBuffPosition > 0)
	{
		pDocFileTemp->write(pDestBuffer, destBuffPosition);
		destBuffPosition = 0;
	}

	//check errors ===========================================
	if(Info.ControlValue != oldControlValue)
	{
		logError("HDD Control value is wrong.");
		goto destroy;
	}
	else
	{
		Info.ControlValue = newControlValue;
	}
	//========================================================

	////save on hdd
	//for(uint32 i=0; i < Info.CountWordsRAM; i++)
	//{
	//	haWordsHDD.insert(pKeysAndValuesRAM[i].Key,
	//					  pKeysAndValuesRAM[i].Value);
	//}

	//Save DocNames
	//A. Save blocks from HDD
	pDocNameFile->setPosition(0);
	pDocNameFileTemp->setPosition(0);

	BinaryFile::copyFile(pDocNameFile, pDocNameFileTemp);

	//B. Save blocks from RAM
	documentsName.writeBlocksToFile(pDocNameFileTemp,
									pDestBuffer,
									MAX_SIZE_BUFFER,
									destBuffPosition);

	pDocNameFileTemp->flush();
	
	//save info
	haWordsHDDTemp.flush();

	if(Configuration.MemoryMode != IN_MEMORY_MODE)
	{
		haWordsRAM.clear();
		
		Info.CountWordsRAM = 0;

		documentsName.clear();
		//documentsName.FilePosition = pDocNameFile->getPosition();
	}

	//save header
	Info.LastNameIDHDD = Info.LastNameIDRAM;

	pDocFileTemp->setPosition(0);
	pDocFileTemp->writeInt(&UNIQUE_IDENTIFIER);
	pDocFileTemp->writeInt(&Info.Version);
	pDocFileTemp->write(&Info, sizeof(Info));
	pDocFileTemp->flush();

destroy:	
	
	//1. Close tables
	haWordsHDD.close();
	haWordsHDDTemp.close();
	
	closeDicIndex();

	closeDocIndex();

	closeDocNameIndex();

	pDocFileTemp->close();
	delete pDocFileTemp;

	pDocNameFileTemp->close();
	delete pDocNameFileTemp;
	
	if(!Info.HasError)
	{
		//2. Delete old files
		BinaryFile::deleteFile(documentPath);
		BinaryFile::deleteFile(documentNamePath);
		HArrayTextFile::deleteFiles(haWordsHDD.Path, haWordsHDD.TableName);

		//3.Rename tables
		BinaryFile::renameFile(documentPathTemp, documentPath);
		BinaryFile::renameFile(documentNamePathTemp, documentNamePath);
		HArrayTextFile::renameFiles(haWordsHDD.Path, "temp", haWordsHDD.TableName);
	}
	else
	{
		//2. Delete old files
		BinaryFile::deleteFile(documentPathTemp);
		BinaryFile::deleteFile(documentNamePathTemp);
		HArrayTextFile::deleteFiles(haWordsHDDTemp.Path, haWordsHDDTemp.TableName);
	}

	//open new
	haWordsHDD.open();

	memset(pBuffer, 0, MAX_SIZE_BUFFER);

	if(pKeysAndValuesRAM)
	{
		HArrayFixPair::DeleteArray(pKeysAndValuesRAM);
	}

	if(pKeysAndValuesHDD)
	{
		HArrayFixPair::DeleteArray(pKeysAndValuesHDD);
	}

	if(pSourceBuffer)
	{
		delete[] pSourceBuffer;
	}

	openDocIndex();

	openDocNameIndex();
}

void FTSInstance::importIndex(const char* importPath)
{
	//save index first
	if(Info.CountWordsRAM)
	{
		saveIndex();
	}

	closeDicIndex();

	closeDocIndex();

	closeDocNameIndex();

	//init
	FTSConfiguration configurationImport;
	configurationImport.setIndexPath(importPath);

	FTSInstanceInfo infoImport;

	HArrayTextFile haWordsHDDImport;
	HArrayTextFile haWordsHDDTemp;
	
	uchar8* pSourceBuffer = 0;
	uchar8* pSourceBufferImport = 0;

	HArrayFixPair* pKeysAndValuesHDDImport = 0;
	HArrayFixPair* pKeysAndValuesHDD = 0;

	char documentPath[1024];
	char documentNamePath[1024];

	char documentPathImport[1024];
	char documentNamePathImport[1024];

	char documentPathTemp[1024];
	char documentNamePathTemp[1024];

	Configuration.getDocumentPath(documentPath);
	Configuration.getDocumentNamePath(documentNamePath);

	configurationImport.getDocumentPath(documentPathImport);
	configurationImport.getDocumentNamePath(documentNamePathImport);

	Configuration.getDocumentTempPath(documentPathTemp);
	Configuration.getDocumentNameTempPath(documentNamePathTemp);

	pDocFile = new BinaryFile(documentPath, false, false); //only read
	pDocNameFile = new BinaryFile(documentNamePath, false, false); //only read

	BinaryFile* pDocFileImport = new BinaryFile(documentPathImport, false, false); //only read
	BinaryFile* pDocNameFileImport = new BinaryFile(documentNamePathImport, false, false); //only read

	BinaryFile* pDocFileTemp = new BinaryFile(documentPathTemp, true, true); //create new
	BinaryFile* pDocNameFileTemp = new BinaryFile(documentNamePathTemp, true, true); //create new
	
	//open doc files
	if(!pDocFile->open())
	{
		logError("File %s is not open.", documentPath);
		goto destroy;
	}

	if(!pDocNameFile->open())
	{
		logError("File %s is not open.", documentNamePath);
		goto destroy;
	}

	if(!pDocFileImport->open())
	{
		logError("File %s is not open.", documentPathImport);
		goto destroy;
	}

	if(!pDocNameFileImport->open())
	{
		logError("File %s is not open.", documentNamePathImport);
		goto destroy;
	}

	if(!pDocFileTemp->open())
	{
		logError("File %s is not open.", documentPathTemp);
		goto destroy;
	}

	if(!pDocNameFileTemp->open())
	{
		logError("File %s is not open.", documentNamePathTemp);
		goto destroy;
	}

	//read import info
	uint32 uniqueIdentifierImport;
	pDocFileImport->readInt(&uniqueIdentifierImport);

	uint32 codeVersionImport;
	pDocFileImport->readInt(&codeVersionImport);

	pDocFileImport->read(&infoImport, sizeof(FTSInstanceInfo));
	
	infoImport.LastErrorMessage = LastErrorMessage;

	if(uniqueIdentifierImport != UNIQUE_IDENTIFIER ||
	   codeVersionImport != CODE_VERSION ||
	   Info.Version != infoImport.Version ||
	   Info.DocumentNameSize != infoImport.DocumentNameSize)
	{
		logError("Indexes doesn't match with settings.");
		goto destroy;
	}

	//save version and info will be later
	uint32 docHeaderSize = getDocHeaderSize();

	pDocFileTemp->setPosition(docHeaderSize);

	//========================================================
	pSourceBuffer = new uchar8[MAX_SIZE_BUFFER];
	pSourceBufferImport = new uchar8[MAX_SIZE_BUFFER];

	uchar8* pDestBuffer = (uchar8*)pBuffer;
	
	ulong64 sourceFilePosition = docHeaderSize; //version + unique identifier + info
	uint32 sourceBuffPosition = 0;
	uint32 sourceBuffLength = 0;

	ulong64 sourceFilePositionImport = docHeaderSize; //version + unique identifier + info
	uint32 sourceBuffPositionImport = 0;
	uint32 sourceBuffLengthImport = 0;

	uint32 destBuffPosition = 0;
	uint32 destBuffLength = MAX_SIZE_BUFFER;

	//get data by portions from HDD
	uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;

	pKeysAndValuesHDD = HArrayFixPair::CreateArray(MAX_SIZE_BUFFER, countKeySegments);

	//get data from import hdd
	pKeysAndValuesHDDImport = HArrayFixPair::CreateArray(MAX_SIZE_BUFFER, countKeySegments);
	
	uint32 currKeyHDD = MAX_INT;
	uint32 currKeyHDDImport = MAX_INT;
	
	uint32 countHDD = MAX_INT;
	uint32 countHDDImport = MAX_INT;
	
	ulong64 blockNumber = 0;
	uint32 wordInBlock = 0;

	ulong64 blockNumberImport = 0;
	uint32 wordInBlockImport = 0;

	//open haWordsHDD
	haWordsHDD.open();
	
	char indexPath[1024];
	Configuration.getIndexPath(indexPath);

	//create import HDD table
	haWordsHDDImport.init(importPath,
							"", 
							Configuration.AutoStemmingOn / 4);

	haWordsHDDImport.open();

	//create temp HDD table
	haWordsHDDTemp.init(indexPath,
						"temp", 
						Configuration.AutoStemmingOn / 4);

	haWordsHDDTemp.create();
	//haWordsHDDTemp.create(BIN_FILE_BUFFER_SIZE, haWordsHDD.countPartitions);

	//copy schema partitions
	/*if (haWordsHDD.countPartitions > 1)
	{
		HeaderCell* headerCells = new HeaderCell[haWordsHDD.HeaderSize];
		haWordsHDD.readHeaderCellsHDD(headerCells, 0, haWordsHDD.HeaderSize);

		for (uint32 i = 0; i < haWordsHDD.HeaderSize; i++)
		{
			if (headerCells[i].Partition)
			{
				*(uint32*)&headerCells[i] = 0;

				haWordsHDDTemp.writeHeaderCellHDD(headerCells[i], i);
			}
		}

		delete[] headerCells;
	}*/

	uint32 oldControlValue = 0;
	uint32 newControlValue = 0;

	while(true)
	{
		//get data from hdd
		if(currKeyHDD == countHDD && countHDD)
		{
			//bool isBufferNotEnough = false;

			currKeyHDD = 0;
			countHDD = haWordsHDD.getKeysAndValuesByPortions(pKeysAndValuesHDD,
															  MAX_SIZE_BUFFER, 
															  blockNumber,
															  wordInBlock);

			/*if(isBufferNotEnough)
			{
				logError("The buffer MAX_SIZE_BUFFER is not enough to get portion from HDD.");
				goto destroy;
			}*/

			//check errors ===========================================
			if (countHDD)
			{
				for (uint32 i = 0; i < countHDD - 1; i++)
				{
					if (pKeysAndValuesHDD[i].compareKeys(pKeysAndValuesHDD[i + 1], countKeySegments) != -1)
					{
						logError("importIndex.haWordsHDD. Array is not sorted.");
						goto destroy;
					}
				}
			}
		}

		//get data from hdd import
		if(currKeyHDDImport == countHDDImport && countHDDImport)
		{
			//bool isBufferNotEnough = false;

			currKeyHDDImport = 0;
			countHDDImport = haWordsHDDImport.getKeysAndValuesByPortions(pKeysAndValuesHDDImport,
																		MAX_SIZE_BUFFER, 
																		blockNumberImport,
																		wordInBlockImport);

			/*if(isBufferNotEnough)
			{
				logError("The buffer MAX_SIZE_BUFFER is not enough to get portion from HDD.");
				goto destroy;
			}*/
		}
		
		if(!countHDD && !countHDDImport)
		{
			//exit if no merge items
			break;
		}

		printf("\rMerge keys: %u/%u %u/%u", currKeyHDDImport, countHDDImport, currKeyHDD, countHDD);

		int compareResult;
			
		if(currKeyHDD < countHDD && currKeyHDDImport < countHDDImport)
		{
			compareResult = pKeysAndValuesHDDImport[currKeyHDDImport].compareKeys(pKeysAndValuesHDD[currKeyHDD], countKeySegments);
		}
		else if(currKeyHDD < countHDD)
		{
			compareResult = 1; //exists only on HDD
		}
		else if(currKeyHDDImport < countHDDImport)
		{
			compareResult = -1; //exists only on RAM and no new portions
		}
		else
		{
			break;
		}

		//check errors ===========================================
		if(compareResult == 0 || compareResult == 1)
		{
			for(uint32 j=0; j < haWordsRAM.KeyLen; j++)
			{
				oldControlValue += pKeysAndValuesHDD[currKeyHDD].Key[j];
			}

			oldControlValue += pKeysAndValuesHDD[currKeyHDD].Value;
		}
		//========================================================

		if(compareResult == -1) //KEY IS EXISTS ONLY ON IMPORT ===================================================
		{
			//get positions
			ulong64 docPosition = pKeysAndValuesHDDImport[currKeyHDDImport].Value;
			
			ulong64 docTempPosition = pDocFileTemp->getPosition() + destBuffPosition;

			//update
			haWordsHDDTemp.insert(pKeysAndValuesHDDImport[currKeyHDDImport].Key, docTempPosition);

			//check errors ===========================================
			for(uint32 i=0; i < haWordsRAM.KeyLen; i++)
			{
				newControlValue += pKeysAndValuesHDDImport[currKeyHDDImport].Key[i];
			}

			newControlValue += docTempPosition;
			//========================================================

			uint32 lastDocNumber = 0;

			//Read and save blocks from HDD
			if(docPosition == sourceFilePositionImport + sourceBuffPositionImport)
			{
				uint32 baseDocNumber = Info.LastNameIDHDD - docHeaderSize;

				moveDocFileBlocks(pDocFileImport, 
								  pDocFileTemp, 
								  pSourceBufferImport, 
								  pDestBuffer, 
								  sourceFilePositionImport, 
								  sourceBuffPositionImport, 
								  sourceBuffLengthImport, 
								  destBuffPosition, 
								  destBuffLength,
								  MAX_SIZE_BUFFER,
								  baseDocNumber,
								  lastDocNumber,
								  Info.LastNameIDHDD + infoImport.LastNameIDHDD - docHeaderSize);
			}
			else
			{
				logError("Read corrupted.");
				goto destroy;
			}
			
			pDestBuffer[destBuffPosition++] = 0; //null terminated
		
			currKeyHDDImport++;

			//increment words
			Info.CountWordsHDD++;
		}
		else if(compareResult == 0) //KEY IS EXISTS ON IMPORT AND HDD ===================================================
		{
			//get positions
			ulong64 docPosition = pKeysAndValuesHDD[currKeyHDD].Value;

			ulong64 docTempPosition = pDocFileTemp->getPosition() + destBuffPosition;
		
			//update
			haWordsHDDTemp.insert(pKeysAndValuesHDD[currKeyHDD].Key, docTempPosition);

			//check errors ===========================================
			for(uint32 i=0; i < haWordsRAM.KeyLen; i++)
			{
				newControlValue += pKeysAndValuesHDD[currKeyHDD].Key[i];
			}

			newControlValue += docTempPosition;
			//========================================================

			//insert or update
			uint32 lastDocNumber = 0;

			//A. Read and save blocks from HDD
			if(docPosition == sourceFilePosition + sourceBuffPosition)
			{
				moveDocFileBlocks(pDocFile, 
								  pDocFileTemp, 
								  pSourceBuffer, 
								  pDestBuffer, 
								  sourceFilePosition, 
								  sourceBuffPosition, 
								  sourceBuffLength, 
								  destBuffPosition, 
								  destBuffLength,
								  MAX_SIZE_BUFFER,
								  0,
								  lastDocNumber,
								  Info.LastNameIDHDD);
			}
			else
			{
				logError("Read corrupted.");
				goto destroy;
			}
			
			//B. Save blocks from import
			ulong64 docPositionImport = pKeysAndValuesHDDImport[currKeyHDDImport].Value;

			if(docPositionImport == sourceFilePositionImport + sourceBuffPositionImport)
			{
				uint32 baseDocNumber = Info.LastNameIDHDD - docHeaderSize;

				moveDocFileBlocks(pDocFileImport,
								  pDocFileTemp, 
								  pSourceBufferImport, 
								  pDestBuffer, 
								  sourceFilePositionImport, 
								  sourceBuffPositionImport, 
								  sourceBuffLengthImport, 
								  destBuffPosition, 
								  destBuffLength,
								  MAX_SIZE_BUFFER,
								  baseDocNumber,
								  lastDocNumber,
								  Info.LastNameIDHDD + infoImport.LastNameIDHDD - docHeaderSize);
			}
			else
			{
				logError("Read corrupted.");
				goto destroy;
			}
		
			pDestBuffer[destBuffPosition++] = 0; //null terminated
		
			currKeyHDDImport++;
			currKeyHDD++;
		}
		else //KEY IS EXISTS ONLY ON HDD ===================================================
		{
			//get positions
			ulong64 docPosition = pKeysAndValuesHDD[currKeyHDD].Value;

			/*if(isWord("базист", pKeysAndValues[i].Key[0], pKeysAndValues[i].Key[1]))
			{
				docPosition = docPosition;
			}*/

			ulong64 docTempPosition = pDocFileTemp->getPosition() + destBuffPosition;

			//update
			haWordsHDDTemp.insert(pKeysAndValuesHDD[currKeyHDD].Key, docTempPosition);

			//check errors ===========================================
			for(uint32 i=0; i < haWordsRAM.KeyLen; i++)
			{
				newControlValue += pKeysAndValuesHDD[currKeyHDD].Key[i];
			}

			newControlValue += docTempPosition;
			//========================================================

			uint32 lastDocNumber = 0;

			//Read and save blocks from HDD
			if(docPosition == sourceFilePosition + sourceBuffPosition)
			{
				moveDocFileBlocks(pDocFile, 
								  pDocFileTemp, 
								  pSourceBuffer, 
								  pDestBuffer, 
								  sourceFilePosition, 
								  sourceBuffPosition, 
								  sourceBuffLength, 
								  destBuffPosition, 
								  destBuffLength,
								  MAX_SIZE_BUFFER,
								  0,
								  lastDocNumber,
								  Info.LastNameIDHDD);
			}
			else
			{
				logError("Read corrupted.");
				goto destroy;
			}
			
			pDestBuffer[destBuffPosition++] = 0; //null terminated
		
			currKeyHDD++;
		}

		//check capacity, split on partitions
		/*char error[1024];
		haWordsHDDTemp.checkCapacity(error);

		if (strlen(error))
		{
			logError(error);
			goto destroy;
		}*/
	}

	if(destBuffPosition > 0)
	{
		pDocFileTemp->write(pDestBuffer, destBuffPosition);
		destBuffPosition = 0;
	}

	//check errors ===========================================
	if(Info.ControlValue != oldControlValue)
	{
		logError("HDD Control value is wrong.");
		goto destroy;
	}
	else
	{
		Info.ControlValue = newControlValue;
	}
	//========================================================

	//Save DocNames
	//A. Save blocks from HDD
	BinaryFile::copyFile(pDocNameFile, pDocNameFileTemp);

	//B. Save blocks from import HDD
	pDocNameFileImport->setPosition(DOC_NAME_HEADER_SIZE);
	BinaryFile::copyFile(pDocNameFileImport, pDocNameFileTemp);

	pDocNameFileTemp->flush();
	
	//save info
	haWordsHDDTemp.flush();

	if(Configuration.MemoryMode != IN_MEMORY_MODE)
	{
		Info.CountWordsRAM = 0;
	}

	//save header
	Info.LastNameIDHDD += infoImport.LastNameIDHDD - docHeaderSize;
	Info.LastNameIDRAM += infoImport.LastNameIDRAM - docHeaderSize; //new ids from here

	pDocFileTemp->setPosition(0);
	pDocFileTemp->writeInt(&UNIQUE_IDENTIFIER);
	pDocFileTemp->writeInt(&Info.Version);
	pDocFileTemp->write(&Info, sizeof(Info));
	pDocFileTemp->flush();

destroy:	
	
	//1. Close tables
	haWordsHDD.close();
	haWordsHDDImport.close();
	haWordsHDDTemp.close();
	
	closeDocIndex();

	closeDocNameIndex();
	
	//close import index
	pDocFileImport->close();
	delete pDocFileImport;
	pDocFileImport = 0;

	pDocNameFileImport->close();
	delete pDocNameFileImport;
	pDocNameFileImport = 0;

	//close temp index
	pDocFileTemp->close();
	delete pDocFileTemp;
	pDocFileTemp = 0;

	pDocNameFileTemp->close();
	delete pDocNameFileTemp;
	pDocNameFileTemp = 0;
	
	if(!Info.HasError)
	{
		//2. Delete old files
		BinaryFile::deleteFile(documentPath);
		BinaryFile::deleteFile(documentNamePath);
		HArrayTextFile::deleteFiles(haWordsHDD.Path, haWordsHDD.TableName);

		//3.Rename tables
		BinaryFile::renameFile(documentPathTemp, documentPath);
		BinaryFile::renameFile(documentNamePathTemp, documentNamePath);
		HArrayTextFile::renameFiles(haWordsHDD.Path, "temp", haWordsHDD.TableName);
	}
	else
	{
		//2. Delete old files
		BinaryFile::deleteFile(documentPathTemp);
		BinaryFile::deleteFile(documentNamePathTemp);
		HArrayTextFile::deleteFiles(haWordsHDDTemp.Path, haWordsHDDTemp.TableName);
	}

	//open new
	haWordsHDD.open();

	memset(pBuffer, 0, MAX_SIZE_BUFFER);

	if(pKeysAndValuesHDD)
	{
		HArrayFixPair::DeleteArray(pKeysAndValuesHDD);
	}

	if(pKeysAndValuesHDDImport)
	{
		HArrayFixPair::DeleteArray(pKeysAndValuesHDDImport);
	}

	if(pSourceBuffer)
	{
		delete[] pSourceBuffer;
	}

	if(pSourceBufferImport)
	{
		delete[] pSourceBufferImport;
	}

	openDocIndex();

	openDocNameIndex();
}

void FTSInstance::openOrCreateIndex(bool onlyCheckIndex)
{
	closeDicIndex();

	closeDocIndex();

	closeDocNameIndex();

	char indexPath[1024];
	Configuration.getIndexPath(indexPath);

	//create if not exists
	BinaryFile::createDirectory(indexPath);

	//dictionary
	haWordsHDD.openOrCreate();

	//if(Configuration.MemoryMode == IN_MEMORY_MODE ||
	//   Configuration.MemoryMode == HDD_MEMORY_MODE)
	//{
	//	haWordsHDD.openOrCreate();
	//}
	//else //if(Configuration.MemoryMode == HDD_BUFFERED_MEMORY_MODE)
	//{
	//	haWordsHDD.openOrCreate(); //1 gb on all
	//}

	//index
	if(isExistsIndex())
	{
		openIndex(onlyCheckIndex);
	}

	if(Configuration.MemoryMode == IN_MEMORY_MODE)
	{
		closeDicIndex();

		closeDocIndex();

		closeDocNameIndex();
	}
}

void FTSInstance::openIndex(bool onlyCheckIndex)
{
	char documentPath[1024];
	char documentNamePath[1024];
	
	Configuration.getDocumentPath(documentPath);
	Configuration.getDocumentNamePath(documentNamePath);
	
	pDocFile = new BinaryFile(documentPath, false, false);
	pDocNameFile = new BinaryFile(documentNamePath, false, false);
		
	if(!pDocFile->open())
	{
		logError("File %s is not open.", documentPath);
		
		delete pDocFile;
		
		delete pDocNameFile;

		return;
	}
		
	if(!pDocNameFile->open())
	{
		logError("File %s is not open.", documentNamePath);
		
		pDocFile->close();
		delete pDocFile;

		delete pDocNameFile;

		return;
	}

	//read repository version
	uint32 uniqueIdentifier1;
	pDocFile->readInt(&uniqueIdentifier1);

	uint32 codeVersion1;
	pDocFile->readInt(&codeVersion1);

	uint32 uniqueIdentifier2;
	pDocNameFile->readInt(&uniqueIdentifier2);

	uint32 codeVersion2;
	pDocNameFile->readInt(&codeVersion2);

	if(uniqueIdentifier1 == UNIQUE_IDENTIFIER 
	   && codeVersion1 == CODE_VERSION
	   && uniqueIdentifier2 == UNIQUE_IDENTIFIER 
	   && codeVersion2 == CODE_VERSION)
	{
		pDocFile->read(&Info, sizeof(Info));
		
		Info.LastErrorMessage = LastErrorMessage;
		//documentsName.FilePosition = (Info.LastNameID - INDEX_FILE_HEADER_SIZE) * Info.DocumentNameSize + INDEX_FILE_HEADER_SIZE;

		Configuration.WordsHeaderBase = Info.WordsHeaderBase;
		
		//haWordsRAM
		//reinit haWordsRAM
		if(Configuration.MemoryMode == IN_MEMORY_MODE)
		{
			//load data to ram from hdd pages
			haWordsHDD.close();

			haWordsRAM.load();

			Info.CountWordsRAM = Info.CountWordsHDD;
			
			haWordsHDD.open();
			
			//load documents
			uchar8* pSourceBuffer = pBuffer;
			
			ulong64 sourceFilePosition = pDocFile->getPosition(); //version + unique identifier + info
			uint32 sourceBuffPosition = 0;
			uint32 sourceBuffLength = 0;

			uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;
						
			HArrayFixPair* pKeysAndValuesRAM = HArrayFixPair::CreateArray(Info.CountWordsRAM, countKeySegments);

			uint32 count = haWordsRAM.getKeysAndValuesByRange(pKeysAndValuesRAM,
															Info.CountWordsRAM, 
															0, 
															0);
			
			if(count != Info.CountWordsRAM)
			{
				logError("Count != Info.CountWordsRAM");
				
				goto destroy1;
			}

			for(uint32 i=0; i < count - 1; i++)
			{
				if(pKeysAndValuesRAM[i].compareKeys(pKeysAndValuesRAM[i+1], countKeySegments) != -1)
				{
					logError("openIndex.haWordsRAM. Array is not sorted.");
				}
			}

			bool isFormatCorrupted = false;

			uint32 id;

			DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->newObject(id);

			for(uint32 i=0; i < Info.CountWordsRAM; i++)
			{
				//check errors ================
				//checkKeyAndValue("readIndex.haWordsRAM", prevKey, key, value, MAX_INT);
				//prevKey = key;
				//=============================

				/*if(key == getCode("базист", 6))
				{
					key = key;
				}*/

				if(pKeysAndValuesRAM[i].Value == sourceFilePosition + sourceBuffPosition)
				{
					pDocumentsBlock->readBlocksFromFile(pDocFile,
														pSourceBuffer,
														sourceFilePosition,
														sourceBuffPosition,
														sourceBuffLength,
														MAX_SIZE_BUFFER,
														Info.LastNameIDRAM,
														isFormatCorrupted);

					if(isFormatCorrupted)
					{
						logError("Read corrupted.");
						
						goto destroy1;
					}
				}
				else
				{
					logError("Read corrupted.");

					goto destroy1;
				}

				if(!onlyCheckIndex)
				{
					haWordsRAM.insert(pKeysAndValuesRAM[i].Key, id);
				}
				else
				{
					//FOR TESTING !!! =====================================
					pDocumentsBlock->clear();
					//pDocumentsBlockPool->releaseObject(pDocumentsBlock);
				}

				//pKeysAndValuesRAM[i].Value = pDocumentsBlock->CountDocuments;
				//=====================================================

				if(getUsedMemory() >= Configuration.LimitUsedMemory)
				{
					logError("Configuration.LimitUsedMemory is exceed.");

					goto destroy1;
				}

				if(Info.HasError)
				{
					goto destroy1;
				}
			}

			//move doc name file to memory
			sourceFilePosition = pDocNameFile->getPosition(); //version + unique identifier
			moveDocNameFileBlocksToRAM(pDocNameFile,
										&documentsName,
										sourceFilePosition,
										pSourceBuffer,
										MAX_SIZE_BUFFER);
			
			////get top words
			//for(uint32 i = 0; i < Info.CountWordsRAM; i++)
			//{
			//	printf("\r%u", i);

			//	for(uint32 j = i + 1; j < Info.CountWordsRAM; j++)
			//	{
			//		if(pKeysAndValuesRAM[i].Value < pKeysAndValuesRAM[j].Value)
			//		{
			//			HArrayFixPair tempPair = pKeysAndValuesRAM[j];
			//			pKeysAndValuesRAM[j] = pKeysAndValuesRAM[i];
			//			pKeysAndValuesRAM[i] = tempPair;
			//		}
			//	}
			//}

			//BinaryFile bf("c:\\fts\\top.txt", true, true);
			//bf.open();

			//for(uint32 i = 0; i < Info.CountWordsRAM; i++)
			//{
			//	char word[10];
			//	pKeysAndValuesRAM[i].getWord(word);

			//	char buff[256];
			//	sprintf(buff, "%s\t%u\n", word,  pKeysAndValuesRAM[i].Value);

			//	bf.write(buff, strlen(buff));
			//}

			//bf.close();

			destroy1:

			memset(pBuffer, 0, MAX_SIZE_BUFFER);
						
			HArrayFixPair::DeleteArray(pKeysAndValuesRAM);

			//documentsName.FilePosition = 0;
		}
		else
		{
			if (onlyCheckIndex)
			{
				uchar8* pSourceBuffer = (uchar8*)pBuffer;

				ulong64 sourceFilePosition = getDocHeaderSize(); //version + unique identifier + info
				uint32 sourceBuffPosition = 0;
				uint32 sourceBuffLength = 0;


				//get data by portions from HDD
				uint32 countKeySegments = Configuration.AutoStemmingOn >> 2;
				HArrayFixPair* pKeysAndValuesHDD = HArrayFixPair::CreateArray(MAX_SIZE_BUFFER, countKeySegments);

				uint32 currKeyRAM = 0;

				ulong64 blockNumber = 0;
				uint32 wordInBlock = 0;

				uint32 id;

				DocumentsBlock* pDocumentsBlock = pDocumentsBlockPool->newObject(id);

				while (true)
				{
					//get data
					//bool isBufferNotEnough = false;

					uint32 countHDD = haWordsHDD.getKeysAndValuesByPortions(pKeysAndValuesHDD,
																			MAX_SIZE_BUFFER,
																			blockNumber,
																			wordInBlock);

					/*if (isBufferNotEnough)
					{
						logError("The buffer MAX_SIZE_BUFFER is not enough to get portion from HDD.");
						goto destroy2;
					}*/

					//check errors ===========================================
					if (countHDD)
					{
						for (uint32 i = 0; i < countHDD - 1; i++)
						{
							if (pKeysAndValuesHDD[i].compareKeys(pKeysAndValuesHDD[i + 1], countKeySegments) != -1)
							{
								logError("createIndex.haWordsHDD. Array is not sorted.");
								goto destroy2;
							}
						}
					}

					//========================================================

					if (countHDD == 0)
					{
						//exit
						break;
					}

					uint32 currKeyHDD = 0;

					while (currKeyHDD < countHDD)
					{
						//get positions
						if (pKeysAndValuesHDD[currKeyHDD].Value == sourceFilePosition + sourceBuffPosition)
						{
							bool isFormatCorrupted = false;

							pDocumentsBlock->readBlocksFromFile(pDocFile,
																pSourceBuffer,
																sourceFilePosition,
																sourceBuffPosition,
																sourceBuffLength,
																MAX_SIZE_BUFFER,
																Info.LastNameIDRAM,
																isFormatCorrupted);

							if (isFormatCorrupted)
							{
								logError("Read corrupted.");

								goto destroy2;
							}
						}
						else
						{
							logError("Read corrupted.");

							goto destroy2;
						}

						pDocumentsBlock->clear();

						currKeyHDD++;
					}
				}

			destroy2:
				HArrayFixPair::DeleteArray(pKeysAndValuesHDD);
			}

		

			Info.CountWordsRAM = 0;
		}
	}
	else
	{
		logError("Bad version.");
	}
}

bool FTSInstance::isExistsIndex()
{
	char documentPath[1024];
	
	Configuration.getDocumentPath(documentPath);

	return BinaryFile::existsFile(documentPath);
}

//0. Normal
//1. Commit
//2. Rollback

uint32 FTSInstance::getRecoveryIndexState()
{
	char documentPathTemp[1024];
	char documentNamePathTemp[1024];

	Configuration.getDocumentTempPath(documentPathTemp);
	Configuration.getDocumentNameTempPath(documentNamePathTemp);

	bool isDocumentPathTempExists = BinaryFile::existsFile(documentPathTemp);
	bool isDocumentNamePathTempExists = BinaryFile::existsFile(documentNamePathTemp);

	if(!isDocumentPathTempExists && 
	   !isDocumentNamePathTempExists)
	{
		//1. if there are no temp files, than Ok
		return 0;
	}
	else if(isDocumentPathTempExists &&
		    isDocumentNamePathTempExists)
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

void FTSInstance::recoveryIndex()
{
	//char indexPath[1024];
	//Configuration.getIndexPath(indexPath);

	//uint32 recoveryDictionaryState = HArrayTextFile::getRecoveryIndexState(indexPath, "temp");
	//uint32 recoveryIndexState = getRecoveryIndexState();
	//
	//if(recoveryDictionaryState == 0 && recoveryIndexState == 0)
	//{
	//	//normal
	//	return;
	//}
	//else if(recoveryDictionaryState == 2 || recoveryIndexState == 2)
	//{
	//	//rollback
	//	HArrayFixHDD::recoveryIndex(indexPath, "", "temp", 2);
	//	recoveryIndex(2);
	//}
	//else
	//{
	//	//commit
	//	HArrayFixHDD::recoveryIndex(indexPath, "", "temp", 1);
	//	recoveryIndex(1);
	//}
}

void FTSInstance::recoveryIndex(uint32 recoveryIndexState)
{
	if(recoveryIndexState == 0)
	{
		//1. if there are no temp files, than Ok
		return;
	}

	char documentPathTemp[1024];
	char documentNamePathTemp[1024];

	Configuration.getDocumentTempPath(documentPathTemp);
	Configuration.getDocumentNameTempPath(documentNamePathTemp);

	if(recoveryIndexState == 2)
	{
		//2. if there are all temp files, they are corrupted, just delete them
		BinaryFile::deleteFile(documentPathTemp);
		BinaryFile::deleteFile(documentNamePathTemp);
	}
	else if(recoveryIndexState == 1)
	{
		char documentPath[1024];
		char documentNamePath[1024];
	
		Configuration.getDocumentPath(documentPath);
		Configuration.getDocumentNamePath(documentNamePath);

		bool isDocumentPathTempExists = BinaryFile::existsFile(documentPathTemp);
		bool isDocumentNamePathTempExists = BinaryFile::existsFile(documentNamePathTemp);
		
		//3. if there are some temp files, they are ok, rename rest of them
		if(isDocumentPathTempExists)
		{
			BinaryFile::renameFile(documentPathTemp, documentPath);
		}

		if(isDocumentNamePathTempExists)
		{
			BinaryFile::renameFile(documentNamePathTemp, documentNamePath);
		}
	}
}

void FTSInstance::saveIndex()
{
	closeDicIndex();

	closeDocIndex();

	closeDocNameIndex();

	if(Configuration.MemoryMode == IN_MEMORY_MODE)
	{
		createIndex();

		closeDicIndex();

		closeDocIndex();

		closeDocNameIndex();
	}
	else
	{
		char indexPath[1024];
		Configuration.getIndexPath(indexPath);

		//create if not exists
		BinaryFile::createDirectory(indexPath);

		if(isExistsIndex())
		{
			updateIndex();
		}
		else
		{
			createIndex();			
		}

		if (Configuration.IsCreateNewInstanceOnUpdate)
		{
			closeDicIndex();

			closeDocIndex();

			closeDocNameIndex();

			Configuration.InstanceNumber++;

			/*if (!BinaryFile::createDirectory(Configuration.getIndexPath()))
			{
				logError("Can't create directory.");

				return;
			}*/

			Configuration.getIndexPath(haWordsRAM.Path);
			Configuration.getIndexPath(haWordsHDD.Path);

			Info.clear();
		}
	}
}

