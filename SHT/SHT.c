#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "SHT.h"
#include "../BF/BF.h"
#include "../HT/HT.h"


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

int SHT_CreateSecondaryIndex(char* sfileName , char* attrName , int attrLength , int buckets , char* primaryFileName){

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
