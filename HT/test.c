
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"

int main(void)
{
    HT_info* info;
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
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
    info = HT_OpenIndex("file1");

    char* line = NULL;
    int cntr = 0;

    while (1)
    {
        char* token = NULL;
        size_t len = 0;

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
        printf("ENTRY %d: %d\n\n", cntr , HT_GetAllEntries(*info, &rec.id));



        // return 0;
        if (feof(gen_fp))
            break;
            
        free(line);
        line = NULL;
    }
    fclose(gen_fp);


    printf("BLOCK DELETE = %d\n", BlockDelete(info));
    printf("CLOSING INDEX = %d\n" , HT_CloseIndex(info));

    return 0;

    rec.id = 3;
    strcpy(rec.name , "Mitsos");
    strcpy(rec.surname , "Alitiz");
    strcpy(rec.address , "House of your mother");

    rec2.id = 4;
    strcpy(rec2.name , "Mitsos");
    strcpy(rec2.surname , "Alitiz");
    strcpy(rec2.address , "House of your mother");

    rec3.id = 7;
    strcpy(rec3.name , "Mitsos");
    strcpy(rec3.surname , "Alitiz");
    strcpy(rec3.address , "House of your mother");

    rec4.id = 10;
    strcpy(rec4.name , "Mitsos");
    strcpy(rec4.surname , "Alitiz");
    strcpy(rec4.address , "House of your mother");

    rec5.id = 13;
    strcpy(rec5.name , "Mitsos");
    strcpy(rec5.surname , "Alitiz");
    strcpy(rec5.address , "House of your mother");

    rec6.id = 16;
    strcpy(rec6.name , "Mitsos");
    strcpy(rec6.surname , "Alitiz");
    strcpy(rec6.address , "House of your mother");

    rec7.id = 19;
    strcpy(rec7.name , "Mitsos");
    strcpy(rec7.surname , "Alitiz");
    strcpy(rec7.address , "House of your mother");

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 1\n");
    BF_Init();
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 2\n");
    HT_CreateIndex("file1" , 'i' , "character" , 10 , 3);
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
    info = HT_OpenIndex("file1");
    printf("BLOCK COUNTER = %d\n" , BF_GetBlockCounter(info->fileDesc));
    printf("INSERTED 1 = %d\n" , HT_InsertEntry(*info,rec));
    printf("INSERTED 2 = %d\n" , HT_InsertEntry(*info,rec2));
    printf("INSERTED 3 = %d\n" , HT_InsertEntry(*info,rec3));
    printf("INSERTED 4 = %d\n" , HT_InsertEntry(*info,rec4));
    printf("INSERTED 5 = %d\n" , HT_InsertEntry(*info,rec5));
    printf("INSERTED 6 = %d\n" , HT_InsertEntry(*info,rec6));
    printf("INSERTED 7 = %d\n" , HT_InsertEntry(*info,rec7));
    printf("INSERTED 7 = %d\n" , HT_InsertEntry(*info,rec7));

    printf("\n-------------------------------------------------------------------------------\n\n");

    printf("ENTRY 1: %d\n", HT_GetAllEntries(*info, &rec.id));
    printf("DELETE 1 = %d\n" , HT_DeleteEntry(*info,&rec.id));
    printf("ENTRY 1: %d\n", HT_GetAllEntries(*info, &rec.id));

    printf("ENTRY 2: %d\n", HT_GetAllEntries(*info, &rec2.id));
    printf("DELETE 2 = %d\n" , HT_DeleteEntry(*info,&rec2.id));
    printf("ENTRY 2: %d\n", HT_GetAllEntries(*info, &rec2.id));

    printf("ENTRY 3: %d\n", HT_GetAllEntries(*info, &rec3.id));
    printf("DELETE 3 = %d\n" , HT_DeleteEntry(*info,&rec3.id));
    printf("ENTRY 3: %d\n", HT_GetAllEntries(*info, &rec3.id));

    printf("ENTRY 4: %d\n", HT_GetAllEntries(*info, &rec4.id));
    printf("DELETE 4 = %d\n" , HT_DeleteEntry(*info,&rec4.id));
    printf("ENTRY 4: %d\n", HT_GetAllEntries(*info, &rec4.id));

    printf("ENTRY 5: %d\n", HT_GetAllEntries(*info, &rec5.id));
    printf("DELETE 5 = %d\n" , HT_DeleteEntry(*info,&rec5.id));
    printf("ENTRY 5: %d\n", HT_GetAllEntries(*info, &rec5.id));

    printf("ENTRY 6: %d\n", HT_GetAllEntries(*info, &rec6.id));
    printf("DELETE 6 = %d\n" , HT_DeleteEntry(*info,&rec6.id));
    printf("ENTRY 6: %d\n", HT_GetAllEntries(*info, &rec6.id));

    printf("ENTRY 7: %d\n", HT_GetAllEntries(*info, &rec7.id));
    printf("DELETE 7 = %d\n" , HT_DeleteEntry(*info,&rec7.id));
    printf("ENTRY 7: %d\n", HT_GetAllEntries(*info, &rec7.id));

    printf("DELETE 7 = %d\n" , HT_DeleteEntry(*info,&rec7.id));

    printf("BLOCK COUNTER = %d\n" , BF_GetBlockCounter(info->fileDesc));
    printf("BLOCK DELETE = %d\n", BlockDelete(info));
    printf("CLOSING INDEX = %d\n" , HT_CloseIndex(info));
}
