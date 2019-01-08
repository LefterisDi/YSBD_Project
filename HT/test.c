
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"
#include "../AuxFuncs/auxFuncs.h"

int main(int argc, char* argv[])
{
    HT_info* info;
    SHT_info* sinfo;
    Record rec;
    SecondaryRecord secRec;
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 0\n");

    if (argc < 2) {
        printf("Error: Missing input file\n");
    }

    FILE* gen_fp;
    gen_fp = fopen(argv[1],"r");
    if (gen_fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    info = (HT_info *)malloc(sizeof(HT_info));
    if (info == NULL) {
        perror("Cannot allocate memory");
        fclose(gen_fp);
        exit(EXIT_FAILURE);
    }

    sinfo = (SHT_info *)malloc(sizeof(SHT_info));
    if (sinfo == NULL) {
        perror("Cannot allocate memory");
        // free(info);
        fclose(gen_fp);
        exit(EXIT_FAILURE);
    }

    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    BF_Init();
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
    HT_CreateIndex("file1" , 'i' , "Id" , 2 , 10);
    HT_info* tmp_info = HT_OpenIndex("file1");

    // info->fileDesc   = tmp_info->fileDesc;
    // info->attrType   = tmp_info->attrType;
    // info->attrName   = tmp_info->attrName;
    // info->attrLength = tmp_info->attrLength;
    // info->numBuckets = tmp_info->numBuckets;

    *info = *tmp_info;

    SHT_CreateSecondaryIndex("sfile" , "Address" , 10 , 100 , "file1");
    SHT_info* tmp_sinfo = SHT_OpenSecondaryIndex("sfile");
    // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");

    // SHT_info* secInfo = SHT_OpenSecondaryIndex("file1");
    // if (secInfo == NULL) {
    //     printf("ERROR\n");
    // }
    //
    // printf("%d\n", secInfo->sfileDesc);
    // printf("%s\n", secInfo->fileName);
    // printf("%s\n", secInfo->attrName);
    // printf("%d\n", secInfo->attrLength);
    // printf("%ld\n", secInfo->numBuckets);
    //
    // fclose(gen_fp);
    // free(info);
    // free(sinfo);
    // return -1;

    // sinfo->sfileDesc  = tmp_sinfo->sfileDesc;
    // sinfo->fileName   = tmp_sinfo->fileName;
    // sinfo->attrName   = tmp_sinfo->attrName;
    // sinfo->attrLength = tmp_sinfo->attrLength;
    // sinfo->numBuckets = tmp_sinfo->numBuckets;

    *sinfo = *tmp_sinfo;

    char* line = NULL;
    size_t len = 0;
    int cntr = 0;

    // int primFileDesc = info->fileDesc;
    // int secFileDesc = sinfo->sfileDesc;

    while (1)
    {
        char* token = NULL;


        fscanf(gen_fp, "{%d",&rec.id);

        if(getline(&line, &len, gen_fp) == -1) {
            break;
        }

        // printf("LINE = %s\n",line);

        strtok(line,"\"");
        token = strtok(NULL,"\"");
        // printf("TOKEN = %s\n",token);
        strcpy(rec.name, token);

        strtok(NULL,"\"");
        token = strtok(NULL,"\"");
        // printf("TOKEN = %s\n",token);
        strcpy(rec.surname, token);

        strtok(NULL,"\"");
        token = strtok(NULL,"\"");
        // printf("TOKEN = %s\n",token);
        strcpy(rec.address, token);

        // printf("\nID = %d\n",rec.id);
        // printf("NAME = %s\n",rec.name);
        // printf("SURNAME = %s\n",rec.surname);
        // printf("ADDRESS = %s\n",rec.address);


        cntr++;

        // if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
		//     BF_PrintError("Error getting block");
		//     return -1;
	    // }

        secRec.record = rec;
        secRec.blockId = HT_InsertEntry(*info,rec);
        // secRec.blockId = 0;

        printf("INSERTED %d = %d\n" , cntr , secRec.blockId);


        // if (BF_ReadBlock(secFileDesc , 0 , (void **)&sinfo) < 0) {
		//     BF_PrintError("Error getting block");
		//     return -1;
	    // }

        printf("INSERTED SECONDARY %d = %d\n" , cntr , SHT_SecondaryInsertEntry(*sinfo , secRec));

        // printf("INFO FROM MAIN: FileDesc = %d\n", info->fileDesc);
        // printf("INFO FROM MAIN: AttrType = %c\n", info->attrType);
        // printf("INFO FROM MAIN: AttrName = %s\n", info->attrName);
        // printf("INFO FROM MAIN: AttrLen  = %d\n", info->attrLength);
        // printf("INFO FROM MAIN: Buckets  = %ld\n", info->numBuckets);
        // printf("REC ID FROM MAIN = %d\n", rec.id);

        // if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
        //    BF_PrintError("Error getting block");
        //    return -1;
        // }

        printf("ENTRY %d: %d\n\n", cntr , HT_GetAllEntries(*info, &rec.id));



        // return 0;
        if (feof(gen_fp))
            break;

        // line = NULL;
    }

    fclose(gen_fp);

        //  if (BF_ReadBlock(secFileDesc , 0 , (void **)&sinfo) < 0) {
		//     BF_PrintError("Error getting block");
		//     return -1;
	    // }

       //  if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
       //     BF_PrintError("Error getting block");
       //     return -1;
       // }

    printf("SECONDARY GETALL: %d\n\n", SHT_SecondaryGetAllEntries(*sinfo,*info,"Bakersfield"));


   //  if (BF_ReadBlock(secFileDesc , 0 , (void **)&sinfo) < 0) {
   //     BF_PrintError("Error getting block");
   //     return -1;
   // }

   // if (BF_CloseFile(info->fileDesc) < 0) {
   //     BF_PrintError("Error closing file");
   //     return -1;
   // }
   //
   // if (BF_CloseFile(sinfo->sfileDesc) < 0) {
   //     BF_PrintError("Error closing file");
   //     return -1;
   // }

   printf("CLOSING INDEX = %d\n" , HT_CloseIndex(info));
   printf("CLOSING SECONDARY INDEX = %d\n" , SHT_CloseSecondaryIndex(sinfo));

   HashStatistics("file1");
   HashStatistics("sfile");

   // *sinfo = *(SHT_OpenSecondaryIndex("sfile"));
   // printf("FILEDESC FROM MAIN 1 = %d\n", sinfo->sfileDesc);

    // printf("BLOCK DELETE = %d\n", SHTBlockDelete("sfile"));
   //  if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
   //     BF_PrintError("Error getting block");
   //     return -1;
   // }
   // *info = *(HT_OpenIndex("file1"));
   // printf("FILEDESC FROM MAIN 2 = %d\n", info->fileDesc);
    // printf("BLOCK DELETE = %d\n", BlockDelete("file1"));

    // printf("CLOSING INDEX = %d\n" , HT_CloseIndex(info));
    // printf("CLOSING SECONDARY INDEX = %d\n" , SHT_CloseSecondaryIndex(sinfo));

   //  if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
   //     BF_PrintError("Error getting block");
   //     return -1;
   // }

    // if (BF_ReadBlock(secFileDesc , 0 , (void **)&sinfo) < 0) {
    //     BF_PrintError("Error getting block");
    //     return -1;
    // }


    free(line);
    free(info);
    free(sinfo);

    return 0;
}
