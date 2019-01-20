/* File: auxFuncs.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxFuncs.h"
#include "../SHT/SHT.h"
#include "../HT/HT.h"
#include "../BF/BF.h"

int HashFunc(const unsigned int id, const int mask)
{
    return id % mask;
}

unsigned int strtoi(const char* str)
{
    int i;
    int len = strlen(str);

    unsigned int key = 0;

    for(i = 0; i < len; i++)
    {
        key = (key << 5) | (key >> 27);
        key += (unsigned int) str[i];
    }

    return key;
}

int HTBlockInit(const int fileDesc)
{
	Block* block;
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

    int entries = MAX_PRIM_RECS;

    block->nextBlock    = -1;
    block->availablePos = true;

    for (int i = 0 ; i < entries ; i++)
    {
        block->rec[i].id = -1;
        block->rec[i].name   [0] = '\0';
        block->rec[i].surname[0] = '\0';
        block->rec[i].address[0] = '\0';
    }

    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return blockID;
}

int SHTBlockInit(const int fileDesc)
{
	SecondaryBlock* sblock;
    int blockID;

    if (BF_AllocateBlock(fileDesc) < 0) {
        BF_PrintError("Error allocating block");
        return -1;
    }

    blockID = BF_GetBlockCounter(fileDesc) - 1;

	if (BF_ReadBlock(fileDesc , blockID , (void **)&sblock) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    sblock->nextBlock    = -1;
    sblock->availablePos = true;

	int entries = MAX_SEC_RECS;

    for (int i = 0 ; i < entries ; i++)
    {
        sblock->rec[i].blockId       = -1;
        sblock->rec[i].id            = -1;
        sblock->rec[i].secHashKey[0] = '\0';
    }

    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return blockID;
}
