/* File: Ht.c */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "HT.h"
// #include "HashFuncs.h"
#include "../BF/BF.h"

int HashFunc(const int id, const int mask)
{
    return (id % mask) + 1;
}

int strtoi(const char* str)
{
    int i;
    int len = strlen(str);

    int key = 0;

    for(i = 0; i < len; i++)
    {
        key = (key << 5) | (key >> 27);
        key += (unsigned int) str[i];
    }

    return key;
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
    block->nextBlock = -1;

	int entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);

    // initialBlock->rec = (Record **)malloc(entries * sizeof(Record *));
    block->rec = (Record **)malloc(entries * sizeof(Record *));

    for (int i = 0 ; i < entries ; i++)
        block->rec[i] = NULL;

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
    block->attrName   = (char *)malloc(sizeof(attrName));
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
    if (BF_CloseFile(temp) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}
    // free(header_info);
    return 0;

}

int HT_InsertEntry(HT_info header_info, Record record)
{
    Block* block;
    int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
    int    blockID = HashFunc(record.id, header_info.numBuckets);
    int    i;
    bool   entryExists = true;

    while(1)
    {
        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (i = 0 ; i < entries ; i++)
        {
            if (block->rec[i] == NULL)
            {
                entryExists = false;
                break;
            }

            if (block->rec[i]->id == record.id)
            {
                return -1;
            } // if
        } // for

        if (!entryExists)
        {
            break;
        }

        if (block->nextBlock != -1)
        {
            blockID = block->nextBlock;
        }
        else
        {
            break;
        }
    } // while

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

    printf("I : %d\n" , i);
    if (i == entries)
    {
        // Record* rec;

        blockID = BlockInit(header_info.fileDesc);
        printf("D: %d", BF_GetBlockCounter(header_info.fileDesc));
        block->nextBlock = blockID;
    }

    if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
        BF_PrintError("Error getting block");
        return -1;
    }

    int index = i % entries;

    block->rec[index] = (Record *)malloc(sizeof(Record));

    block->rec[index]->id = record.id;
    strcpy(block->rec[index]->name    , record.name);
    strcpy(block->rec[index]->surname , record.surname);
    strcpy(block->rec[index]->address , record.address);

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
    Block* block;
    int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
    int    blockID;
    int    pkey;

    switch (header_info.attrType)
    {
        case 'c':
            pkey = strtoi((char *)value);
        break;

        case 'i':
            pkey = *(int *)value;
        break;
    }

    blockID = HashFunc(pkey , header_info.numBuckets);

    for (int blockIndex = 0 ; blockID != -1 ; blockIndex++)
    {
        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
            if (block->rec[i] == NULL)
                return -1;

            if (block->rec[i]->id == pkey)
            {
                Block* currBlock   = block;
                int    currBlockID = block->nextBlock;
                int    prevBlockID = blockID;
                int    entryIndex  = i + 1;
                // int    currBlockID = blockID;

                free(block->rec[i]);
                block->rec[i] = NULL;

                // if (block->nextBlock == -1 && blockIndex != 0)
                // {
                //
                // }

                /*
                 * We first check if there is only one block, which we
                 * shouldn't delete and then, we search for the last block.
                 */
                if (currBlockID != -1)
                {
                    while(1)
                    {
                        if (BF_ReadBlock(header_info.fileDesc , currBlockID , (void **)&currBlock) < 0) {
                            BF_PrintError("Error getting block");
                            return -1;
                        }

                        if (currBlock->nextBlock != -1)
                        {
                            prevBlockID = currBlockID;
                            currBlockID = currBlock->nextBlock;
                        }
                        else
                        {
                            entryIndex = 0;
                            break;
                        } // if - else
                    } // while
                } // if

                int j;
                for (j = entryIndex ; j < entries ; j++)
                    if (currBlock->rec[j] == NULL)
                        break;

                block->rec[i] = currBlock->rec[j-1];

                // if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
                //     BF_PrintError("Error writing block back");
                //     return -1;
                // }

                currBlock->rec[j-1] = NULL;

                /*
                 * If moved entry was the only one in the block, means that
                 * it is now empty and should be removed only if it is not
                 * the first block.
                 */
                if (j == 1 && blockIndex != 0)
                {
                    if (prevBlockID == blockID)
                        block->nextBlock = -1;
                    else
                    {
                        Block* tmpBlock;

                        if (BF_ReadBlock(header_info.fileDesc , prevBlockID , (void **)&tmpBlock) < 0) {
                            BF_PrintError("Error getting block");
                            return -1;
                        }

                        tmpBlock->nextBlock = -1;

                        if (BF_WriteBlock(header_info.fileDesc , prevBlockID) < 0) {
                            BF_PrintError("Error writing block back");
                            return -1;
                        }
                    }
                    free(currBlock->rec);
                }

                if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
                    BF_PrintError("Error writing block back");
                    return -1;
                }

                return 0;

            } // if
        } // for

        blockID = block->nextBlock;
    } // for

    return -1;  /* As soon as we reach this point, means that the requested entry doesn't exist in the Table */
}

int HT_GetAllEntries(HT_info header_info, void* value)
{
    Block* block;
    int    entries     = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
    int    numOfBlocks = 0;
    int    blockID;
    int    pkey;

    switch (header_info.attrType)
    {
        case 'c':
            pkey = strtoi((char *)value);
        break;

        case 'i':
            pkey = *(int *)value;
        break;
    }

    blockID = HashFunc(pkey , header_info.numBuckets);

    while(blockID != -1)
    {
        numOfBlocks++;

        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
            if (block->rec[i] != NULL)
                return -1;

            if (block->rec[i]->id == pkey)
            {
                printf("     ID: %d\n", block->rec[i]->id);
                printf("   Name: %s\n", block->rec[i]->name);
                printf("Surname: %s\n", block->rec[i]->surname);
                printf("Address: %s\n", block->rec[i]->address);
            } // if
        } // for

        blockID = block->nextBlock;
    } // while

    return numOfBlocks;
}
