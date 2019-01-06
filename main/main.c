
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../HT/HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"


int main(void){

/*FIRST PART*/
    HT_info* info;
    Record rec;

    FILE* gen_fp;
    gen_fp = fopen("entries.txt","r");
    if (gen_fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }
    BF_Init();
    HT_CreateIndex("file1" , 'i' , "character" , 10 , 3);
    info = HT_OpenIndex("file1");


    char* line = NULL;
    size_t len = 0;
    int cntr = 0;

    int primFileDesc = info->fileDesc;

    while (1)
    {
        char* token = NULL;

        fscanf(gen_fp, "{%d",&rec.id);

        if(getline(&line, &len, gen_fp) == -1) {
            break;
        }

        strtok(line,"\"");
        token = strtok(NULL,"\"");
        strcpy(rec.name, token);

        strtok(NULL,"\"");
        token = strtok(NULL,"\"");
        strcpy(rec.surname, token);

        strtok(NULL,"\"");
        token = strtok(NULL,"\"");
        strcpy(rec.address, token);

        cntr++;

        if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
		    BF_PrintError("Error getting block");
		    return -1;
	    }

        printf("INSERTED %d = %d\n" , cntr , HT_InsertEntry(*info,rec));

        if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
		    BF_PrintError("Error getting block");
		    return -1;
	    }


        if (rec.id % 4 == 0 || rec.id % 3 == 0){/*example search*/
            if (BF_ReadBlock(primFileDesc , 0 , (void **)&info) < 0) {
		        BF_PrintError("Error getting block");
		        return -1;
	        }
            printf("ENTRY %d: %d\n\n", cntr , HT_GetAllEntries(*info, &rec.id));
        }

        if (feof(gen_fp))
            break;

        free(line);
        line = NULL;
    }
    fclose(gen_fp);

/*SECOND PART*/

    SHT_info* sinfo;
    SecondaryRecord secRec;


    FILE* gen_fp;
    gen_fp = fopen("entries.txt","r");
    if (gen_fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    SHT_CreateSecondaryIndex("sfile" , "Name" , 10 , 3 , "file1");
    sinfo = SHT_OpenSecondaryIndex("sfile");

    char* line = NULL;
    size_t len = 0;
    int cntr = 0;

    int secFileDesc = sinfo->sfileDesc;

    /*while (1)
    {
        read file
    }*/

}