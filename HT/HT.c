/* File: HT.c */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"
#include "../AuxFuncs/auxFuncs.h"

int FILEDESC = -1;

int HT_PrintStats(HT_info info)
{
    Block* block;

    unsigned int totalBlocks               = 0;
    unsigned int bucketsWithOverflowBlocks = 0;
    unsigned int minNumOfEntries           = 0;
    unsigned int minNumOfBlocks            = 0;
    unsigned int maxNumOfEntries           = 0;
    unsigned int maxNumOfBlocks            = 0;
    unsigned int overflowBlocks [info.numBuckets];
    unsigned int bucketEntries  [info.numBuckets];

    int entries = MAX_PRIM_RECS;

    for (int i = 0 ; i < info.numBuckets ; i++)
    {
        overflowBlocks[i] = 0;
        bucketEntries[i] = 0;
    }

    for (int i = 1 ; i <= info.numBuckets ; i++)
    {
        int blockID = i;

        while (blockID != -1)
        {
            totalBlocks++;

            if (BF_ReadBlock(info.fileDesc , blockID , (void **)&block) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }

            if (blockID != i)
                overflowBlocks[i-1]++;

            for (int j = 0 ; j < entries ; j++)
            {
                if (block->rec[j].id == -1)
                    break;

                bucketEntries[i-1]++;
            } // for

            blockID = block->nextBlock;
        } // while
    } // for

    minNumOfEntries = maxNumOfEntries = bucketEntries[0];

    unsigned int totalEntries = 0;

    for (int i = 0 ; i < info.numBuckets ; i++)
    {
        if (bucketEntries[i] < minNumOfEntries)
            minNumOfEntries = bucketEntries[i];

        if (bucketEntries[i] > maxNumOfEntries)
            maxNumOfEntries = bucketEntries[i];

        totalEntries += bucketEntries[i];
    }

    double avgNumOfEntries = (double)totalEntries / info.numBuckets;

    minNumOfBlocks = maxNumOfBlocks = overflowBlocks[0];

    for (int i = 0 ; i < info.numBuckets ; i++)
    {
        if (overflowBlocks[i] < minNumOfBlocks)
            minNumOfBlocks = overflowBlocks[i];

        if (overflowBlocks[i] > maxNumOfBlocks)
            maxNumOfBlocks = overflowBlocks[i];

		if (overflowBlocks[i] != 0)
			bucketsWithOverflowBlocks++;
    }

	/*
	 * We only count the overflow blocks as 'blocks'
	 * for calculating the average number of blocks
	 * that each bucket has.
	 * --------------------------------------------
	 * totalBlocks - numBuckets     totalBlocks
	 * ────────────────────────  =  ─────────── - 1
	 *        numBuckets            numBuckets
	 */
    double avgNumOfBlocks = (double)totalBlocks / info.numBuckets - 1.0;

    /*
     * ┌─────────────────┬─────────┬─────────┬─────────┐
     * │      Stats      │   Min   │   Max   │ Average │
     * ├─────────────────┼─────────┼─────────┼─────────┤
     * │     Entries     │         │         │         │
     * ├─────────────────┼─────────┼─────────┼─────────┤
     * │     Blocks      │         │         │         │
     * ├─────────────────┼─────────┴─────────┴─────────┤
     * │  Total Blocks   │                             │
     * └─────────────────┴─────────────────────────────┘
     */
    printf("┌─────────────────┬─────────┬─────────┬─────────┐\n");
    printf("│      Stats      │   Min   │   Max   │ Average │\n");
    printf("├─────────────────┼─────────┼─────────┼─────────┤\n");
    printf("│     Entries     │");

    if (minNumOfEntries < 10)
        printf("    %u    │", minNumOfEntries);
    else if (minNumOfEntries < 100)
        printf("   %u    │", minNumOfEntries);
    else if (minNumOfEntries < 1000)
        printf("   %u   │", minNumOfEntries);
    else if (minNumOfEntries < 10000)
        printf("  %u   │", minNumOfEntries);
    else if (minNumOfEntries < 100000)
        printf("  %u  │", minNumOfEntries);
    else if (minNumOfEntries < 1000000)
        printf(" %u  │", minNumOfEntries);
    else if (minNumOfEntries < 10000000)
        printf(" %u │", minNumOfEntries);
    else if (minNumOfEntries < 100000000)
        printf("%u │", minNumOfEntries);
    else
        printf("%u│", minNumOfEntries);

    if (maxNumOfEntries < 10)
        printf("    %u    │", maxNumOfEntries);
    else if (maxNumOfEntries < 100)
        printf("   %u    │", maxNumOfEntries);
    else if (maxNumOfEntries < 1000)
        printf("   %u   │", maxNumOfEntries);
    else if (maxNumOfEntries < 10000)
        printf("  %u   │", maxNumOfEntries);
    else if (maxNumOfEntries < 100000)
        printf("  %u  │", maxNumOfEntries);
    else if (maxNumOfEntries < 1000000)
        printf(" %u  │", maxNumOfEntries);
    else if (maxNumOfEntries < 10000000)
        printf(" %u │", maxNumOfEntries);
    else if (maxNumOfEntries < 100000000)
        printf("%u │", maxNumOfEntries);
    else
        printf("%u│", maxNumOfEntries);


    if (avgNumOfEntries < 10)
        printf("   %.2f  │\n", avgNumOfEntries);
    else if (avgNumOfEntries < 100)
        printf("  %.2f  │\n", avgNumOfEntries);
    else if (avgNumOfEntries < 1000)
        printf("  %.2f │\n", avgNumOfEntries);
    else if (avgNumOfEntries < 10000)
        printf(" %.2f │\n", avgNumOfEntries);
	else if (avgNumOfEntries < 100000)
		printf(" %.2f│\n", avgNumOfEntries);
    else
        printf("%.2f│\n", avgNumOfEntries);


    printf("├─────────────────┼─────────┼─────────┼─────────┤\n");
    printf("│     Blocks      │");

    if (minNumOfBlocks < 10)
        printf("    %u    │", minNumOfBlocks);
    else if (minNumOfBlocks < 100)
        printf("   %u    │", minNumOfBlocks);
    else if (minNumOfBlocks < 1000)
        printf("   %u   │", minNumOfBlocks);
    else if (minNumOfBlocks < 10000)
        printf("  %u   │", minNumOfBlocks);
    else if (minNumOfBlocks < 100000)
        printf("  %u  │", minNumOfBlocks);
    else if (minNumOfBlocks < 1000000)
        printf(" %u  │", minNumOfBlocks);
    else if (minNumOfBlocks < 10000000)
        printf(" %u │", minNumOfBlocks);
    else if (minNumOfBlocks < 100000000)
        printf("%u │", minNumOfBlocks);
    else
        printf("%u│", minNumOfBlocks);

    if (maxNumOfBlocks < 10)
        printf("    %u    │", maxNumOfBlocks);
    else if (maxNumOfBlocks < 100)
        printf("   %u    │", maxNumOfBlocks);
    else if (maxNumOfBlocks < 1000)
        printf("   %u   │", maxNumOfBlocks);
    else if (maxNumOfBlocks < 10000)
        printf("  %u   │", maxNumOfBlocks);
    else if (maxNumOfBlocks < 100000)
        printf("  %u  │", maxNumOfBlocks);
    else if (maxNumOfBlocks < 1000000)
        printf(" %u  │", maxNumOfBlocks);
    else if (maxNumOfBlocks < 10000000)
        printf(" %u │", maxNumOfBlocks);
    else if (maxNumOfBlocks < 100000000)
        printf("%u │", maxNumOfBlocks);
    else
        printf("%u│", maxNumOfBlocks);

    if (avgNumOfBlocks < 10)
        printf("   %.2f  │\n", avgNumOfBlocks);
    else if (avgNumOfBlocks < 100)
        printf("  %.2f  │\n", avgNumOfBlocks);
    else if (avgNumOfBlocks < 1000)
        printf("  %.2f │\n", avgNumOfBlocks);
    else if (avgNumOfBlocks < 10000)
        printf(" %.2f │\n", avgNumOfBlocks);
	else if (avgNumOfBlocks < 100000)
		printf(" %.2f│\n", avgNumOfBlocks);
    else
        printf("%.2f│\n", avgNumOfBlocks);

    printf("├─────────────────┼─────────┴─────────┴─────────┤\n");
    printf("│  Total Entries  │             %-16u│\n",totalEntries);
    printf("├─────────────────┼─────────────────────────────┤\n");
    printf("│  Total Blocks   │             %-16u│\n",totalBlocks);
    printf("└─────────────────┴─────────────────────────────┘\n");

	/*
	 *     ┌───────────────┬─────────────────┐
	 *     │   Bucket ID   │ Overflow Blocks │
	 *     ├───────────────┼─────────────────┤
	 *     │               │                 │
	 *     │               │                 │
	 *     │               │                 │
	 *     │               │                 │
	 *     │               │                 │
	 *     │               │                 │
	 *     │               │                 │
	 *     ├───────────────┼─────────────────┤
 	 *     │ Total Buckets │                 │
 	 *     │ with Overflow │                 │
	 *     │    Blocks     │                 │
	 *     └───────────────┴─────────────────┘
	 */
	printf("       ┌───────────────┬─────────────────┐\n");
	printf("       │   Bucket ID   │ Overflow Blocks │\n");
	printf("       ├───────────────┼─────────────────┤\n");

	for (int i = 1; i <= info.numBuckets; i++)
	{
	    if (i < 10)
	        printf("       │       %d       ", i);
	    else if (i < 100)
	        printf("       │      %d       ", i);
	    else if (i < 1000)
	        printf("       │     %d       ", i);
	    else if (i < 10000)
	        printf("       │    %d       ", i);
	    else if (i < 100000)
	        printf("       │   %d       ", i);
	    else if (i < 1000000)
	        printf("       │  %d       ", i);
	    else if (i < 10000000)
	        printf("       │ %d       ", i);
	    else
	        printf("       │%d       ", i);

	    if (overflowBlocks[i-1] < 10)
			printf("│        %u        │\n", overflowBlocks[i-1]);
	    else if (overflowBlocks[i-1] < 100)
	        printf("│       %u        │\n", overflowBlocks[i-1]);
	    else if (overflowBlocks[i-1] < 1000)
	        printf("│      %u        │\n", overflowBlocks[i-1]);
	    else if (overflowBlocks[i-1] < 10000)
	        printf("│     %u        │\n", overflowBlocks[i-1]);
	    else if (overflowBlocks[i-1] < 100000)
	        printf("│    %u        │\n", overflowBlocks[i-1]);
	    else if (overflowBlocks[i-1] < 1000000)
	        printf("│   %u        │\n", overflowBlocks[i-1]);
	    else if (overflowBlocks[i-1] < 10000000)
	        printf("│  %u        │\n", overflowBlocks[i-1]);
	    else
	        printf("│ %u        │\n", overflowBlocks[i-1]);
	}

	printf("       ├───────────────┼─────────────────┤\n");
	printf("       │ Total Buckets │                 │\n");
	printf("       │ with Overflow │");

	if (bucketsWithOverflowBlocks < 10)
		printf("        %u        │\n", bucketsWithOverflowBlocks);
	else if (bucketsWithOverflowBlocks < 100)
		printf("       %u        │\n", bucketsWithOverflowBlocks);
	else if (bucketsWithOverflowBlocks < 1000)
		printf("       %u       │\n", bucketsWithOverflowBlocks);
	else if (bucketsWithOverflowBlocks < 10000)
		printf("      %u       │\n", bucketsWithOverflowBlocks);
	else if (bucketsWithOverflowBlocks < 100000)
		printf("      %u      │\n", bucketsWithOverflowBlocks);
	else if (bucketsWithOverflowBlocks < 1000000)
		printf("     %u      │\n", bucketsWithOverflowBlocks);
	else if (bucketsWithOverflowBlocks < 10000000)
		printf("     %u     │\n", bucketsWithOverflowBlocks);
	else if (bucketsWithOverflowBlocks < 100000000)
		printf("    %u     │\n", bucketsWithOverflowBlocks);
	else
		printf("    %u    │\n", bucketsWithOverflowBlocks);

	printf("       │    Blocks     │                 │\n");
	printf("       └───────────────┴─────────────────┘\n");

    return 0;
}

