
/* File: auxFuncs.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxFuncs.h"
#include "../SHT/SHT.h"
#include "../HT/HT.h"
#include "../BF/BF.h"

int deletes2;

int HashFunc(const unsigned int id, const int mask)
{
    // printf("HASHFUNC ID = %u\n", id);
    // printf("HASHFUNC MASK = %d\n", mask);
    return id % mask;
}

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

int BlockInit(const int fileDesc)
{
	Block block;
    int blockID;
    void* voidBlock;

    memset((void *)&block, 0 , sizeof(Block));


    if (BF_AllocateBlock(fileDesc) < 0) {
        BF_PrintError("Error allocating block");
        return -1;
    }

    blockID = BF_GetBlockCounter(fileDesc) - 1;

	if (BF_ReadBlock(fileDesc , blockID , &voidBlock) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

	memcpy(&block , voidBlock , sizeof(Block));

    int entries = MAX_PRIM_RECS;

    block.nextBlock = -1;

    for (int i = 0 ; i < entries ; i++)
    {
        block.rec[i].id = -1;
        block.rec[i].name   [0] = '\0';
        block.rec[i].surname[0] = '\0';
        block.rec[i].address[0] = '\0';
    }


    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    printf("%d\n" , block.rec[0].id);

    return blockID;
}

// void DispayPrimaryIndex(char* filename)
// {
// 	HT_info*  info;
//
//     info = HT_OpenIndex(filename);
//
//     printf("\033[1;34m!!!!!!!!!!!!! FILEDESC FROM DISPLAY PRIMARY = %d\033[0m\n", info->fileDesc);
//
// 	Block* block;
//
//     int entries = MAX_PRIM_RECS;
//
// 	FILE* index_fp;
// 	index_fp = fopen("index.txt", "w");
//
// 	for (int i = 1 ; i <= info->numBuckets ; i++)
//     {
//         // unsigned int totalEntries = 0;
//         int blockID = i;
//
//         while (blockID != -1)
//         {
//             if (BF_ReadBlock(info->fileDesc , blockID , (void **)&block) < 0) {
//                 BF_PrintError("Error getting block");
// 				fclose(index_fp);
//                 return;
//             }
//
// 			fprintf(index_fp,"\nBLOCK ID = %d\n", blockID);
//
//             for (int j = 0 ; j < entries ; j++)
//             {
//                 if (block->rec[j].name[0] == '\0')
//                     break;
//
// 				fprintf(index_fp,"     ID: %d\n", block->rec[j].id);
//                 fprintf(index_fp,"   Name: %s\n", block->rec[j].name);
//                 fprintf(index_fp,"Surname: %s\n", block->rec[j].surname);
//                 fprintf(index_fp,"Address: %s\n\n", block->rec[j].address);
//
//             } // for
//
//             blockID = block->nextBlock;
//         } // while
//     } // for
// 	fclose(index_fp);
//
// 	HT_CloseIndex(info);
// }
//
// void DispaySecondaryIndex(char* filename)
// {
// 	SHT_info*  info;
//
//     info = SHT_OpenSecondaryIndex(filename);
//
//     printf("\033[1;34m!!!!!!!!!!!!! FILEDESC FROM DISPLAY SECONDARY = %d\033[0m\n", info->sfileDesc);
//
// 	SecondaryBlock* block;
//
//     int entries = MAX_PRIM_RECS;
//
// 	FILE* index_fp;
// 	index_fp = fopen("secondaryindex.txt", "w");
//
// 	for (int i = 1 ; i <= info->numBuckets ; i++)
//     {
//         // unsigned int totalEntries = 0;
//         int blockID = i;
//
//         while (blockID != -1)
//         {
//             if (BF_ReadBlock(info->sfileDesc , blockID , (void **)&block) < 0) {
//                 BF_PrintError("Error getting block");
// 				fclose(index_fp);
//                 return;
//             }
//
// 			fprintf(index_fp,"\nBLOCK ID = %d\n", blockID);
//
//             for (int j = 0 ; j < entries ; j++)
//             {
//                 if (block->rec[j].record.name[0] == '\0')
//                     break;
//
// 				fprintf(index_fp,"     ID: %d\n", block->rec[j].record.id);
//                 fprintf(index_fp,"   Name: %s\n", block->rec[j].record.name);
//                 fprintf(index_fp,"Surname: %s\n", block->rec[j].record.surname);
//                 fprintf(index_fp,"Address: %s\n\n", block->rec[j].record.address);
//
//             } // for
//
//             blockID = block->nextBlock;
//         } // while
//     } // for
// 	fclose(index_fp);
//
// 	SHT_CloseSecondaryIndex(info);
// }

int SHTBlockInit(const int sfileDesc)
{
	SecondaryBlock block;
    void* voidBlock;
    int blockID;

    memset((void *)&block, 0 , sizeof(Block));

    if (BF_AllocateBlock(sfileDesc) < 0) {
        BF_PrintError("Error allocating block");
        return -1;
    }

    blockID = BF_GetBlockCounter(sfileDesc) - 1;

	if (BF_ReadBlock(sfileDesc , blockID , &voidBlock) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

	memcpy(&block , voidBlock , sizeof(SecondaryBlock));

    block.nextBlock = -1;

	int entries = MAX_SEC_RECS;

    for (int i = 0 ; i < entries ; i++)
    {
        block.rec[i].blockId           = -1;
        block.rec[i].id                = -1;
        memset((void *)&block.rec[i].secHashKey , 0 , sizeof(block.rec[i].secHashKey));
    }

    if (BF_WriteBlock(sfileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return blockID;
}
