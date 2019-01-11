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
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ALLOCATED ID = %d\n", blockID);


    // printf("BLOCKID FROM BLOCKINIT = %d\n",blockID);

	if (BF_ReadBlock(fileDesc , blockID , (void **)&block) < 0) {
		BF_PrintError("Error getting block");
        // free(initialBlock);
		return -1;
	}

    // initialBlock->nextBlock = -1;

    int entries = MAX_PRIM_RECS;

    // initialBlock->rec = (Record **)malloc(entries * sizeof(Record *));
    block->nextBlock = -1;

    for (int i = 0 ; i < entries ; i++)
    {
        block->rec[i].id = -1;
        block->rec[i].name   [0] = '\0';
        block->rec[i].surname[0] = '\0';
        block->rec[i].address[0] = '\0';
    }

	// memcpy(block , initialBlock , sizeof(Block));

    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        // free(initialBlock);
        return -1;
    }

    // free(initialBlock);
    return blockID;
}
//
// int BlockSearch(HT_info header_info, const int id)
// {
//     Block* block;
//     int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
//     int    blockID = HashFunc(id , header_info.numBuckets) + 1;
//
//     while(blockID != -1)
//     {
//         if (BF_ReadBlock(header_info.fileDesc , blockID , (void **)&block) < 0) {
//             BF_PrintError("Error getting block");
//             return -1;
//         }
//
//         for (int i = 0 ; i < entries ; i++)
//         {
//             if (block->rec[i] == NULL)
//                 return -1;
//
//             if (block->rec[i]->id == id)
//                 return blockID;
//         }
//
//         blockID = block->nextBlock;
//     }
//
//     return -1;
// }
//
// int BlockDelete(char* filename)
// {
//     Block* block;
//     int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);
//     // int primFileDesc = header_info->fileDesc;
//
//     // HT_info  header_info;
//
//     HT_info* header_info = HT_OpenIndex(filename);
//     if (header_info == NULL)
//         return -1;
//
//     // header_info = *tmp_info;
//     // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
//     for (int i = 0; i < header_info->numBuckets; i++)
//     {
//
//         // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
//         int blockID = i + 1;
//
//         {
//             printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! INSERTING TO 1st FILE\n");
//             int newBlockID = i + 1;
//
//             bool leave = false;
//             FILE* tmp_fp;
//             tmp_fp = fopen("res2.txt", "w");
//
//             while (newBlockID != -1)
//             {
//                 Block* tmp_block;
//
//                 if (BF_ReadBlock(header_info->fileDesc , newBlockID , (void **)&tmp_block) < 0) {
//                     BF_PrintError("Error getting block");
//                     fclose(tmp_fp);
//                     return -1;
//                 }
//
//                 fprintf(tmp_fp,"\nBLOCK ID = %d\n", newBlockID);
//
//                 for (int i = 0 ; i < entries ; i++)
//                 {
//                     if (tmp_block->rec[i] == NULL)
//                     {
//                         leave = true;
//                         break;
//                     }
//                     fprintf(tmp_fp,"PRINTING ID = %d\n", tmp_block->rec[i]->id);
//                 }
//
//                 if (leave)
//                     break;
//
//                 newBlockID = tmp_block->nextBlock;
//             }
//             fclose(tmp_fp);
//             printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! INSERTING TO 1st FILE\n");
//         }
//
//         while (blockID != -1)
//         {
//             printf("\nBLOCKID = %d\n", blockID);
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
//             if (BF_ReadBlock(header_info->fileDesc , blockID , (void **)&block) < 0) {
//                 BF_PrintError("Error getting block");
//
//                 if (BF_CloseFile(header_info->fileDesc) < 0) {
//             		BF_PrintError("Error closing file");
//             	}
//
//                 return -1;
//             }
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");
//
//             for (int j = 0 ; j < entries ; j++)
//             {
//                 // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
//                 if (block->rec[j] == NULL)
//                     break;
//                     // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");
//
//                 printf("PRINTING ID = %d\n", block->rec[j]->id);
//                 free(block->rec[j]);
//                 // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
//             } // for
//
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");
//             deletes2++;
//             printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DELETES    = %d\n", deletes2);
//             printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DELETED ID = %d\n", blockID);
//             free(block->rec);
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
//
//             if (BF_WriteBlock(header_info->fileDesc , blockID) < 0) {
//                 BF_PrintError("Error writing block back");
//
//                 HT_CloseIndex(header_info);
//
//                 // if (BF_CloseFile(header_info->fileDesc) < 0) {
//             	// 	BF_PrintError("Error closing file");
//             	// }
//
//                 return -1;
//             }
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");
//
//             blockID = block->nextBlock;
//         } // while
//         getchar();
//
//
//         // if (BF_ReadBlock(primFileDesc , 0 , (void **)&header_info) < 0) {
// 		//     BF_PrintError("Error getting block");
// 		//     return -1;
// 	    // }
//
//
//         // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
//     } // for
//
//     HT_CloseIndex(header_info);
//
//     // if (BF_CloseFile(header_info->fileDesc) < 0) {
//     //     BF_PrintError("Error closing file");
//     //     return -1;
//     // }
//
//     // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
//     return 0;
// }


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

	int entries = MAX_SEC_RECS;

    for (int i = 0 ; i < entries ; i++)
    {
        block->rec[i].blockId           = -1;
        block->rec[i].record.id         = -1;
        block->rec[i].record.name   [0] = '\0';
        block->rec[i].record.surname[0] = '\0';
        block->rec[i].record.address[0] = '\0';
    }

    if (BF_WriteBlock(fileDesc , blockID) < 0) {
        BF_PrintError("Error writing block back");
        return -1;
    }

    return blockID;
}
//
// int SHTBlockDelete(char* filename)
// {
//     SecondaryBlock* block;
//     int    entries = (BLOCK_SIZE - sizeof(SecondaryBlock)) / sizeof(SecondaryRecord);
// 	int cntr = 0;
//     // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
// 	// int secFileDesc = header_info->sfileDesc;
// 	//
// 	// if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info) < 0) {
// 	// 	BF_PrintError("Error getting block");
// 	// 	return -1;
// 	// }
//
//     // SHT_info  header_info;
//
//     SHT_info* header_info = SHT_OpenSecondaryIndex(filename);
//     if (header_info == NULL)
//         return -1;
//
//     // header_info = *tmp_sinfo;
//
//
//     for (int i = 0; i < header_info->numBuckets; i++)
//     {
//
//         // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
//         int blockID = i + 1;
//
//         while (blockID != -1)
//         {
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
//             if (BF_ReadBlock(header_info->sfileDesc , blockID , (void **)&block) < 0) {
//                 BF_PrintError("Error getting block");
//
//                 if (BF_CloseFile(header_info->sfileDesc) < 0) {
//                     BF_PrintError("Error closing file");
//                 }
//
//                 return -1;
//             }
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 4\n");
//
//             for (int j = 0 ; j < entries ; j++)
//             {
//                 // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 5\n");
//                 if (block->rec[j] == NULL)
//                     break;
//                     // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 6\n");
//
// 				cntr++;
//                 free(block->rec[j]);
//                 // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 7\n");
//             } // for
//
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 8\n");
//             free(block->rec);
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 9\n");
//
//             if (BF_WriteBlock(header_info->sfileDesc , blockID) < 0) {
//                 BF_PrintError("Error writing block back");
//
//                 SHT_CloseSecondaryIndex(header_info);
//                 // if (BF_CloseFile(header_info->sfileDesc) < 0) {
//                 //     BF_PrintError("Error closing file");
//                 // }
//
//                 return -1;
//             }
//             // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 10\n");
//
//             blockID = block->nextBlock;
//         } // while
//
// 		// if (BF_ReadBlock(secFileDesc , 0 , (void **)&header_info) < 0) {
// 		//     BF_PrintError("Error getting block");
// 		//     return -1;
// 	    // }
//
//         // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 11\n");
//     } // for
//
//     SHT_CloseSecondaryIndex(header_info);
//     //
//     // if (BF_CloseFile(header_info->sfileDesc) < 0) {
//     //     BF_PrintError("Error closing file");
//     //     return -1;
//     // }
//
//     // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 12\n");
// 	// printf("CNTR = %d\n", cntr);
//     return 0;
// }
