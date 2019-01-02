/* File: Ht.c */

#include <string.h>
#include <stdio.h>

#include "HT.h"
#include "../BF/BF.h"

int HashFunc(const int id, const int mask)
{
    return id % mask;
}

int HT_CreateIndex(char* fileName, char attrType, char* attrName, int attrLength, int buckets)
{
    int     file;
    void*   block;
    HT_info info;

	if (BF_CreateFile(fileName) < 0) {
		BF_PrintError("Error creating file");
		return -1;
	}

    if ((file = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
		return -1;
	}

    if (BF_AllocateBlock(file) < 0) {
		BF_PrintError("Error allocating block");
		return -1;
	}

    if (BF_ReadBlock(file , 0 , &block) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    info.fileDesc   = file;
    info.attrName   = attrName;
    info.attrLength = attrLength;
    info.attrType   = attrType;
    info.numBuckets = buckets;


    //copy content to the block
	memcpy(block, &info, sizeof(HT_info));

    if (BF_WriteBlock(file , 0) < 0){
		BF_PrintError("Error writing block back");
		return -1;
	}

    return 0;
}

HT_info* HT_OpenIndex(char* fileName)
{

}

int HT_CloseIndex(HT_info* header_info)
{

}

int HT_InsertEntry(HT_info header_info, Record record)
{

}

int HT_DeleteEntry(HT_info header_info, void* value)
{

}

int HT_GetAllEntries( HT_info header_info, void* value)
{

}
