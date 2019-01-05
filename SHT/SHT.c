/* File: SHT.c */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "SHT.h"
#include "../BF/BF.h"

int SHTBlockInit(const int fileDesc)
{
	SecondaryBlock* block;
    int blockID;

    if (BF_AllocateBlock(fileDesc) < 0) {
        BF_PrintError("Error allocating block");
        return -1;
    }

    blockID = BF_GetBlockCounter(fileDesc) - 1;

	if (BF_ReadBlock(fileDesc , blockID , (void **)&block) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    block->nextBlock = -1;

	int entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(SecondaryRecord);

    block->rec = (SecondaryRecord **)malloc(entries * sizeof(SecondaryRecord *));
	if (block->rec == NULL) {
		perror("Cannot allocate memory");
		return -1;
	}

    for (int i = 0 ; i < entries ; i++)
        block->rec[i] = NULL;

    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return blockID;
}

int SHT_CreateSecondaryIndex(char* sfileName , char* attrName , int attrLength , int buckets , char* primaryFileName)
{
    int      fileDesc;
    SHT_info* block;

	if (BF_CreateFile(sfileName) < 0) {
		BF_PrintError("Error creating file");
		return -1;
	}

    if ((fileDesc = BF_OpenFile(sfileName)) < 0) {
		BF_PrintError("Error opening file");
		return -1;
	}

    if (BF_AllocateBlock(fileDesc) < 0) {
		BF_PrintError("Error allocating block");
		return -1;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&block) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    block->sfileDesc   = fileDesc;
    block->attrName   = attrName;
    block->attrLength = attrLength;
    block->numBuckets = buckets;
    block->fileName = primaryFileName;

    if (BF_WriteBlock(fileDesc , 0 ) < 0) {
		BF_PrintError("Error writing block back");
		return -1;
	}

    for (int i = 1 ; i <= buckets ; i++) {
		SHTBlockInit(fileDesc);
	}

    if (BF_CloseFile(fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}
}


SHT_info* SHT_OpenSecondaryIndex(char* sfileName)
{
    SHT_info* block;
    int fileDesc;

    if ((fileDesc = BF_OpenFile(sfileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&block) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

    return block;
}


int SHT_CloseSecondaryIndex( SHT_info* header_info )
{
    if (BF_CloseFile(header_info->sfileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

    return 0;
}

int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord secRec)
{
    SecondaryBlock* sblock;
    int entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
	int pkey = -1;

	     // if (!strcmp(header_info.attrName , "Id"))		pkey = secRec.record.id;
	 	 if (!strcmp(header_info.attrName , "Name"))    pkey = strtoi(secRec.record.name);
	else if (!strcmp(header_info.attrName , "Surname")) pkey = strtoi(secRec.record.surname);
	else if (!strcmp(header_info.attrName , "Address")) pkey = strtoi(secRec.record.address);

	int blockID = HashFunc(pkey, header_info.numBuckets) + 1;

    printf("ENTRIES = %d\n",entries);
    printf("REC ID = %d\n", secRec.record.id);
    printf("BLOCKID = %d\n",blockID);
    int    i;
    bool   availablePos = false;

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");

    while(1)
    {
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
        if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");

        for (i = 0 ; i < entries ; i++)
        {
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");
            if (sblock->rec[i] == NULL)
            {
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
                availablePos = true;
                break;
            }
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");
        }

		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
		if (availablePos)
			break;
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");

        if (sblock->nextBlock != -1)
        {
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
            blockID = sblock->nextBlock;
            printf("BLOCKID = %d\n",blockID);
        }
        else
        {
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");
            break;
        }
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
    } // while

	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
    printf("BLOCKID = %d\n",blockID);

    printf("I : %d\n" , i);
    if (i == entries)
    {
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 13\n");
        blockID = SHTBlockInit(header_info.sfileDesc);
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 14\n");

        printf("D: %d\n", BF_GetBlockCounter(header_info.sfileDesc) - 1);

        sblock->nextBlock = blockID;

        printf("BLOCKID = %d\n",blockID);
    }

	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 15\n");
    if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
        BF_PrintError("Error getting block");
        return -1;
    }

	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 16\n");
    int index = i % entries;

    sblock->rec[index] = (SecondaryRecord *)malloc(sizeof(SecondaryRecord));
	if (sblock->rec[index] == NULL) {
		perror("Cannot allocate memory");
		return -1;
	}
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 17\n");

    sblock->rec[index]->blockId   = secRec.blockId;
    sblock->rec[index]->record.id = secRec.record.id;

    strcpy(sblock->rec[index]->record.name    , secRec.record.name);
    strcpy(sblock->rec[index]->record.surname , secRec.record.surname);
    strcpy(sblock->rec[index]->record.address , secRec.record.address);
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 18\n");

    if (BF_WriteBlock(header_info.sfileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 19\n");
    printf("BLOCKID = %d\n",blockID);

    return blockID;
}

int SHT_GetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value)
{
    Block* block;
    int    entries     = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
    int    numOfBlocks = 0;
    int    blockID;
    int    pkey;

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    switch (header_info.attrType)
    {
        case 'c':
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
            pkey = strtoi((char *)value);
            printf("GIVEN ID STR = %s\n", (char *)value);
        break;

        case 'i':
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
            pkey = *(int *)value;
            printf("GIVEN ID INT = %d\n", *(int *)value);
        break;

        default:
            printf("ATRR TYPE = %c\n", header_info.attrType);
        break;
    }
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");

    printf("INFO FROM GET ALL ENTRIES: FileDesc = %d\n", header_info.fileDesc);
    printf("INFO FROM GET ALL ENTRIES: AttrType = %c\n", header_info.attrType);
    printf("INFO FROM GET ALL ENTRIES: AttrName = %s\n", header_info.attrName);
    printf("INFO FROM GET ALL ENTRIES: AttrLen  = %d\n", header_info.attrLength);
    printf("INFO FROM GET ALL ENTRIES: Buckets  = %ld\n", header_info.numBuckets);
    blockID = HashFunc(pkey , header_info.numBuckets) + 1;
    printf("PKEY = %d\n", pkey);
    printf("BLOCKID FROM GET ALL ENTRIES = %d\n", blockID);
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");

    while(blockID != -1)
    {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");
        numOfBlocks++;

        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
            BF_PrintError("Error getting block");
            return -1;
        }
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");

        for (int i = 0 ; i < entries ; i++)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
            if (block->rec[i] == NULL)
                return -1;
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");

            if (block->rec[i]->id == pkey)
            {
                printf("     ID: %d\n", block->rec[i]->id);
                printf("   Name: %s\n", block->rec[i]->name);
                printf("Surname: %s\n", block->rec[i]->surname);
                printf("Address: %s\n", block->rec[i]->address);

                return numOfBlocks;
            } // if
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
        } // for

        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
        blockID = block->nextBlock;
    } // while

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 13\n");
    return -1;
}