int HashStatistics(char* filename)
{
    HT_info*  info;
    SHT_info* sinfo;

    info = HT_OpenIndex(filename);
    if (info != NULL)
    {
        HT_PrintStats(*info);

        HT_CloseIndex(info);

        return 0;
    }

    sinfo = SHT_OpenSecondaryIndex(filename);
    if (sinfo != NULL)
    {
        SHT_PrintStats(*sinfo);

        SHT_CloseSecondaryIndex(sinfo);

        return 0;
    }

    return -1;
}

int HT_CreateIndex(char* fileName, char attrType, char* attrName, int attrLength, int buckets)
{
    int   fileDesc;
    Info* infoBlock;

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

    infoBlock->hash_type = 0;

    infoBlock->info.ht_info.fileDesc   = -1;
    infoBlock->info.ht_info.numBuckets = buckets;
    infoBlock->info.ht_info.attrLength = attrLength;
    infoBlock->info.ht_info.attrType   = attrType;
    infoBlock->info.ht_info.attrName   = (char *)(infoBlock + 1);
    strcpy(infoBlock->info.ht_info.attrName, attrName);

    if (BF_WriteBlock(fileDesc , 0) < 0) {
		BF_PrintError("Error writing block back");
		return -1;
	}

	for (int i = 1 ; i <= buckets ; i++) {
		HTBlockInit(fileDesc/*,i*/);
	}

    if (BF_CloseFile(fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

    return 0;
}

HT_info* HT_OpenIndex(char* fileName)
{
    Info*    infoBlock;
    HT_info* info;
    int      fileDesc;

    if (FILEDESC != -1) {
        printf("File is already opened\n");
        return NULL;
    }

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}


	if (infoBlock->hash_type != 0)
    {
        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        return NULL;
    }

    infoBlock->info.ht_info.attrName = (char *)(infoBlock + 1);

    info = (HT_info *)malloc(sizeof(HT_info));
    if (info == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        return NULL;
    }

    memcpy(info, &infoBlock->info.ht_info, sizeof(HT_info));

    info->attrName = (char *)malloc((info->attrLength + 1) * sizeof(char));
    if (info->attrName == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        free(info);
        return NULL;
    }

    info->fileDesc = fileDesc;
    strcpy(info->attrName , infoBlock->info.ht_info.attrName);

    FILEDESC = fileDesc;

    return info;
}

int HT_CloseIndex(HT_info* header_info)
{
    if (FILEDESC == -1) {
        printf("File is already closed\n");
        return -1;
    }

    FILEDESC = -1;

    if (BF_CloseFile(header_info->fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

    free(header_info->attrName);
    free(header_info);

    return 0;
}

int HT_InsertEntry(HT_info header_info, Record record)
{
    Block* block;
    int    entries = MAX_PRIM_RECS;
    int    blockID;
    unsigned int pkey;

    int insBlockID = -1;
    int pos = -1;

    switch (header_info.attrType)
    {
        case 'c':
                if (!strcmp(header_info.attrName , "name"))    pkey = strtoi(record.name);
           else if (!strcmp(header_info.attrName , "surname")) pkey = strtoi(record.surname);
           else if (!strcmp(header_info.attrName , "address")) pkey = strtoi(record.address);
        break;

        case 'i':
            pkey = record.id;
        break;

        default:
        return -1;
    }

    blockID = HashFunc(pkey, header_info.numBuckets) + 1;

    int prevBlockID = -1;

    while(blockID != -1)
    {
        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
            if (block->rec[i].id == -1)
            {
                if (insBlockID == -1)
                {
                    insBlockID = blockID;
                    pos = i;
                }
                break;
            }

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "name"))    { if (!strcmp(block->rec[i].name    , record.name))    return -1; }
                    else if (!strcmp(header_info.attrName , "surname")) { if (!strcmp(block->rec[i].surname , record.surname)) return -1; }
                    else if (!strcmp(header_info.attrName , "address")) { if (!strcmp(block->rec[i].address , record.address)) return -1; }
                break;

                case 'i':
                    if (block->rec[i].id == pkey)
                        return -1;
                break;
            }
        } // for

        prevBlockID = blockID;
        blockID = block->nextBlock;
    } // while


    if (insBlockID == -1)
    {
        pos = 0;

        insBlockID = HTBlockInit(header_info.fileDesc);
        block->nextBlock = insBlockID;

    	if (BF_WriteBlock(header_info.fileDesc , prevBlockID) < 0) {
            BF_PrintError("Error writing block back");
            return -1;
        }
    }

    if (BF_ReadBlock(header_info.fileDesc , insBlockID , (void **)&block) < 0) {
        BF_PrintError("Error getting block");
        return -1;
    }

    block->rec[pos].id = record.id;
    strcpy(block->rec[pos].name    , record.name);
    strcpy(block->rec[pos].surname , record.surname);
    strcpy(block->rec[pos].address , record.address);

    if (BF_WriteBlock(header_info.fileDesc , insBlockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return insBlockID;
}

int HT_DeleteEntry(HT_info header_info, void* value)
{
    Block* block;
    int    entries = MAX_PRIM_RECS;
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

    while (blockID != -1)
    {
        bool foundEntry = false;

        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
            if (block->rec[i].id == -1)
                break;

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "name"))    { if (!strcmp(block->rec[i].name    , (char *)value)) foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "surname")) { if (!strcmp(block->rec[i].surname , (char *)value)) foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "address")) { if (!strcmp(block->rec[i].address , (char *)value)) foundEntry = true; }
                break;

                case 'i':
                    if (block->rec[i].id == pkey)
                        foundEntry = true;
                break;
            }

            if (foundEntry)
            {
                block->rec[i].id = -1;

                if (i < entries - 1)
                {
                    int j;
                    for (j = i+1; j < entries; j++)
                        if (block->rec[j].id == -1)
                            break;

                    block->rec[i] = block->rec[j-1];

                    block->rec[j-1].id = -1;
                }

                if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
                    BF_PrintError("Error writing block back");
                    return -1;
                }

                return 0;
            } // if
        } // for

        blockID = block->nextBlock;
    } // while

    return -1;  /* As soon as we reach this point, means that the requested entry doesn't exist in the Table */
}

