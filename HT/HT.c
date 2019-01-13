/* File: HT.c */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"
#include "../AuxFuncs/auxFuncs.h"

int deletes = 0;

int HT_PrintStats(HT_info info)
{
    Block block;
    void* voidBlock;

    unsigned int totalBlocks               = 0;
    unsigned int bucketsWithOverflowBlocks = 0;
    unsigned int minNumOfEntries           = 0;
    unsigned int minNumOfBlocks            = 0;
    unsigned int maxNumOfEntries           = 0;
    unsigned int maxNumOfBlocks            = 0;
    unsigned int overflowBlocks [info.numBuckets];
    unsigned int bucketEntries  [info.numBuckets];

    // int entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
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

            if (BF_ReadBlock(info.fileDesc , blockID , &voidBlock) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }

            memcpy((void *)&block , voidBlock , sizeof(Block));


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
                if (block.rec[j].name[0] == '\0')
                    break;

                bucketEntries[i-1]++;
                // printf("NEW BUCKET ENTRIES = %u\n", bucketEntries[i-1]);

            } // for

            blockID = block.nextBlock;
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

    // HT_info* tmp_info = HT_OpenIndex(filename);
    info = HT_OpenIndex(filename);
    if (info != NULL)
    {
        // info = *tmp_info;
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

    // SHT_info* tmp_sinfo = SHT_OpenSecondaryIndex(filename);
    sinfo = SHT_OpenSecondaryIndex(filename);
    if (sinfo != NULL)
    {
        // sinfo = *tmp_sinfo;
        SHT_PrintStats(*sinfo);

        SHT_CloseSecondaryIndex(sinfo);

        //
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
    Info infoBlock;
    void* voidBlock;

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

    if (BF_ReadBlock(fileDesc , 0 , &voidBlock) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    memset((void *)&infoBlock, 0 , sizeof(Info));

    memcpy(&infoBlock , voidBlock , sizeof(Info));

    // info.fileDesc   = file;
    // info.attrName   = attrName;
    // info.attrLength = attrLength;
    // info.attrType   = attrType;
    // info.numBuckets = buckets;

    // infoBlock->info->attrName   = attrName;

    infoBlock.hash_type = 0;

    infoBlock.info.ht_info.fileDesc   = -1;
    infoBlock.info.ht_info.numBuckets = buckets;
    infoBlock.info.ht_info.attrLength = attrLength;
    infoBlock.info.ht_info.attrType   = attrType;
    infoBlock.info.ht_info.attrName   = attrName;

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
    //
    // strcpy(infoBlock->info.attrName,attrName);

    //copy content to the block
	// memcpy(block, &info, sizeof(HT_info));

    memcpy(voidBlock , (void*)&infoBlock , sizeof(Info));

    if (BF_WriteBlock(fileDesc , 0 /*BF_GetBlockCounter(fileDesc) - 1*/) < 0) {
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

// HT_info* HT_OpenIndex(char* fileName)
// {
//     Info* infoBlock;
//     int fileDesc;

//     if ((fileDesc = BF_OpenFile(fileName)) < 0) {
// 		BF_PrintError("Error opening file");
// 		return NULL;
// 	}

//     if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
// 		BF_PrintError("Error getting block");
// 		return NULL;
// 	}

// 	if (infoBlock->hashFlag != 0)
//     {
//         if (BF_CloseFile(fileDesc) < 0) {
//             BF_PrintError("Error closing file");
//         }

//         return NULL;
//     }

//     infoBlock->info.fileDesc = fileDesc;
//     printf("INFOBLOCK = %d\n",infoBlock->info.fileDesc );

//     return &(infoBlock->info);
// }

// int HT_CloseIndex(HT_info* header_info)
// {
//     // int temp = header_info->fileDesc;
//     // free(header_info->attrName);
//     if (BF_CloseFile(header_info->fileDesc) < 0) {
// 		BF_PrintError("Error closing file");
// 		return -1;
// 	}

//     free(header_info->attrName);

//     header_info->attrName = NULL;

//     return 0;
// }

HT_info* HT_OpenIndex(char* fileName)
{
    void*    voidBlock;
    HT_info* info;

    int      fileDesc;
    Info     infoBlock;

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , &voidBlock) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

    memcpy(&infoBlock , voidBlock , sizeof(Info));

	if (infoBlock.hash_type != 0)
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

    infoBlock.info.ht_info.fileDesc = fileDesc;

    memcpy(info, &infoBlock.info.ht_info, sizeof(infoBlock.info.ht_info));

    // infoBlock->info.fileDesc = fileDesc;
    printf("INFOBLOCK = %d\n",info->fileDesc);

    return info;
}

int HT_CloseIndex(HT_info* header_info)
{
    if (BF_CloseFile(header_info->fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

    free(header_info);
    return 0;
}

int HT_InsertEntry(HT_info header_info, Record record)
{
    Block block;
    void* voidBlock;
    // int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
    int    entries = MAX_PRIM_RECS;
    int    blockID;
    unsigned int pkey;

    // printf("REC = %d\n", record.id);
    // printf("INFO: ATTRTYPE = %c\n", header_info.attrType);
    switch (header_info.attrType)
    {
        case 'c':
                if (!strcmp(header_info.attrName , "name"))    pkey = strtoi(record.name);
           else if (!strcmp(header_info.attrName , "surname")) pkey = strtoi(record.surname);
           else if (!strcmp(header_info.attrName , "address")) pkey = strtoi(record.address);
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
    int  i;
    bool entryExists = true;

    // printf("ATTR TYPE FROM INSERT ENTRY = %c\n", header_info.attrType);

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    while(1)
    {
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
        if (BF_ReadBlock(header_info.fileDesc , blockID , &voidBlock) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        memset((void *)&block, 0 , sizeof(Block));

        memcpy(&block , voidBlock , sizeof(Block));

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING PRIMARY INDEX FROM INSERT BEFORE INSERTION\n");
        // DispayPrimaryIndex("primary.index");
        // // getchar();

        for (i = 0 ; i < entries ; i++)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");
            if (block.rec[i].name[0] == '\0')
            {
                // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
                entryExists = false;
                break;
            }

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "name"))    { if (!strcmp(block.rec[i].name    , record.name))    return -1; }
                    else if (!strcmp(header_info.attrName , "surname")) { if (!strcmp(block.rec[i].surname , record.surname)) return -1; }
                    else if (!strcmp(header_info.attrName , "address")) { if (!strcmp(block.rec[i].address , record.address)) return -1; }
                break;

                case 'i':
                    if (block.rec[i].id == pkey)
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

        if (block.nextBlock != -1)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
            blockID = block.nextBlock;
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

        blockID = HTBlockInit(header_info.fileDesc);
        // printf("D: %d\n", BF_GetBlockCounter(header_info.fileDesc) - 1);
        // sleep(1);
        block.nextBlock = blockID;
        // printf("BLOCKID = %d\n",blockID);

        memcpy(voidBlock , (void*)&block , sizeof(Block));

    	if (BF_WriteBlock(header_info.fileDesc , old_blockID) < 0) {
            BF_PrintError("Error writing block back");
            return -1;
        }
    }

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 17\n");
    if (BF_ReadBlock(header_info.fileDesc , blockID , &voidBlock) < 0) {
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 18\n");
        BF_PrintError("Error getting block");
        return -1;
    }

    memset((void *)&block, 0 , sizeof(Block));

    memcpy(&block , voidBlock , sizeof(Block));

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 19\n");

    int index = i % entries;
    //
    // block->rec[index] = (Record *)malloc(sizeof(Record));
    // if (block->rec[index] == NULL) {
	// 	perror("Cannot allocate memory");
	// 	return -1;
	// }

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 20\n");

    block.rec[index].id = record.id;
    strcpy(block.rec[index].name    , record.name);
    strcpy(block.rec[index].surname , record.surname);
    strcpy(block.rec[index].address , record.address);

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! PRIMARY INSERTION\n");
    printf("     ID: %d\n", block.rec[index].id);
    printf("   Name: %s\n", block.rec[index].name);
    printf("Surname: %s\n", block.rec[index].surname);
    printf("Address: %s\n\n", block.rec[index].address);

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 21\n");
    // if (BF_WriteBlock(header_info.fileDesc , BF_GetBlockCounter(header_info.fileDesc) - 1) < 0) {

    memcpy(voidBlock , (void *)&block , sizeof(Block));

    if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }
    // printf("BLOCKID = %d\n",blockID);
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 22\n");

    // printf("ATTR TYPE FROM INSERT ENTRY = %c\n", header_info.attrType);

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING PRIMARY INDEX FROM INSERT AFTER INSERTION\n");
    // DispayPrimaryIndex("primary.index");
    // getchar();

    return blockID;

    // // block->nextBlock = HTBlockInit(header_info.fileDesc);
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
    Block block;
    void* voidBlock;
    // int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
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
    int prevBlockID = blockID;

    // printf("\033[1;33m!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING PRIMARY INDEX FROM DELETE FOR DEBUGGING\033[0m\n");
    // DispayPrimaryIndex("primary.index");
    // // getchar();

    for (int blockIndex = 0 ; blockID != -1 ; blockIndex++)
    {
        bool foundEntry = false;

        if (BF_ReadBlock(header_info.fileDesc , blockID , &voidBlock) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        memset((void *)&block, 0 , sizeof(Block));

        memcpy(&block , voidBlock , sizeof(Block));


        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING PRIMARY INDEX FROM DELETE BEFORE DELETION\n");
        // DispayPrimaryIndex("primary.index");
        // // getchar();

        for (int i = 0 ; i < entries ; i++)
        {
            if (block.rec[i].name[0] == '\0')
                return -1;

            printf("Record Test ID: %d\n" , block.rec[i].id);


            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "name"))    { if (!strcmp(block.rec[i].name    , (char *)value)) foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "surname")) { if (!strcmp(block.rec[i].surname , (char *)value)) foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "address")) { if (!strcmp(block.rec[i].address , (char *)value)) foundEntry = true; }
                break;

                case 'i':
                    if (block.rec[i].id == pkey)
                        foundEntry = true;
                break;
            }

            if (foundEntry)
            {

                {
                    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! INSERTING TO 1st FILE\n");
                    int newBlockID = HashFunc(pkey , header_info.numBuckets) + 1;

                    bool leave = false;
                    FILE* tmp_fp;
                    tmp_fp = fopen("res1.txt", "w");

                    while (newBlockID != -1)
                    {
                        Block tmp_block;
                        void* voidBlock;

                        if (BF_ReadBlock(header_info.fileDesc , newBlockID , &voidBlock) < 0) {
                            BF_PrintError("Error getting block");
                            fclose(tmp_fp);
                            return -1;
                        }

                        memcpy(&tmp_block , voidBlock , sizeof(Block));


                        fprintf(tmp_fp,"\nBLOCK ID = %d\n", newBlockID);

                        for (int i = 0 ; i < entries ; i++)
                        {
                            if (tmp_block.rec[i].name[0] == '\0')
                            {
                                leave = true;
                                break;
                            }
                            fprintf(tmp_fp,"PRINTING ID = %d\n", tmp_block.rec[i].id);
                        }

                        if (leave)
                            break;

                        newBlockID = tmp_block.nextBlock;
                    }
                    fclose(tmp_fp);
                    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! INSERTING TO 1st FILE\n");
                }

                    printf("Rec ID : %d\n",block.rec[i].id);
                Block currBlock;

                memset((void *)&currBlock, 0 , sizeof(Block));

                memcpy((void *)&currBlock , (void *)&block , sizeof(Block));

                // void* voidBlock;
                // int    currBlockID = block->nextBlock;
                int    currBlockID = blockID;
                // int    prevBlockID = blockID;
                int    entryIndex  = i + 1;
                // int    currBlockID = blockID;

                // free(block->rec[i]);
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! PRIMARY DELETEION\n");
                printf("     ID: %d\n", block.rec[i].id);
                printf("   Name: %s\n", block.rec[i].name);
                printf("Surname: %s\n", block.rec[i].surname);
                printf("Address: %s\n\n", block.rec[i].address);
                // getchar();

                block.rec[i].name[0] = '\0';

                // if (block->nextBlock == -1 && blockIndex != 0)
                // {
                //
                // }

                /*
                 * We first check if there is only one block, which we
                 * shouldn't delete and then, we search for the last block.
                 */
                if (currBlock.nextBlock != -1)
                {
                    prevBlockID = currBlockID;
                    currBlockID = currBlock.nextBlock;

                    while(1)
                    {
                        if (BF_ReadBlock(header_info.fileDesc , currBlockID , &voidBlock) < 0) {
                            BF_PrintError("Error getting block");
                            return -1;
                        }

                        memset((void *)&currBlock, 0 , sizeof(Block));

                        memcpy(&currBlock , voidBlock , sizeof(Block));

                        if (currBlock.nextBlock != -1)
                        {
                            prevBlockID = currBlockID;
                            currBlockID = currBlock.nextBlock;
                        }
                        else
                        {
                            entryIndex = 0;
                            break;
                        } // if - else
                    } // while
                } // if

                int j;
                printf("ENTRYINDEX    = %d\n", entryIndex);
                printf("CURR BLOCK ID = %d\n", currBlockID);
                for (j = entryIndex ; j < entries ; j++)
                {
                    printf("J = %d\n", j);
                    if (currBlock.rec[j].name[0] == '\0')
                        break;
                    printf("CURR BLOCK[j] = %d\n", currBlock.rec[j].id);

                }

                if (j-1 < 0) {
                        /* code */
                    printf("CURRBLOCK = %d\n", j-1);
                    sleep(2);
                }
                // if (currBlock == block)
                // {

                    memcpy((void *)&block.rec[i] , (void *)&currBlock.rec[j-1] , sizeof(Record));


                    // block.rec[i] = currBlock.rec[j-1];

                    // if (block != currBlock)
                    // {
                    //     block->rec[i].id = currBlock->rec[j-1].id;
                    //     strcpy(block->rec[i].name    , currBlock->rec[j-1].name);
                    //     strcpy(block->rec[i].surname , currBlock->rec[j-1].surname);
                    //     strcpy(block->rec[i].address , currBlock->rec[j-1].address);
                    // }

                    // if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
                    //     BF_PrintError("Error writing block back");
                    //     return -1;
                    // }

                    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! PRIMARY DELETEION MOVED RECORD\n");
                    printf("     ID: %d\n", currBlock.rec[j-1].id);
                    printf("   Name: %s\n", currBlock.rec[j-1].name);
                    printf("Surname: %s\n", currBlock.rec[j-1].surname);
                    printf("Address: %s\n\n", currBlock.rec[j-1].address);
                    // getchar();

                    currBlock.rec[j-1].name[0] = '\0';
                // }
                // else
                // {
                //
                // }
                /////////////////////////////////////////////// if (BF_WriteBlock(header_info.fileDesc , currBlockID) < 0) {
                ///////////////////////////////////////////////     BF_PrintError("Error writing block back");
                ///////////////////////////////////////////////     return -1;
                /////////////////////////////////////////////// }


                /*
                 * If moved entry was the only one in the block, means that
                 * it is now empty and should be removed only if it is not
                 * the first block.
                 */
                 printf("BLOCKINDEX = %d\n", blockIndex);
                 printf("BLOCKID    = %d\n", blockID);
                 printf("PREVBLOCK  = %d\n", prevBlockID);
                if (j == 1 && prevBlockID != currBlockID)
                {
                    Block tmpBlock;
                    void* voidBlock;

                    if (BF_ReadBlock(header_info.fileDesc , prevBlockID , &voidBlock) < 0) {
                        BF_PrintError("Error getting block");
                        return -1;
                    }

                    memset((void *)&tmpBlock, 0 , sizeof(Block));

                    memcpy(&tmpBlock , voidBlock , sizeof(Block));

                    tmpBlock.nextBlock = -1;

                    memcpy(voidBlock , (void *)&tmpBlock , sizeof(Block));

                    if (BF_WriteBlock(header_info.fileDesc , prevBlockID) < 0) {
                        BF_PrintError("Error writing block back");
                        return -1;
                    }

                    printf("PREVBLOCK = %d\n", prevBlockID);
                    deletes++;
                    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DELETES    = %d\n", deletes);
                    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DELETED ID = %d\n", currBlockID);

                    // free(currBlock->rec);
                }
                else if (prevBlockID == currBlockID)
                    block.nextBlock = -1;

                memcpy(voidBlock , (void *)&currBlock , sizeof(Block));

                if (BF_WriteBlock(header_info.fileDesc , currBlockID) < 0) {
                    BF_PrintError("Error writing block back");
                    return -1;
                }

                memcpy(voidBlock , (void *)&block , sizeof(Block));

                if (BF_WriteBlock(header_info.fileDesc , blockID) < 0) {
                    BF_PrintError("Error writing block back");
                    return -1;
                }

                {
                    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! INSERTING TO 2nd FILE\n");

                    int newBlockID = HashFunc(pkey , header_info.numBuckets) + 1;

                    bool leave = false;
                    FILE* tmp_fp;
                    tmp_fp = fopen("res2.txt", "w");

                    while (newBlockID != -1)
                    {
                        Block tmp_block;
                        void* voidBlock;

                        if (BF_ReadBlock(header_info.fileDesc , newBlockID , &voidBlock) < 0) {
                            BF_PrintError("Error getting block");
                            fclose(tmp_fp);
                            return -1;
                        }

                        memcpy(&tmp_block , voidBlock , sizeof(Block));


                        fprintf(tmp_fp,"\nBLOCK ID = %d\n", newBlockID);

                        for (int i = 0 ; i < entries ; i++)
                        {
                            if (tmp_block.rec[i].name[0] == '\0')
                            {
                                leave = true;
                                break;
                            }
                            fprintf(tmp_fp,"PRINTING ID = %d\n", tmp_block.rec[i].id);
                        }

                        if (leave)
                            break;

                        newBlockID = tmp_block.nextBlock;
                    }
                    fclose(tmp_fp);
                    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! INSERTING TO 2nd FILE\n");
                }

                // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING PRIMARY INDEX FROM DELETE AFTER DELETION\n");
                // DispayPrimaryIndex("primary.index");
                // // getchar();

                return 0;

            } // if
        } // for

        prevBlockID = blockID;

        blockID = block.nextBlock;
    } // for

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING PRIMARY INDEX FROM DELETE AFTER DELETION\n");
    // DispayPrimaryIndex("primary.index");
    // // getchar();

    return -1;  /* As soon as we reach this point, avgs that the requested entry doesn't exist in the Table */
}

int HT_GetAllEntries(HT_info header_info, void* value)
{
    Block block;
    void* voidBlock;
    // int    entries     = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
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
            pkey = *(unsigned int *)value;
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

        if (BF_ReadBlock(header_info.fileDesc , blockID , &voidBlock) < 0) {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
            BF_PrintError("Error getting block");
            return -1;
        }

        memcpy(&block , voidBlock , sizeof(Block));

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");

        for (int i = 0 ; i < entries ; i++)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
            if (block.rec[i].name[0] == '\0')
                return -1;
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");

            switch (header_info.attrType)
            {
                case 'c':
                         if (!strcmp(header_info.attrName , "name"))    { if (!strcmp(block.rec[i].name    , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "surname")) { if (!strcmp(block.rec[i].surname , (char *)value))  foundEntry = true; }
                    else if (!strcmp(header_info.attrName , "address")) { if (!strcmp(block.rec[i].address , (char *)value))  foundEntry = true; }
                break;

                case 'i':
                    if (block.rec[i].id == pkey)
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
        blockID = block.nextBlock;
    } // while

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 13\n");
    return -1;
}
