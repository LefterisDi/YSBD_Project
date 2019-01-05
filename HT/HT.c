/* File: HT.c */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "HT.h"
// #include "HashFuncs.h"
#include "../BF/BF.h"

int HashFunc(const unsigned int id, const int mask)
{
    // printf("HASHFUNC ID = %u\n", id);
    // printf("HASHFUNC MASK = %d\n", mask);
    return id % mask;
}

int sdsgajd = 9;

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

    // printf("BLOCKID FROM BLOCKINIT = %d\n",blockID);

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
    if (block->rec == NULL) {
        perror("Cannot allocate memory");
        return -1;
    }

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
    block->attrName   = attrName;
    // block->attrName   = (char *)malloc(sizeof(attrName));
    // strcpy(block->attrName,attrName);
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

    if (BF_CloseFile(fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

    return 0;
}

HT_info* HT_OpenIndex(char* fileName)
{
    HT_info* info;
    int fileDesc;

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&info) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

    return info;
}

int HT_CloseIndex(HT_info* header_info)
{
    // int temp = header_info->fileDesc;
    // free(header_info->attrName);
    if (BF_CloseFile(header_info->fileDesc) < 0) {
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
    int    blockID = HashFunc(record.id, header_info.numBuckets) + 1;
    // printf("ENTRIES = %d\n",entries);
    // printf("REC ID = %d\n", record.id);
    // printf("BLOCKID = %d\n",blockID);
    int    i;
    bool   entryExists = true;

    // printf("ATTR TYPE FROM INSERT ENTRY = %c\n", header_info.attrType);

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    while(1)
    {
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");

        for (i = 0 ; i < entries ; i++)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");
            if (block->rec[i] == NULL)
            {
                // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
                entryExists = false;
                break;
            }

            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");
            if (block->rec[i]->id == record.id)
            {
                // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
                return -1;
            } // if
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");
        } // for

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
        if (!entryExists)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");
            break;
        }
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");

        if (block->nextBlock != -1)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
            blockID = block->nextBlock;
            // printf("BLOCKID = %d\n",blockID);
        }
        else
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 13\n");
            break;
        }
    } // while
    // printf("BLOCKID = %d\n",blockID);

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 14\n");
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

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 15\n");
    // printf("I : %d\n" , i);
    if (i == entries)
    {
        int old_blockID = blockID;
        // Record* rec;
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 16\n");

        blockID = BlockInit(header_info.fileDesc);
        // printf("D: %d\n", BF_GetBlockCounter(header_info.fileDesc) - 1);
        // sleep(1);
        block->nextBlock = blockID;
        // printf("BLOCKID = %d\n",blockID);

    	if (BF_WriteBlock(header_info.fileDesc , old_blockID) < 0) {
            BF_PrintError("Error writing block back");
            return -1;
        }
    }

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 17\n");
    if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 18\n");
        BF_PrintError("Error getting block");
        return -1;
    }
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 19\n");

    int index = i % entries;

    block->rec[index] = (Record *)malloc(sizeof(Record));
    if (block->rec[index] == NULL) {
		perror("Cannot allocate memory");
		return -1;
	}

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 20\n");

    block->rec[index]->id = record.id;
    strcpy(block->rec[index]->name    , record.name);
    strcpy(block->rec[index]->surname , record.surname);
    strcpy(block->rec[index]->address , record.address);

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 21\n");
    // if (BF_WriteBlock(header_info.fileDesc , BF_GetBlockCounter(header_info.fileDesc) - 1) < 0) {
    if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }
    // printf("BLOCKID = %d\n",blockID);
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 22\n");

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

int BlockDelete(HT_info* header_info)
{
    Block* block;
    int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);

    int primFileDesc = header_info->fileDesc;

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    for (int i = 0; i < header_info->numBuckets; i++)
    {

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
        int blockID = i + 1;

        while (blockID != -1)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
            if (BF_ReadBlock(primFileDesc , blockID , (void **)&block) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");

            for (int j = 0 ; j < entries ; j++)
            {
                // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
                if (block->rec[j] == NULL)
                    break;
                    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");

                free(block->rec[j]);
                // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
            } // for

            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");
            free(block->rec);
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");

            if (BF_WriteBlock(primFileDesc , blockID) < 0) {
                BF_PrintError("Error writing block back");
                return -1;
            }
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");

            blockID = block->nextBlock;
        } // while


        if (BF_ReadBlock(primFileDesc , 0 , (void **)&header_info) < 0) {
		    BF_PrintError("Error getting block");
		    return -1;
	    }


        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
    } // for

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
    return 0;
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

    blockID = HashFunc(pkey , header_info.numBuckets) + 1;

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

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    switch (header_info.attrType)
    {
        case 'c':
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
            pkey = strtoi((char *)value);
            // printf("GIVEN ID STR = %s\n", (char *)value);
        break;

        case 'i':
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
            pkey = *(int *)value;
            // printf("GIVEN ID INT = %d\n", *(int *)value);
        break;

        default:
            // printf("ATRR TYPE = %c\n", header_info.attrType);
        break;
    }
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");

    // printf("INFO FROM GET ALL ENTRIES: FileDesc = %d\n", header_info.fileDesc);
    // printf("INFO FROM GET ALL ENTRIES: AttrType = %c\n", header_info.attrType);
    // printf("INFO FROM GET ALL ENTRIES: AttrName = %s\n", header_info.attrName);
    // printf("INFO FROM GET ALL ENTRIES: AttrLen  = %d\n", header_info.attrLength);
    // printf("INFO FROM GET ALL ENTRIES: Buckets  = %ld\n", header_info.numBuckets);
    blockID = HashFunc(pkey , header_info.numBuckets) + 1;
    // printf("PKEY = %d\n", pkey);
    // printf("BLOCKID FROM GET ALL ENTRIES = %d\n", blockID);
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");

    while(blockID != -1)
    {
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");
        numOfBlocks++;

        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
            BF_PrintError("Error getting block");
            return -1;
        }
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");

        for (int i = 0 ; i < entries ; i++)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
            if (block->rec[i] == NULL)
                return -1;
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");

            if (block->rec[i]->id == pkey)
            {
                printf("     ID: %d\n", block->rec[i]->id);
                printf("   Name: %s\n", block->rec[i]->name);
                printf("Surname: %s\n", block->rec[i]->surname);
                printf("Address: %s\n", block->rec[i]->address);

                return numOfBlocks;
            } // if
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
        } // for

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
        blockID = block->nextBlock;
    } // while

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 13\n");
    return -1;
}
