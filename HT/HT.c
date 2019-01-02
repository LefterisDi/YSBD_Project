/* File: Ht.c */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "HT.h"
#include "../BF/BF.h"

int HashFunc(const int id, const int mask)
{
    return id % mask;
}

// int BlockAdd(const int fileDesc, const int blockID, Block** block)
// {
//     // if (BF_AllocateBlock(fileDesc) < 0) {
//     //     BF_PrintError("Error allocating block");
//     //     return -1;
//     // }
//
//     (*block)->nextBlock = BF_GetBlockCounter(fileDesc)/* - 1*/;
//
//     /*
//      * We write the changes of the current block to
//      *  the disk before we move to the next one.
//      */
//     if (BF_WriteBlock(fileDesc , blockID) < 0) {
//         BF_PrintError("Error writing block back");
//         return -1;
//     }
//
//     if (BF_ReadBlock(fileDesc , (*block)->nextBlock , (void **)block) < 0) {
//         BF_PrintError("Error getting block");
//         return -1;
//     }
//
//     (*block)->nextBlock = -1;
//
//     int entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
//
//     (*block)->rec = (Record **)malloc(entries * sizeof(Record *));
//
//     int i;
//     for (i = 0; i < entries; i++)
//     {
//         // (*block)->rec[i] = (Record *)malloc(sizeof(Record));
//         (*block)->rec[i] = NULL;
//     }
// }

int BlockInit(const int fileDesc/*, const int blockID*/)
{
	// Block* initialBlock;
	Block* block;
    int blockID;

	// initialBlock = (Block*)malloc(sizeof(Block));

    if (BF_AllocateBlock(fileDesc) < 0) {
        BF_PrintError("Error allocating block");
        // free(initialBlock);
        return -1;
    }

    blockID = BF_GetBlockCounter(fileDesc) - 1;

	if (BF_ReadBlock(fileDesc , blockID , (void **)&block) < 0) {
		BF_PrintError("Error getting block");
        // free(initialBlock);
		return -1;
	}

    // initialBlock->nextBlock = -1;

	int entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);

    // initialBlock->rec = (Record **)malloc(entries * sizeof(Record *));
    block->rec = (Record **)malloc(entries * sizeof(Record *));

    for (int i = 0; i < entries; i++)
    {
        block->rec[i] = NULL;
    }

	// memcpy(block , initialBlock , sizeof(Block));

    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        // free(initialBlock);
        return -1;
    }

    // free(initialBlock);

    return blockID;
}

int HT_CreateIndex(char* fileName, char attrType, char* attrName, int attrLength, int buckets)
{
    int      fileDesc;
    HT_info* block;
    // HT_info info;

	if (BF_CreateFile(fileName) < 0) {
		BF_PrintError("Error creating file");
		return -1;
	}

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
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

    // info.fileDesc   = file;
    // info.attrName   = attrName;
    // info.attrLength = attrLength;
    // info.attrType   = attrType;
    // info.numBuckets = buckets;

    block->fileDesc   = fileDesc;
    block->attrName   = malloc(sizeof(attrName));
    strcpy(block->attrName,attrName);
    block->attrLength = attrLength;
    block->attrType   = attrType;
    block->numBuckets = buckets;

    //copy content to the block
	// memcpy(block, &info, sizeof(HT_info));

    if (BF_WriteBlock(fileDesc , 0 /*BF_GetBlockCounter(fileDesc) - 1*/) < 0) {
		BF_PrintError("Error writing block back");
		return -1;
	}

	for (int i = 1 ; i <= buckets ; i++) {
		BlockInit(fileDesc/*,i*/);
	}

    return 0;
}

HT_info* HT_OpenIndex(char* fileName)
{
    HT_info* block;
    int fileDesc;

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&block) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

    return block;
}

int HT_CloseIndex(HT_info* header_info)
{
    int temp = header_info->fileDesc;
    free(header_info->attrName);
    // free(header_info);
    // if (BF_CloseFile(temp) < 0) {
	// 	BF_PrintError("Error closing file");
	// 	return -1;
	// }
    return 0;

}

int HT_InsertEntry(HT_info header_info, Record record)
{
    int    bucket = HashFunc(record.id, header_info.numBuckets);
    int    blockID = bucket;
    Block* block;

    while(1)
    {
        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        if (block->nextBlock == -1)
            blockID = block->nextBlock;
        else
            break;
    }

    int i;
    int entries = sizeof(*(block->rec)) / sizeof(Record);
    for (i = 0; i < entries; i++)
    {
        if (block->rec[i] == NULL)
            break;
    }

    if (i == entries)
    {
        // Record* rec;

        blockID = BlockInit(header_info.fileDesc);
        block->nextBlock = blockID;
    }

    if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
        BF_PrintError("Error getting block");
        return -1;
    }

    block->rec[i % entries]->id = record.id;
    strcpy(block->rec[i % entries]->name    , record.name);
    strcpy(block->rec[i % entries]->surname , record.surname);
    strcpy(block->rec[i % entries]->address , record.address);

    // if (BF_WriteBlock(header_info.fileDesc , BF_GetBlockCounter(header_info.fileDesc) - 1) < 0) {
    if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

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

int HT_DeleteEntry(HT_info header_info, void* value)
{

}

int HT_GetAllEntries( HT_info header_info, void* value)
{

}
