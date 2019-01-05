/* File: SHT.c */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "SHT.h"
#include "../BF/BF.h"

int SHTBlockInit(const int fileDesc)
{
	SecondaryBlock* block;
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

    block->nextBlock = -1;

	int entries = (BLOCK_SIZE - sizeof(SecondaryBlock)) / sizeof(SecondaryRecord);

    block->rec = (SecondaryRecord **)malloc(entries * sizeof(SecondaryRecord *));
	if (block->rec == NULL) {
		perror("Cannot allocate memory");
		return -1;
	}

    for (int i = 0 ; i < entries ; i++)
        block->rec[i] = NULL;

    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return blockID;
}

int SHT_CreateSecondaryIndex(char* sfileName , char* attrName , int attrLength , int buckets , char* primaryFileName)
{
    int       fileDesc;
    SHT_info* block;

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

    if (BF_ReadBlock(fileDesc , 0 , (void **)&block) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    block->sfileDesc  = fileDesc;
    block->attrName   = attrName;
    block->attrLength = attrLength;
    block->numBuckets = buckets;
    block->fileName   = primaryFileName;

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
    SHT_info* info;
    int fileDesc;

    if ((fileDesc = BF_OpenFile(sfileName)) < 0) {
		BF_PrintError("Error opening file");
		return NULL;
	}

    if (BF_ReadBlock(fileDesc , 0 , (void **)&info) < 0) {
		BF_PrintError("Error getting block");
		return NULL;
	}

    return info;
}


int SHT_CloseSecondaryIndex(SHT_info* header_info)
{
    if (BF_CloseFile(header_info->sfileDesc) < 0) {
		BF_PrintError("Error closing file");
		return -1;
	}

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

int SHT_GetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value)
{
    SecondaryBlock* sblock;
    int    entries     		   = (BLOCK_SIZE - sizeof(SecondaryBlock)) / sizeof(SecondaryRecord);
    int    totalSearchedBlocks = 0;
    int    currSearchedBlocks  = 0;
	bool   foundEntry  		   = false;
	unsigned int pkey  		   = 0;

	int secFileDesc = header_info_sht.sfileDesc;

		 if (!strcmp(header_info_sht.attrName , "Name"))    pkey = strtoi((char *)value);
	else if (!strcmp(header_info_sht.attrName , "Surname")) pkey = strtoi((char *)value);
	else if (!strcmp(header_info_sht.attrName , "Address")) pkey = strtoi((char *)value);

	if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info_sht) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

	int blockID = HashFunc(pkey, header_info_sht.numBuckets) + 1;

	printf("VALUE = %s\n", (char *)value);
	printf("BLOCKID = %d\n", blockID);




    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");

    while(blockID != -1)
    {
        currSearchedBlocks++;

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

			if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info_sht) < 0) {
				BF_PrintError("Error getting block");
				return -1;
			}

			if (BF_ReadBlock(secFileDesc , blockID , (void **)&sblock) < 0) {
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

				totalSearchedBlocks += currSearchedBlocks;
				currSearchedBlocks = 0;

                printf("     ID: %d\n", sblock->rec[i]->record.id);
                printf("   Name: %s\n", sblock->rec[i]->record.name);
                printf("Surname: %s\n", sblock->rec[i]->record.surname);
                printf("Address: %s\n\n", sblock->rec[i]->record.address);
				// sleep(1);
            }
        } // for

        blockID = sblock->nextBlock;
		printf("BLOCKID INSIDE = %d\n", blockID);
    } // while

    return totalSearchedBlocks;
}

int SHTBlockDelete(SHT_info* header_info)
{
    SecondaryBlock* block;
    int    entries = (BLOCK_SIZE - sizeof(SecondaryBlock)) / sizeof(SecondaryRecord);
	int cntr = 0;
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
	int secFileDesc = header_info->sfileDesc;

	if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    for (int i = 0; i < header_info->numBuckets; i++)
    {

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
        int blockID = i + 1;

        while (blockID != -1)
        {
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
            if (BF_ReadBlock(secFileDesc , blockID , (void **)&block) < 0) {
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

				cntr++;
                free(block->rec[j]);
                // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
            } // for

            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");
            free(block->rec);
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");

            if (BF_WriteBlock(secFileDesc , blockID) < 0) {
                BF_PrintError("Error writing block back");
                return -1;
            }
            // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");

            blockID = block->nextBlock;
        } // while

		if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info) < 0) {
		    BF_PrintError("Error getting block");
		    return -1;
	    }

        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
    } // for

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
	// printf("CNTR = %d\n", cntr);
    return 0;
}
