/* File: SHT.c */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SHT.h"
#include "../HT/HT.h"
#include "../BF/BF.h"
#include "../AuxFuncs/auxFuncs.h"

extern int FILEDESC;

int SHT_PrintStats(SHT_info sec_info)
{
    SecondaryBlock* sblock;

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
        int blockID = i;

        while (blockID != -1)
        {
            totalBlocks++;

            if (BF_ReadBlock(sec_info.sfileDesc , blockID , (void **)&sblock) < 0) {
                BF_PrintError("Error getting block");
                return -1;
            }

			if (blockID != i)
				overflowBlocks[i-1]++;

            for (int j = 0 ; j < entries ; j++)
            {
                if (sblock->rec[j].secHashKey[0] == '\0')
                    break;

                bucketEntries[i-1]++;
            } // for

            blockID = sblock->nextBlock;
        } // while
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

	infoBlock->hash_type = 1;

	infoBlock->info.sht_info.sfileDesc  = -1;
	infoBlock->info.sht_info.numBuckets = buckets;
	infoBlock->info.sht_info.attrLength = attrLength;
    infoBlock->info.sht_info.attrName   = (char *)(infoBlock + 1);
    infoBlock->info.sht_info.fileName   = (char *)(infoBlock + 1) + strlen(attrName) + 1;
    strcpy(infoBlock->info.sht_info.attrName , attrName);
    strcpy(infoBlock->info.sht_info.fileName , primFileName);

    if (BF_WriteBlock(fileDesc , 0) < 0) {
		BF_PrintError("Error writing block back");
		return -1;
	}

    for (int i = 1 ; i <= buckets ; i++) {
		SHTBlockInit(fileDesc);
	}

    /* Synchronization of the two Hashing Indexes */
    void*    blockptr;
    Block    block;
    HT_info* ht_info;
    Info     info;
    int      entries = MAX_PRIM_RECS;

    SecondaryRecord secRec;
    SHT_info        sec_info = infoBlock->info.sht_info;

    sec_info.attrName = (char *)malloc((sec_info.attrLength + 1) * sizeof(char));
    if (sec_info.attrName == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        return -1;
    }

    sec_info.fileName = (char *)malloc((strlen(primFileName) + 1) * sizeof(char));
    if (sec_info.fileName == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        free(sec_info.attrName);
        return -1;
    }

    strcpy(sec_info.attrName , infoBlock->info.sht_info.attrName);
    strcpy(sec_info.fileName , infoBlock->info.sht_info.fileName);

    sec_info.sfileDesc = fileDesc;

    bool openedPrimary = false;

    if (FILEDESC == -1)
    {
        openedPrimary = true;

        ht_info = HT_OpenIndex(primFileName);

        info.info.ht_info = *ht_info;
    }
    else
    {
        if (BF_ReadBlock(FILEDESC , 0 , &blockptr) < 0) {
            BF_PrintError("Error getting block");

            free(sec_info.attrName);
            free(sec_info.fileName);
            return -1;
        }

        info = *(Info *)blockptr;
    }

    for (int i = 1 ; i <= info.info.ht_info.numBuckets ; i++)
    {
        int blockID = i;

        while (blockID != -1)
        {
            if (BF_ReadBlock(FILEDESC , blockID , &blockptr) < 0) {
                BF_PrintError("Error getting block");

                free(sec_info.attrName);
                free(sec_info.fileName);
                return -1;
            }

            block = *(Block *)blockptr;

            secRec.blockId = blockID;

            for (int j = 0 ; j < entries ; j++)
            {
                if (block.rec[j].id == -1)
                    break;

                secRec.record = block.rec[j];

                SHT_SecondaryInsertEntry(sec_info , secRec);
            } // for

            blockID = block.nextBlock;
        } // while
    } // for

    free(sec_info.attrName);
    free(sec_info.fileName);

    if (openedPrimary)
        HT_CloseIndex(ht_info);

    if (BF_CloseFile(fileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

	return 0;
}

SHT_info* SHT_OpenSecondaryIndex(char* sfileName)
{
    Info*     infoBlock;
    SHT_info* sinfo;
    int       fileDesc;

    if ((fileDesc = BF_OpenFile(sfileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&infoBlock) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

	if (infoBlock->hash_type != 1)
	{
		if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

		return NULL;
	}

    infoBlock->info.sht_info.attrName = (char *)(infoBlock + 1);
    infoBlock->info.sht_info.fileName   = (char *)(infoBlock + 1) + strlen(infoBlock->info.sht_info.attrName) + 1;

    sinfo = (SHT_info *)malloc(sizeof(SHT_info));
    if (sinfo == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        return NULL;
    }

    memcpy(sinfo, &infoBlock->info.sht_info, sizeof(SHT_info));

    sinfo->attrName = (char *)malloc((sinfo->attrLength + 1) * sizeof(char));
    if (sinfo->attrName == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        free(sinfo);
        return NULL;
    }

    sinfo->fileName = (char *)malloc((strlen(infoBlock->info.sht_info.fileName) + 1) * sizeof(char));
    if (sinfo->attrName == NULL) {
        perror("Cannot allocate memory");

        if (BF_CloseFile(fileDesc) < 0) {
            BF_PrintError("Error closing file");
        }

        free(sinfo);
        return NULL;
    }

    sinfo->sfileDesc = fileDesc;
    strcpy(sinfo->attrName , infoBlock->info.sht_info.attrName);
    strcpy(sinfo->fileName , infoBlock->info.sht_info.fileName);

    return sinfo;
}


int SHT_CloseSecondaryIndex(SHT_info* header_info)
{
    if (BF_CloseFile(header_info->sfileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

	free(header_info->attrName);
	free(header_info->fileName);
	free(header_info);

    return 0;
}

int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord secRec)
{
    SecondaryBlock* sblock;
    int entries = MAX_SEC_RECS;
	unsigned int pkey = 0;

	 	 if (!strcmp(header_info.attrName , "name"))    pkey = strtoi(secRec.record.name);
	else if (!strcmp(header_info.attrName , "surname")) pkey = strtoi(secRec.record.surname);
	else if (!strcmp(header_info.attrName , "address")) pkey = strtoi(secRec.record.address);

	int blockID = HashFunc(pkey, header_info.numBuckets) + 1;

    int    i;
    bool   availablePos = false;

    while(1)
    {
        if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (i = 0 ; i < entries ; i++)
        {
            if (sblock->rec[i].secHashKey[0] == '\0')
            {
                availablePos = true;
                break;
            }
        }

		if (availablePos)
			break;

        if (sblock->nextBlock != -1)
            blockID = sblock->nextBlock;
        else
            break;
    } // while

    if (i == entries)
    {
		int old_blockID = blockID;

        blockID = SHTBlockInit(header_info.sfileDesc);

        sblock->nextBlock = blockID;

		if (BF_WriteBlock(header_info.sfileDesc , old_blockID) < 0) {
	        BF_PrintError("Error writing block back");
	        return -1;
	    }
    }

    if (BF_ReadBlock(header_info.sfileDesc , blockID , (void **)&sblock) < 0) {
        BF_PrintError("Error getting block");
        return -1;
    }

    int index = i % entries;

    sblock->rec[index].blockId   = secRec.blockId;
    sblock->rec[index].id = secRec.record.id;

         if (!strcmp(header_info.attrName , "name"))    strcpy(sblock->rec[index].secHashKey , secRec.record.name);
    else if (!strcmp(header_info.attrName , "surname")) strcpy(sblock->rec[index].secHashKey , secRec.record.surname);
    else if (!strcmp(header_info.attrName , "address")) strcpy(sblock->rec[index].secHashKey , secRec.record.address);

    if (BF_WriteBlock(header_info.sfileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return blockID;
}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value)
{
    SecondaryBlock* sblock;
    int entries     		   = MAX_SEC_RECS;
    int totalSearchedBlocks    = 0;
	int foundEntry  		   = 0;
	unsigned int pkey;

    pkey = strtoi((char *)value);

	int blockID = HashFunc(pkey, header_info_sht.numBuckets) + 1;

    while(blockID != -1)
    {
        totalSearchedBlocks++;

        if (BF_ReadBlock(header_info_sht.sfileDesc , blockID , (void **)&sblock) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int i = 0 ; i < entries ; i++)
        {
			bool displayEntry = false;

            if (sblock->rec[i].secHashKey[0] == '\0')
			{
				if (foundEntry)
                	return totalSearchedBlocks;
				else
                	return -1;
			}

            if (!strcmp(sblock->rec[i].secHashKey , (char *)value))
                displayEntry = true;

            if (displayEntry)
            {
                Block* block;

                int primEntries   = MAX_PRIM_RECS;

                if (BF_ReadBlock(header_info_ht.fileDesc , sblock->rec[i].blockId , (void **)&block) < 0) {
                    BF_PrintError("Error getting block");
                    return -1;
                }

                for (int j = 0; j < primEntries; j++)
                {
                    if (block->rec[j].id == -1)
                        break;

                    if (block->rec[j].id == sblock->rec[i].id)
                    {
                        foundEntry++;
                        printf("     ID: %d\n",   block->rec[j].id);
                        printf("   Name: %s\n",   block->rec[j].name);
                        printf("Surname: %s\n\n", block->rec[j].surname);
                        printf("Address: %s\n\n", block->rec[j].address);
                        break;
                    }
                }
            } // if
        } // for

        blockID = sblock->nextBlock;
    } // while

    if (foundEntry)
        return totalSearchedBlocks;
    else
        return -1;
}
