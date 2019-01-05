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

	     if (!strcmp(header_info.attrName , "Id"))		pkey = secRec.record.id;
	else if (!strcmp(header_info.attrName , "Name"))    pkey = strtoi(secRec.record.name);
	else if (!strcmp(header_info.attrName , "Surname")) pkey = strtoi(secRec.record.surname);
	else if (!strcmp(header_info.attrName , "Address")) pkey = strtoi(secRec.record.address);

	int blockID = HashFunc(pkey, header_info.numBuckets) + 1;

    printf("ENTRIES = %d\n",entries);
    printf("REC ID = %d\n", secRec.record.id);
    printf("BLOCKID = %d\n",blockID);
    int    i;
    bool   entryExists = true;

    // printf("ATTR TYPE FROM INSERT ENTRY = %c\n", header_info.attrType);

    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    while(1)
    {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
        if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }
        printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");

        for (i = 0 ; i < entries ; i++)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");
            if (sblock->rec[i] == NULL)
            {
                printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
                entryExists = false;
                break;
            }

            printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");

				 if (!strcmp(header_info.attrName , "Id"))		if (sblock->rec[i]->record.id == secRec.record.id)                   return -1;
			else if (!strcmp(header_info.attrName , "Name"))    if (!strcmp(sblock->rec[i]->record.name    , secRec.record.name))    return -1;
			else if (!strcmp(header_info.attrName , "Surname")) if (!strcmp(sblock->rec[i]->record.surname , secRec.record.surname)) return -1;
			else if (!strcmp(header_info.attrName , "Address")) if (!strcmp(sblock->rec[i]->record.address , secRec.record.address)) return -1;

            // if (sblock->rec[i]->record.id == secRec.record.id)
            // {
                printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
                // return -1;
            // } // if
            printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");
        } // for

        printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
        if (!entryExists)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");
            break;
        }
        printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");

        if (sblock->nextBlock != -1)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
            blockID = sblock->nextBlock;
            printf("BLOCKID = %d\n",blockID);
        }
        else
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 13\n");
            break;
        }
    } // while
    printf("BLOCKID = %d\n",blockID);

    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 14\n");
    // while(1)
    // {
    //     if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
    //         BF_PrintError("Error getting block");
    //         return -1;
    //     }
    //
    //     if (block->nextBlock != -1)
    //         blockID = block->nextBlock;
    //     else
    //         break;
    // }
    //
    // int i;
    // // int entries = sizeof(*(block->rec)) / sizeof(Record);
    // for (i = 0 ; i < entries ; i++)
    // {
    //     if (block->rec[i] == NULL)
    //         break;
    // }

    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 15\n");
    printf("I : %d\n" , i);
    if (i == entries)
    {
        // Record* rec;
        printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 16\n");

        blockID = SHTBlockInit(header_info.sfileDesc);
        printf("D: %d\n", BF_GetBlockCounter(header_info.sfileDesc) - 1);
        // sleep(1);
        sblock->nextBlock = blockID;
        printf("BLOCKID = %d\n",blockID);
    }

    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 17\n");
    if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 18\n");
        BF_PrintError("Error getting block");
        return -1;
    }
    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 19\n");

    int index = i % entries;

    sblock->rec[index] = (SecondaryRecord *)malloc(sizeof(SecondaryRecord));
    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 20\n");

    sblock->rec[index]->blockId   = secRec.blockId;
    sblock->rec[index]->record.id = secRec.record.id;

    strcpy(sblock->rec[index]->record.name    , secRec.record.name);
    strcpy(sblock->rec[index]->record.surname , secRec.record.surname);
    strcpy(sblock->rec[index]->record.address , secRec.record.address);

    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 21\n");
    // if (BF_WriteBlock(header_info.fileDesc , BF_GetBlockCounter(header_info.fileDesc) - 1) < 0) {
    if (BF_WriteBlock(header_info.sfileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }
    printf("BLOCKID = %d\n",blockID);
    printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 22\n");

    // printf("ATTR TYPE FROM INSERT ENTRY = %c\n", header_info.attrType);

    return blockID;

    // // block->nextBlock = BlockInit(header_info.fileDesc);
    // // blockID = block->nextBlock;
    //
    // if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
    //     BF_PrintError("Error getting block");
    //     return -1;
    // }
    //
    // memcpy(block->rec[i], &record, sizeof(Record));
    //
    // if (BF_WriteBlock(header_info.fileDesc , BF_GetBlockCounter(header_info.fileDesc) - 1) < 0) {
    //     BF_PrintError("Error writing block back");
    //     return -1;
    // }
    //
    // return blockID;
}

int SHT_GetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value)
{

}
