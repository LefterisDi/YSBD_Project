/* File: HT.c */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"
#include "../AuxFuncs/auxFuncs.h"

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
        // unsigned int totalEntries = 0;
        int blockID = i;

        while (blockID != -1)
        {
            totalBlocks++;
            //
            // printf("\nINFO: %d\n", info.fileDesc);
            // printf("INFO: %s\n", info.attrName);
            // printf("INFO: %c\n", info.attrType);
            // printf("INFO: %d\n", info.attrLength);
            // printf("INFO: %ld\n", info.numBuckets);
            // printf("INFO: %d\n", blockID);

            if (BF_ReadBlock(info.fileDesc , blockID , (void **)&block) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }

            // if (blockID == i)
            // {
            //     if (block->nextBlock == -1)
            //         bucketsWithOverflowBlocks++;
            // }
            // else
            //     overflowBlocks[i-1]++;
            if (blockID != i)
                overflowBlocks[i-1]++;

            for (int j = 0 ; j < entries ; j++)
            {
                if (block->rec[j].name[0] == '\0')
                    break;

                bucketEntries[i-1]++;
                // printf("NEW BUCKET ENTRIES = %u\n", bucketEntries[i-1]);

            } // for

            blockID = block->nextBlock;
        } // while

        //
        // if (!minNumOfEntries || currEntries < minNumOfEntries)
        //     minNumOfEntries = currEntries;
        //
        // if (currEntries > maxNumOfEntries)
        //     maxNumOfEntries = currEntries;

        // bucketEntries[i] = totalEntries;

    } // for

    minNumOfEntries = maxNumOfEntries = bucketEntries[0];
    // printf("BUCKET ENTRIES = %u\n", bucketEntries[0]);

    unsigned int totalEntries = 0;

    for (int i = 0 ; i < info.numBuckets ; i++)
    {
        // printf("BUCKET ENTRIES = %u\n", bucketEntries[i]);
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

    // if (avgNumOfEntries < 10)
    //     printf("  %.2f  │\n", avgNumOfEntries);
    // else if (avgNumOfEntries < 100)
    //     printf("  %.2f │\n", avgNumOfEntries);
    // else if (avgNumOfEntries < 1000)
    //     printf(" %.2f │\n", avgNumOfEntries);
    // else if (avgNumOfEntries < 10000)
    //     printf(" %.2f│\n", avgNumOfEntries);
    // else
    //     printf("%.2f│\n", avgNumOfEntries);

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
    //
    // if (avgNumOfBlocks < 10)
    //     printf("  %.2f  │\n", avgNumOfBlocks);
    // else if (avgNumOfBlocks < 100)
    //     printf("  %.2f │\n", avgNumOfBlocks);
    // else if (avgNumOfBlocks < 1000)
    //     printf(" %.2f │\n", avgNumOfBlocks);
    // else if (avgNumOfBlocks < 10000)
    //     printf(" %.2f│\n", avgNumOfBlocks);
    // else
    //     printf("%.2f│\n", avgNumOfBlocks);

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
        printf("FILEDESC = %d\n", info->fileDesc);
        HT_PrintStats(*info);
        printf("FILEDESC = %d\n", info->fileDesc);

        HT_CloseIndex(info);

        // if (BF_CloseFile(info->fileDesc) < 0) {
    	// 	BF_PrintError("Error closing file");
    	// 	return -1;
    	// }

        return 0;
    }

    sinfo = SHT_OpenSecondaryIndex(filename);
    if (sinfo != NULL)
    {
        SHT_PrintStats(*sinfo);

        SHT_CloseSecondaryIndex(sinfo);

        // if (BF_CloseFile(sinfo->sfileDesc) < 0) {
    	// 	BF_PrintError("Error closing file");
    	// 	return -1;
    	// }

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

    infoBlock->hashFlag = 0;

    infoBlock->info.fileDesc   = -1;
    infoBlock->info.numBuckets = buckets;
    infoBlock->info.attrLength = attrLength;
    infoBlock->info.attrType   = attrType;
    infoBlock->info.attrName   = attrName;
    // infoBlock->info.attrName   = (char *)malloc((attrLength + 1) * sizeof(char));
    //
    // if (infoBlock->info.attrName == NULL) {
    //     perror("Cannot allocate memory");
    //
    //     if (BF_CloseFile(fileDesc) < 0) {
    // 		BF_PrintError("Error closing file");
    // 	}
    //
    //     return -1;
    // }

    // strcpy(infoBlock->info.attrName,attrName);

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

    printf("FROM HT_CREATE INDEX ATTRTYPE = %c\n", infoBlock->info.attrType);

    return 0;
}

