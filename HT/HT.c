/* File: HT.c */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"
#include "../AuxFuncs/auxFuncs.h"

int HashStatistics(char* filename)
{
    HT_info* info;
    FILE* gen_fp;

    gen_fp = fopen(filename,"r");
    if (gen_fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    info = (HT_info *)malloc(sizeof(HT_info));
    if (info == NULL) {
        perror("Cannot allocate memory");
        fclose(gen_fp);
        exit(EXIT_FAILURE);
    }

    HT_info * tmp_info = HT_OpenIndex(filename);

    *info = *tmp_info;






    printf("STATISTICS CLOSING INDEX = %d\n" , HT_CloseIndex(info));

    fclose(gen_fp);
    free(info);
    return 0;
}

int HT_CreateIndex(char* fileName, char attrType, char* attrName, int attrLength, int buckets)
{
    int   fileDesc;
    Info* infoBlock;
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

    if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    // info.fileDesc   = file;
    // info.attrName   = attrName;
    // info.attrLength = attrLength;
    // info.attrType   = attrType;
    // info.numBuckets = buckets;

    // infoBlock->info->attrName   = attrName;

    infoBlock->sec_info = NULL;
    infoBlock->info     = (HT_info *)malloc(sizeof(HT_info));

    if (infoBlock->info == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
    		BF_PrintError("Error closing file");
    	}

        return -1;
    }

    infoBlock->info->fileDesc   = fileDesc;
    infoBlock->info->numBuckets = buckets;
    infoBlock->info->attrLength = attrLength;
    infoBlock->info->attrType   = attrType;
    infoBlock->info->attrName   = (char *)malloc((attrLength + 1) * sizeof(char));

    if (infoBlock->info->attrName == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
    		BF_PrintError("Error closing file");
    	}

        free(infoBlock->info);

        return -1;
    }

    strcpy(infoBlock->info->attrName,attrName);

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

    free(infoBlock->info);

    return 0;
}

HT_info* HT_OpenIndex(char* fileName)
{
    Info* infoBlock;
    int fileDesc;

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

    return infoBlock->info;
}

int HT_CloseIndex(HT_info* header_info)
{
    // int temp = header_info->fileDesc;
    // free(header_info->attrName);
    if (BF_CloseFile(header_info->fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

    free(header_info->attrName);

    return 0;
}

int HT_InsertEntry(HT_info header_info, Record record)
{
    Block* block;
    int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
    int    blockID;
    unsigned int pkey;

    switch (header_info.attrType)
    {
        case 'c':
                if (!strcmp(header_info.attrName , "Name"))    pkey = strtoi(record.name);
           else if (!strcmp(header_info.attrName , "Surname")) pkey = strtoi(record.surname);
           else if (!strcmp(header_info.attrName , "Address")) pkey = strtoi(record.address);
        break;

        case 'i':
            pkey = record.id;
        break;

        default:
        return -1;
    }

    blockID = HashFunc(pkey, header_info.numBuckets) + 1;
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

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "Name"))    { if (!strcmp(block->rec[i]->name    , record.name))     return -1; }
                    else if (!strcmp(header_info.attrName , "Surname")) { if (!strcmp(block->rec[i]->surname , record.surname))  return -1; }
                    else if (!strcmp(header_info.attrName , "Address")) { if (!strcmp(block->rec[i]->address , record.address))  return -1; }
                break;

                case 'i':
                    if (block->rec[i]->id == pkey)
                        return -1;
                break;
            }

            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");

            // if (block->rec[i]->id == record.id)
            // {
            //     // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
            //     return -1;
            // } // if

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

int HT_DeleteEntry(HT_info header_info, void* value)
{
    Block* block;
    int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
    int    blockID;
    unsigned int pkey;

    switch (header_info.attrType)
    {
        case 'c':
            pkey = strtoi((char *)value);
        break;

        case 'i':
            pkey = *(int *)value;
        break;

        default:
        return -1;
    }

    blockID = HashFunc(pkey , header_info.numBuckets) + 1;

    for (int blockIndex = 0 ; blockID != -1 ; blockIndex++)
    {
        bool foundEntry = false;

        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
            if (block->rec[i] == NULL)
                return -1;

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "Name"))    { if (!strcmp(block->rec[i]->name    , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Surname")) { if (!strcmp(block->rec[i]->surname , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Address")) { if (!strcmp(block->rec[i]->address , (char *)value))  foundEntry = true; }
                break;

                case 'i':
                    if (block->rec[i]->id == pkey)
                        foundEntry = true;
                break;
            }

            if (foundEntry)
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
    unsigned int pkey;

    // int primFileDesc = header_info.fileDesc;

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
        return -1;
    }
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");

    // printf("INFO FROM GET ALL ENTRIES: FileDesc = %d\n", header_info.fileDesc);
    // printf("INFO FROM GET ALL ENTRIES: AttrType = %c\n", header_info.attrType);
    // printf("INFO FROM GET ALL ENTRIES: AttrName = %s\n", header_info.attrName);
    // printf("INFO FROM GET ALL ENTRIES: AttrLen  = %d\n", header_info.attrLength);
    // printf("INFO FROM GET ALL ENTRIES: Buckets  = %ld\n", header_info.numBuckets);
    // if (BF_ReadBlock(primFileDesc , 0 , (void **)&header_info) < 0) {
    //     // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
    //     BF_PrintError("Error getting block");
    //     return -1;
    // }

    blockID = HashFunc(pkey , header_info.numBuckets) + 1;

    // printf("PKEY = %d\n", pkey);
    // printf("BLOCKID FROM GET ALL ENTRIES = %d\n", blockID);
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");

    while(blockID != -1)
    {
        bool foundEntry = false;
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

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "Name"))    { if (!strcmp(block->rec[i]->name    , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Surname")) { if (!strcmp(block->rec[i]->surname , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Address")) { if (!strcmp(block->rec[i]->address , (char *)value))  foundEntry = true; }
                break;

                case 'i':
                    if (block->rec[i]->id == pkey)
                        foundEntry = true;
                break;
            }

            // if (block->rec[i]->id == pkey)
            if (foundEntry)
            {
                // printf("     ID: %d\n", block->rec[i]->id);
                // printf("   Name: %s\n", block->rec[i]->name);
                // printf("Surname: %s\n", block->rec[i]->surname);
                // printf("Address: %s\n", block->rec[i]->address);

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
