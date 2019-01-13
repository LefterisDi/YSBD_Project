/* File: SHT.c */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SHT.h"
#include "../HT/HT.h"
#include "../BF/BF.h"
#include "../AuxFuncs/auxFuncs.h"

int SHT_PrintStats(SHT_info sec_info)
{
    void*          tmpBlock;
    SecondaryBlock sblock;

	unsigned int totalEntries 			   = 0;
    unsigned int totalBlocks  			   = 0;
	unsigned int bucketsWithOverflowBlocks = 0;
	unsigned int minNumOfEntries 		   = 0;
    unsigned int minNumOfBlocks  		   = 0;
	unsigned int maxNumOfEntries 		   = 0;
    unsigned int maxNumOfBlocks  		   = 0;
	unsigned int bucketEntries  [sec_info.numBuckets];
	unsigned int overflowBlocks [sec_info.numBuckets];

	int entries = MAX_SEC_RECS;

	for (int i = 0; i < sec_info.numBuckets; i++)
    {
		bucketEntries [i] = 0;
        overflowBlocks[i] = 0;
    }

    for (int i = 1 ; i <= sec_info.numBuckets ; i++)
    {
        // unsigned int totalEntries = 0;
        int blockID = i;

        while (blockID != -1)
        {
            totalBlocks++;

            if (BF_ReadBlock(sec_info.sfileDesc , blockID , &tmpBlock) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }

            memcpy((void *)&sblock, tmpBlock, sizeof(SecondaryBlock));

            // if (blockID == i)
			// {
            //     if (sblock.nextBlock == -1)
            //         bucketsWithOverflowBlocks++;
			// }
            // else
            //     overflowBlocks[i-1]++;
			if (blockID != i)
				overflowBlocks[i-1]++;

            for (int j = 0 ; j < entries ; j++)
            {
                if (sblock.rec[j].secHashKey[0] == '\0')
                    break;

                bucketEntries[i-1]++;
            } // for

            blockID = sblock.nextBlock;
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

    for (int i = 0; i < sec_info.numBuckets; i++)
    {
        if (bucketEntries[i] < minNumOfEntries)
            minNumOfEntries = bucketEntries[i];

        if (bucketEntries[i] > maxNumOfEntries)
            maxNumOfEntries = bucketEntries[i];

        totalEntries += bucketEntries[i];
    }

    double avgNumOfEntries = (double)totalEntries / sec_info.numBuckets;

	// unsigned int bucketsWithOverflowBlocks = 0;

    minNumOfBlocks = maxNumOfBlocks = overflowBlocks[0];

    for (int i = 0; i < sec_info.numBuckets; i++)
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
	double avgNumOfBlocks = (double)totalBlocks / sec_info.numBuckets - 1.0;

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

	for (int i = 1; i <= sec_info.numBuckets; i++)
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

int SHT_CreateSecondaryIndex(char* sfileName , char* attrName , int attrLength , int buckets , char* primFileName)
{
    void* info;
    int   sfileDesc;
    Info  infoBlock;

	if (BF_CreateFile(sfileName) < 0) {
		BF_PrintError("Error creating file");
		return -1;
	}

    if ((sfileDesc = BF_OpenFile(sfileName)) < 0) {
		BF_PrintError("Error opening file");
		return -1;
	}

    if (BF_AllocateBlock(sfileDesc) < 0) {
		BF_PrintError("Error allocating block");
		return -1;
	}

    if (BF_ReadBlock(sfileDesc , 0 , &info) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    memset((void *)&infoBlock, 0 , sizeof(Info));

    memcpy(&infoBlock, info , sizeof(Info));

	infoBlock.hash_type = 1;

	infoBlock.info.sht_info.sfileDesc  = -1;
	infoBlock.info.sht_info.numBuckets = buckets;
    infoBlock.info.sht_info.attrName   = attrName;
	infoBlock.info.sht_info.attrLength = attrLength;
    infoBlock.info.sht_info.fileName   = primFileName;


	// infoBlock.sec_info.fileName   = (char *)malloc((sizeof(primFileName) + 1) * sizeof(char));
    //
	// if (infoBlock.sec_info.fileName == NULL) {
	// 	perror("Cannot allocate memory");
    //
	// 	if (BF_CloseFile(sfileDesc) < 0) {
	// 		BF_PrintError("Error closing file");
	// 	}
    //
	// 	return -1;
	// }
    //
	// infoBlock.sec_info.attrName = (char *)malloc((attrLength + 1) * sizeof(char));
    //
	// if (infoBlock.sec_info.attrName == NULL) {
	// 	perror("Cannot allocate memory");
    //
	// 	if (BF_CloseFile(sfileDesc) < 0) {
	// 		BF_PrintError("Error closing file");
	// 	}
    //
	// 	free(infoBlock.sec_info.fileName);
    //
	// 	return -1;
	// }

	// strcpy(infoBlock.sec_info.fileName,primFileName);
	// strcpy(infoBlock.sec_info.attrName,attrName);

    if (BF_WriteBlock(sfileDesc , 0 ) < 0) {
		BF_PrintError("Error writing block back");
		return -1;
	}

    for (int i = 1 ; i <= buckets ; i++) {
		SHTBlockInit(sfileDesc);
	}


    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING PRIMARY INDEX FROM INSERT BEFORE INSERTION\n");
    // DispayPrimaryIndex("primary.index");
    // getchar();
    // getchar();
    // getchar();
    // getchar();
    // getchar();
    /* Synchronization of the two Hashing Indexes */
    HT_info* prim_info = HT_OpenIndex(primFileName);
    if (prim_info == NULL) {
        printf("Error: Not a Primary Hashing File\n");
        return -1;
    }

    void* tmpBlock;
    Block block;

    int   entries = MAX_PRIM_RECS;

    SecondaryRecord secRec;
    // SHT_info        sec_info = infoBlock.info.sht_info;

    infoBlock.info.sht_info.sfileDesc = sfileDesc;

    for (int i = 1 ; i <= prim_info->numBuckets ; i++)
    {
        int blockID = i;

        while (blockID != -1)
        {
            if (BF_ReadBlock(prim_info->fileDesc , blockID , &tmpBlock) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }

            memset((void *)&block, 0 , sizeof(Block));

            memcpy(&block, tmpBlock , sizeof(Block));

            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING SECONDARY INDEX FROM CREATE BEFORE SYNCH\n");
        	// DispaySecondaryIndex("secondary.index");
        	// // getchar();

            printf("!!!!!!!!!!!!!! PRINTING BLOCKID FROM SEC CREATE = %d\n", blockID);
            // getchar();
            secRec.blockId = blockID;

            for (int j = 0 ; j < entries ; j++)
            {
                printf("\n!!!!!!!!!!!!!!!!!!!!!!!! RECORD FROM PRIMARY TO BE INSERTED AND SYNCHRONIZED\n");
                printf("     ID: %d\n", block.rec[j].id);
                printf("   Name: %s\n", block.rec[j].name);
                printf("Surname: %s\n", block.rec[j].surname);
                printf("Address: %s\n", block.rec[j].address);
                getchar();
                getchar();
                getchar();
                getchar();
                getchar();
                // printf("\n!!!!!!!!!!!!!!!!!!!!!!!! RECORD FROM PRIMARY TO BE INSERTED AND SYNCHRONIZED\n");
                // printf("     FD: %d\n", infoBlock.sec_info.sfileDesc);
                // printf("   Name: %s\n", infoBlock.sec_info.attrName);
                // printf(" Length: %d\n", infoBlock.sec_info.attrLength);
                // printf("NumBuck: %ld\n", infoBlock.sec_info.numBuckets);
                // printf("FileNam: %s\n", infoBlock.sec_info.fileName);
                // getchar();

                if (block.rec[j].name[0] == '\0')
                    break;

                secRec.record.id  = block.rec[j].id;
                strcpy(secRec.record.name    , block.rec[j].name);
                strcpy(secRec.record.surname , block.rec[j].surname);
                strcpy(secRec.record.address , block.rec[j].address);

                printf("!!!!!!!!!!!!!!!!!!!!!!!! RECORD TO BE INSERTED AND SYNCHRONIZED\n");
                printf("     ID: %d\n", secRec.record.id);
                printf("   Name: %s\n", secRec.record.name);
                printf("Surname: %s\n", secRec.record.surname);
                printf("Address: %s\n", secRec.record.address);
                printf("!!!!!!!!!!!!!!!!!!!!!!!! RECORD TO BE INSERTED AND SYNCHRONIZED\n");
                printf("     FD: %d\n", infoBlock.info.sht_info.sfileDesc);
                printf("   Name: %s\n", infoBlock.info.sht_info.attrName);
                printf(" Length: %d\n", infoBlock.info.sht_info.attrLength);
                printf("NumBuck: %ld\n", infoBlock.info.sht_info.numBuckets);
                printf("FileNam: %s\n", infoBlock.info.sht_info.fileName);
                // getchar();

                SHT_SecondaryInsertEntry(infoBlock.info.sht_info , secRec);
            } // for

            blockID = block.nextBlock;
        } // while
    } // for

    HT_CloseIndex(prim_info);

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BEFORE DISPLAYING SECONDARY INDEX FROM CREATE AFTER SYNCH\n");
    // DispaySecondaryIndex("secondary.index");
    // // getchar();


    if (BF_CloseFile(sfileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

	return 0;
}

SHT_info* SHT_OpenSecondaryIndex(char* sfileName)
{
    void*     generalInfo;
    SHT_info* sinfo;

    int       sfileDesc;
    Info      infoBlock;

    if ((sfileDesc = BF_OpenFile(sfileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(sfileDesc , 0 , &generalInfo) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

    memcpy(&infoBlock, generalInfo , sizeof(Info));

	if (infoBlock.hash_type != 1)
	{
		if (BF_CloseFile(sfileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

		return NULL;
	}

    sinfo = (SHT_info *)malloc(sizeof(SHT_info));
    if (sinfo == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(sfileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        return NULL;
    }

    infoBlock.info.sht_info.sfileDesc = sfileDesc;
    // sinfo->sfileDesc  = infoBlock.sec_info.sfileDesc;
    // sinfo->attrLength = infoBlock.sec_info.attrLength;
    // sinfo->attrName   = infoBlock.sec_info.attrName;
    // sinfo->fileName   = infoBlock.sec_info.fileName;
    // sinfo->numBuckets = infoBlock.sec_info.numBuckets;
    memcpy(sinfo, &infoBlock.info.sht_info, sizeof(infoBlock.info.sht_info));

    // printf("SEC_INFO: %d\n", infoBlock->sec_info.sfileDesc);
    // printf("SEC_INFO: %d\n", infoBlock->sec_info.attrLength);
    // printf("SEC_INFO: %s\n", infoBlock->sec_info.attrName);
    // printf("SEC_INFO: %ld\n", infoBlock->sec_info.numBuckets);
    // printf("SEC_INFO: %s\n", infoBlock->sec_info.fileName);

    // printf("\nSINFO: %d\n", sinfo->sfileDesc);
    // printf("SINFO: %d\n", sinfo->attrLength);
    // printf("SINFO: %s\n", sinfo->attrName);
    // printf("SINFO: %ld\n", sinfo->numBuckets);
    // printf("SINFO: %s\n", sinfo->fileName);
    // infoBlock->sec_info.sfileDesc = fileDesc;

    return sinfo;
}

int SHT_CloseSecondaryIndex(SHT_info* header_info)
{
    if (BF_CloseFile(header_info->sfileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

	free(header_info);
    return 0;
}