HT_info* HT_OpenIndex(char* fileName)
{
    Info*    infoBlock;
    HT_info* info;
    int      fileDesc;

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

	if (infoBlock->hashFlag != 0)
    {
        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        return NULL;
    }

    info = (HT_info *)malloc(sizeof(HT_info));
    if (info == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        return NULL;
    }

    memcpy(info, &infoBlock->info, sizeof(infoBlock->info));

    // infoBlock->info.fileDesc = fileDesc;
    info->fileDesc = fileDesc;
    printf("INFOBLOCK = %d\n",info->fileDesc);

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

    free(header_info);

    // header_info->attrName = NULL;

    return 0;
}

int HT_InsertEntry(HT_info header_info, Record record)
{
    Block* block;
    int    entries = MAX_PRIM_RECS;
    int    blockID;
    unsigned int pkey;

    // printf("REC = %d\n", record.id);
    printf("INFO: ATTRTYPE = %c\n", header_info.attrType);
    switch (header_info.attrType)
    {
        case 'c':
                if (!strcmp(header_info.attrName , "Name"))    pkey = strtoi(record.name);
           else if (!strcmp(header_info.attrName , "Surname")) pkey = strtoi(record.surname);
           else if (!strcmp(header_info.attrName , "Address")) pkey = strtoi(record.address);
        break;

        case 'i':
            // printf("INTEGER SELECTED\n");
            pkey = record.id;
        break;

        default:
            printf("PRINTING FROM DEFAULT\n");
            sleep(1);
        return -1;
    }

    // printf("PKEY = %u\n", pkey);

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
            if (block->rec[i].name[0] == '\0')
            {
                // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
                entryExists = false;
                break;
            }

            switch (header_info.attrType)
            {
                case 'c':
                    if (!strcmp(header_info.attrName , "Name"))
                    {
                        if (!strcmp(block->rec[i].name    , record.name))
                            return -1;
                    }
                    else if (!strcmp(header_info.attrName , "Surname"))
                    {
                        if (!strcmp(block->rec[i].surname , record.surname))
                            return -1;
                    }
                    else if (!strcmp(header_info.attrName , "Address"))
                    {
                        if (!strcmp(block->rec[i].address , record.address))
                            return -1;
                    }
                break;

                case 'i':
                    if (block->rec[i].id == pkey)
                        return -1;
                break;
            }

            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");

            // if (block->rec[i].id == record.id)
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

    // block->rec[index] = (Record *)malloc(sizeof(Record));
    // if (block->rec[index] == NULL) {
	// 	perror("Cannot allocate memory");
	// 	return -1;
	// }

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 20\n");

    block->rec[index] = record;
    // block->rec[index].id = record.id;
    // strcpy(block->rec[index].name    , record.name);
    // strcpy(block->rec[index].surname , record.surname);
    // strcpy(block->rec[index].address , record.address);

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

    for (int blockIndex = 0 ; blockID != -1 ; blockIndex++)
    {
        bool foundEntry = false;

        if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
            if (block->rec[i].name[0] == '\0')
                return -1;

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "Name"))    { if (!strcmp(block->rec[i].name    , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Surname")) { if (!strcmp(block->rec[i].surname , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Address")) { if (!strcmp(block->rec[i].address , (char *)value))  foundEntry = true; }
                break;

                case 'i':
                    if (block->rec[i].id == pkey)
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

                // free(block->rec[i]);
                // block->rec[i] = NULL;
                block->rec[i].name[0] = '\0';

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
                    if (currBlock->rec[j].name[0] == '\0')
                        break;

                block->rec[i] = currBlock->rec[j-1];
                //
                // if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
                //     BF_PrintError("Error writing block back");
                //     return -1;
                // }

                currBlock->rec[j-1].name[0] = '\0';

                if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
                    BF_PrintError("Error writing block back");
                    return -1;
                }

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
                    // free(currBlock->rec);
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

    return -1;  /* As soon as we reach this point, avgs that the requested entry doesn't exist in the Table */
}

int HT_GetAllEntries(HT_info header_info, void* value)
{
    Block* block;
    int    entries     = MAX_PRIM_RECS;
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
            if (block->rec[i].name[0] == '\0')
                return -1;
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "Name"))    { if (!strcmp(block->rec[i].name    , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Surname")) { if (!strcmp(block->rec[i].surname , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "Address")) { if (!strcmp(block->rec[i].address , (char *)value))  foundEntry = true; }
                break;

                case 'i':
                    if (block->rec[i].id == pkey)
                        foundEntry = true;
                break;
            }

            // if (block->rec[i].id == pkey)
            if (foundEntry)
            {
                // printf("     ID: %d\n", block->rec[i].id);
                // printf("   Name: %s\n", block->rec[i].name);
                // printf("Surname: %s\n", block->rec[i].surname);
                // printf("Address: %s\n", block->rec[i].address);

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
