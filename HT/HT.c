/* File: Ht.c */

#include "HT.h"
#include "BF.h"

int HT_CreateIndex(char* fileName, char  attrType, char* attrName, int   attrLength, int buckets)
{
    int file;

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
}

HT_info* HT_OpenIndex(char* fileName)
{

}

int HT_CloseIndex(HT_info* header_info)
{

}

int HT_InsertEntry(HT_info header_info, Record record)
{
    // rigit
    // oronge
}

int HT_DeleteEntry(HT_info header_info, void* value)
{

}

int HT_GetAllEntries( HT_info header_info, void* value)
{

}
