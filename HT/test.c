
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"

int main(void)
{
    HT_info* tempInfo;
    HT_info* info;
    SHT_info* tempSinfo;
    SHT_info* sinfo;
    Record rec;
    SecondaryRecord secRec;
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 0\n");

    FILE* gen_fp;
    gen_fp = fopen("entries.txt","r");
    if (gen_fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    BF_Init();
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
    HT_CreateIndex("file1" , 'i' , "character" , 10 , 3);
    tempInfo = HT_OpenIndex("file1");
    info = malloc(sizeof(HT_info));
    info->attrLength = tempInfo->attrLength;
    info->attrName = tempInfo->attrName;
    info->attrType = tempInfo->attrType;
    info->fileDesc = tempInfo->fileDesc;
    info->numBuckets = tempInfo->numBuckets;
    SHT_CreateSecondaryIndex("sfile" , "Address" , 10 , 3 , "file1");
    tempSinfo = SHT_OpenSecondaryIndex("sfile");
    sinfo = malloc(sizeof(SHT_info));
    sinfo->attrLength = tempSinfo->attrLength;
    sinfo->attrName = tempSinfo->attrName;
    sinfo->fileName = tempSinfo->fileName;
    sinfo->numBuckets = tempSinfo->numBuckets;
    sinfo->sfileDesc = tempSinfo->sfileDesc;
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");


    char* line = NULL;
    size_t len = 0;
    int cntr = 0;

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

        printf("\nID = %d\n",rec.id);
        printf("NAME = %s\n",rec.name);
        printf("SURNAME = %s\n",rec.surname);
        printf("ADDRESS = %s\n",rec.address);


        cntr++;

        secRec.record = rec;
        secRec.blockId = HT_InsertEntry(*info,rec);
        // secRec.blockId = 0;

        printf("INSERTED %d = %d\n" , cntr , secRec.blockId);


        printf("INSERTED SECONDARY %d = %d\n" , cntr , SHT_SecondaryInsertEntry(*sinfo , secRec));

        // printf("INFO FROM MAIN: FileDesc = %d\n", info->fileDesc);
        // printf("INFO FROM MAIN: AttrType = %c\n", info->attrType);
        // printf("INFO FROM MAIN: AttrName = %s\n", info->attrName);
        // printf("INFO FROM MAIN: AttrLen  = %d\n", info->attrLength);
        // printf("INFO FROM MAIN: Buckets  = %ld\n", info->numBuckets);
        printf("REC ID FROM MAIN = %d\n", rec.id);

        
        printf("ENTRY %d: %d\n\n", cntr , HT_GetAllEntries(*info, &rec.id));



        // return 0;
        if (feof(gen_fp))
            break;

        // line = NULL;
    }
    fclose(gen_fp);



    printf("SECONDARY GETALL: %d\n\n", SHT_GetAllEntries(*sinfo,*info,"Bakersfield"));



    printf("BLOCK DELETE = %d\n", SHTBlockDelete(sinfo));

    printf("BLOCK DELETE = %d\n", BlockDelete(info));
    
    printf("CLOSING INDEX = %d\n" , HT_CloseIndex(info));

    printf("CLOSING SECONDARY INDEX = %d\n" , SHT_CloseSecondaryIndex(sinfo));


    free(line);
    return 0;
}
