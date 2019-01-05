
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"

int main(void)
{
    HT_info* info;
    SHT_info* sinfo;
    Record rec , rec2 , rec3 , rec4 , rec5 , rec6 , rec7;
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
    info = HT_OpenIndex("file1");
    SHT_CreateSecondaryIndex("sfile" , "character" , 10 , 3 , "file1");
    sinfo = SHT_OpenSecondaryIndex("sfile");
    int sFDesc = sinfo->sfileDesc;
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");


    char* line = NULL;
    int cntr = 0;

    while (1)
    {
        char* token = NULL;
        size_t len = 0;

        if (BF_ReadBlock(info->fileDesc , 0 , (void **)&info) < 0) {
		    BF_PrintError("Error getting block");
		    return -1;
	    }

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

        printf("ID = %d\n",rec.id);
        printf("NAME = %s\n",rec.name);
        printf("SURNAME = %s\n",rec.surname);
        printf("ADDRESS = %s\n",rec.address);

        cntr++;



        printf("INSERTED %d = %d\n" , cntr , HT_InsertEntry(*info,rec));

        printf("INFO FROM MAIN: FileDesc = %d\n", info->fileDesc);
        printf("INFO FROM MAIN: AttrType = %c\n", info->attrType);
        printf("INFO FROM MAIN: AttrName = %s\n", info->attrName);
        printf("INFO FROM MAIN: AttrLen  = %d\n", info->attrLength);
        printf("INFO FROM MAIN: Buckets  = %ld\n", info->numBuckets);
        printf("REC ID FROM MAIN = %d\n", rec.id);

        printf("ENTRY %d: %d\n\n", cntr , HT_GetAllEntries(*info, &rec.id));



        // return 0;
        if (feof(gen_fp))
            break;

        free(line);
        line = NULL;
    }
    fclose(gen_fp);

    printf("%d\n\n",sinfo->sfileDesc);


    printf("BLOCK DELETE = %d\n", BlockDelete(info));
    printf("CLOSING INDEX = %d\n" , HT_CloseIndex(info));
    if (BF_ReadBlock(sFDesc , 0 , (void **)&sinfo) < 0) {
		BF_PrintError("Error getting block");
		return -1;
	}

    printf("CLOSING SECONDARY INDEX = %d\n" , SHT_CloseSecondaryIndex(sinfo));


    return 0;
}
