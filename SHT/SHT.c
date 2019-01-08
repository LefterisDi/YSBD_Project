/* File: SHT.c */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SHT.h"
#include "../BF/BF.h"
#include "../AuxFuncs/auxFuncs.h"

int SHT_PrintStats(SHT_info sec_info)
{
    SecondaryBlock* sblock;
    int    entries  = (BLOCK_SIZE - sizeof(SecondaryBlock)) / sizeof(SecondaryRecord);

    unsigned int totalBlocks               = 0;
    unsigned int bucketsWithOverflowBlocks = 0;
    unsigned int minNumOfEntries           = 0;
    unsigned int minNumOfBlocks            = 0;
    unsigned int maxNumOfEntries           = 0;
    unsigned int maxNumOfBlocks            = 0;
    unsigned int overflowBuckets [sec_info.numBuckets];
    unsigned int bucketEntries   [sec_info.numBuckets];

    for (int i = 0; i < sec_info.numBuckets; i++)
    {
        overflowBuckets[i] = 0;
        bucketEntries[i] = 0;
    }

    for (int i = 1 ; i <= sec_info.numBuckets ; i++)
    {
        // unsigned int totalEntries = 0;
        int blockID = i;

        while (blockID != -1)
        {
            totalBlocks++;

            if (BF_ReadBlock(sec_info.sfileDesc , blockID , (void **)&sblock) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }

            if (blockID == i)
			{
                if (sblock->nextBlock == -1)
                    bucketsWithOverflowBlocks++;
			}
            else
                overflowBuckets[i-1]++;

            for (int j = 0 ; j < entries ; j++)
            {
                if (sblock->rec[j] == NULL)
                    break;

                bucketEntries[i-1]++;
            } // for

            blockID = sblock->nextBlock;
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

    unsigned int totalEntries = 0;

    for (int i = 0; i < sec_info.numBuckets; i++)
    {
        if (bucketEntries[i] < minNumOfEntries)
            minNumOfEntries = bucketEntries[i];

        if (bucketEntries[i] > maxNumOfEntries)
            maxNumOfEntries = bucketEntries[i];

        totalEntries += bucketEntries[i];
    }

    double avgNumOfEntries = (double)totalEntries / sec_info.numBuckets;


    minNumOfBlocks = maxNumOfBlocks = overflowBuckets[0];

    for (int i = 0; i < sec_info.numBuckets; i++)
    {
        if (overflowBuckets[i] < minNumOfBlocks)
            minNumOfBlocks = overflowBuckets[i];

        if (overflowBuckets[i] > maxNumOfBlocks)
            maxNumOfBlocks = overflowBuckets[i];
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

    printf("│     Blocks      │         │         │         │\n");
    printf("├─────────────────┼─────────┴─────────┴─────────┤\n");
    printf("│  Total Blocks   │             %-16u│\n",totalBlocks);
    printf("└─────────────────┴─────────────────────────────┘\n");

    return 0;
}

int SHT_CreateSecondaryIndex(char* sfileName , char* attrName , int attrLength , int buckets , char* primFileName)
{
    int   fileDesc;
    Info* infoBlock;

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

    if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

	infoBlock->hashFlag = 1;

	infoBlock->sec_info.sfileDesc  = -1;
	infoBlock->sec_info.numBuckets = buckets;
	infoBlock->sec_info.attrLength = attrLength;
	infoBlock->sec_info.fileName   = (char *)malloc((sizeof(primFileName) + 1) * sizeof(char));

	if (infoBlock->sec_info.fileName == NULL) {
		perror("Cannot allocate memory");

		if (BF_CloseFile(fileDesc) < 0) {
			BF_PrintError("Error closing file");
		}

		return -1;
	}

	infoBlock->sec_info.attrName = (char *)malloc((attrLength + 1) * sizeof(char));

	if (infoBlock->sec_info.attrName == NULL) {
		perror("Cannot allocate memory");

		if (BF_CloseFile(fileDesc) < 0) {
			BF_PrintError("Error closing file");
		}

		free(infoBlock->sec_info.fileName);

		return -1;
	}

	strcpy(infoBlock->sec_info.fileName,primFileName);
	strcpy(infoBlock->sec_info.attrName,attrName);

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

	return 0;
}


SHT_info* SHT_OpenSecondaryIndex(char* sfileName)
{
    Info* infoBlock;
    int fileDesc;

    if ((fileDesc = BF_OpenFile(sfileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

	if (infoBlock->hashFlag != 1)
	{
		if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

		return NULL;
	}


    infoBlock->sec_info.sfileDesc = fileDesc;
    printf("S_INFOBLOCK = %d\n",infoBlock->info.fileDesc );

    return &(infoBlock->sec_info);
}


int SHT_CloseSecondaryIndex(SHT_info* header_info)
{
    if (BF_CloseFile(header_info->sfileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

	free(header_info->fileName);
	free(header_info->attrName);

	header_info->fileName = NULL;
	header_info->attrName = NULL;

    return 0;
}

int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord secRec)
{
    SecondaryBlock* sblock;
    int entries = (BLOCK_SIZE - sizeof(SecondaryBlock)) / sizeof(SecondaryRecord);
	unsigned int pkey = 0;
	// printf("PKEY = %d\n", pkey);

	     // if (!strcmp(header_info.attrName , "Id"))		pkey = secRec.record.id;
	 	 if (!strcmp(header_info.attrName , "Name"))    pkey = strtoi(secRec.record.name);
	else if (!strcmp(header_info.attrName , "Surname")) pkey = strtoi(secRec.record.surname);
	else if (!strcmp(header_info.attrName , "Address")) pkey = strtoi(secRec.record.address);

	// printf("PKEY = %u\n", pkey);
	// printf("BUCKETS BEFORE = %ld\n", header_info.numBuckets);
	int blockID = HashFunc(pkey, header_info.numBuckets) + 1;
	// printf("BLOCKID = %d\n",blockID);

	// blockID++;
	// printf("BUCKETS AFTER = %ld\n", header_info.numBuckets);

	// printf("PKEY = %u\n", pkey);
    // printf("ENTRIES = %d\n",entries);
    // printf("REC ID = %d\n", secRec.record.id);
    // printf("REC NAME = %s\n", secRec.record.name);
    // printf("REC SURNAME = %s\n", secRec.record.surname);
    // printf("REC ADDRESS = %s\n", secRec.record.address);
    // printf("BLOCKID = %d\n",blockID);
    int    i;
    bool   availablePos = false;

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");

    while(1)
    {
		// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
        if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }
		// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");

        for (i = 0 ; i < entries ; i++)
        {
			// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");
            if (sblock->rec[i] == NULL)
            {
				// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
                availablePos = true;
                break;
            }
			// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");
        }

		// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
		if (availablePos)
			break;
			// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");

        if (sblock->nextBlock != -1)
        {
			// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
            blockID = sblock->nextBlock;
            // printf("BLOCKID 1 = %d\n",blockID);
			// sleep(0.5);
        }
        else
        {
			// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");
            break;
        }
		// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
    } // while

	// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
    // printf("BLOCKID 2 = %d\n",blockID);
	// sleep(0.5);

    // printf("I : %d\n" , i);
    if (i == entries)
    {
		int old_blockID = blockID;
		// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 13\n");
        blockID = SHTBlockInit(header_info.sfileDesc);
		// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 14\n");

		// printf("D: %d\n", BF_GetBlockCounter(header_info.sfileDesc) - 1);

        sblock->nextBlock = blockID;

        // printf("BLOCKID 3 = %d\n",blockID);

		if (BF_WriteBlock(header_info.sfileDesc , old_blockID) < 0) {
	        BF_PrintError("Error writing block back");
	        return -1;
	    }

		// sleep(0.5);
    }

	// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 15\n");
    if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
        BF_PrintError("Error getting block");
        return -1;
    }

	// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 16\n");
    int index = i % entries;

    sblock->rec[index] = (SecondaryRecord *)malloc(sizeof(SecondaryRecord));
	if (sblock->rec[index] == NULL) {
		perror("Cannot allocate memory");
		return -1;
	}
	// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 17\n");

	// printf("SECONDARY ID = %d\n", secRec.record.id);
	// printf("SECONDARY NAME = %s\n", secRec.record.name);
	// printf("SECONDARY SURNAME = %s\n", secRec.record.surname);
	// printf("SECONDARY ADDRESS = %s\n", secRec.record.address);

    sblock->rec[index]->blockId   = secRec.blockId;
    sblock->rec[index]->record.id = secRec.record.id;

    strcpy(sblock->rec[index]->record.name    , secRec.record.name);
    strcpy(sblock->rec[index]->record.surname , secRec.record.surname);
    strcpy(sblock->rec[index]->record.address , secRec.record.address);

	// printf("SECONDARY 2 ID = %d\n", sblock->rec[index]->record.id);
	// printf("SECONDARY 2 NAME = %s\n", sblock->rec[index]->record.name);
	// printf("SECONDARY 2 SURNAME = %s\n", sblock->rec[index]->record.surname);
	// printf("SECONDARY 2 ADDRESS = %s\n", sblock->rec[index]->record.address);
	// sleep(1);
	// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 18\n");

    if (BF_WriteBlock(header_info.sfileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

	// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 19\n");
    // printf("BLOCKID = %d\n",blockID);

    return blockID;
}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value)
{
    SecondaryBlock* sblock;
    int    entries     		   = (BLOCK_SIZE - sizeof(SecondaryBlock)) / sizeof(SecondaryRecord);
    int    totalSearchedBlocks = 0;
    // int    currSearchedBlocks  = 0;
	bool   foundEntry  		   = false;
	unsigned int pkey  		   = 0;

	// int secFileDesc = header_info_sht.sfileDesc;

		 if (!strcmp(header_info_sht.attrName , "Name"))    pkey = strtoi((char *)value);
	else if (!strcmp(header_info_sht.attrName , "Surname")) pkey = strtoi((char *)value);
	else if (!strcmp(header_info_sht.attrName , "Address")) pkey = strtoi((char *)value);

	// if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info_sht) < 0) {
	// 	BF_PrintError("Error getting block");
	// 	return -1;
	// }

	int blockID = HashFunc(pkey, header_info_sht.numBuckets) + 1;

	// printf("VALUE = %s\n", (char *)value);
	// printf("BLOCKID = %d\n", blockID);




    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");

    while(blockID != -1)
    {
        // currSearchedBlocks++;
        totalSearchedBlocks++;

		// if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info_sht) < 0) {
		//     BF_PrintError("Error getting block");
		//     return -1;
		// }
		//
        // if (BF_ReadBlock(header_info_sht.sfileDesc , blockID , (void **)&sblock) < 0) {
        //     BF_PrintError("Error getting block");
        //     return -1;
        // }

        for (int i = 0 ; i < entries ; i++)
        {
			bool displayEntry = false;

			// if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info_sht) < 0) {
			// 	BF_PrintError("Error getting block");
			// 	return -1;
			// }

			if (BF_ReadBlock(header_info_sht.sfileDesc , blockID , (void **)&sblock) < 0) {
				BF_PrintError("Error getting block");
				return -1;
			}

            if (sblock->rec[i] == NULL)
			{
				if (foundEntry)
                	return totalSearchedBlocks;
				else
                	return -1;
			}

				 if (!strcmp(header_info_sht.attrName , "Name"))    { if (!strcmp(sblock->rec[i]->record.name    , (char *)value))  displayEntry = true; }
			else if (!strcmp(header_info_sht.attrName , "Surname")) { if (!strcmp(sblock->rec[i]->record.surname , (char *)value))  displayEntry = true; }
			else if (!strcmp(header_info_sht.attrName , "Address")) { if (!strcmp(sblock->rec[i]->record.address , (char *)value))  displayEntry = true; }

            if (displayEntry)
            {
				foundEntry = true;

				// totalSearchedBlocks += currSearchedBlocks;
				// currSearchedBlocks = 0;

                printf("     ID: %d\n", sblock->rec[i]->record.id);
                printf("   Name: %s\n", sblock->rec[i]->record.name);
                printf("Surname: %s\n", sblock->rec[i]->record.surname);
                printf("Address: %s\n\n", sblock->rec[i]->record.address);
				// sleep(1);
            }
        } // for

        blockID = sblock->nextBlock;
		// printf("BLOCKID INSIDE = %d\n", blockID);
    } // while

    return totalSearchedBlocks;
}