int HT_GetAllEntries(HT_info header_info, void* value)
{
    Block* block;
    int    entries     = MAX_PRIM_RECS;
    int    numOfBlocks = 0;
    int    blockID;
    unsigned int pkey;

    switch (header_info.attrType)
    {
        case 'c':
            pkey = strtoi((char *)value);
        break;

        case 'i':
            pkey = *(unsigned int *)value;
        break;

        default:
        return -1;
    }

    blockID = HashFunc(pkey , header_info.numBuckets) + 1;

    while(blockID != -1)
    {
        bool foundEntry = false;
        numOfBlocks++;

        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
            if (block->rec[i].id == -1)
                break;

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "name"))    { if (!strcmp(block->rec[i].name    , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "surname")) { if (!strcmp(block->rec[i].surname , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "address")) { if (!strcmp(block->rec[i].address , (char *)value))  foundEntry = true; }
                break;

                case 'i':
                    if (block->rec[i].id == pkey)
                        foundEntry = true;
                break;
            }

            if (foundEntry)
            {
                printf("     ID: %d\n", block->rec[i].id);
                printf("   Name: %s\n", block->rec[i].name);
                printf("Surname: %s\n", block->rec[i].surname);
                printf("Address: %s\n", block->rec[i].address);

                return numOfBlocks;
            }
        } // for

        blockID = block->nextBlock;
    } // while

    return -1;
}
