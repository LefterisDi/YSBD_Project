
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../HT/HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"
#include "../AuxFuncs/auxFuncs.h"


int main(int argc, char* argv[])
{
    HT_info* info;
    SHT_info* sinfo;
    Record rec;
    SecondaryRecord secRec;
    char* file1;
    FILE* gen_fp;
    FILE* gen_fp2;

    while(--argc > 0){

        if (strstr(*argv , "-F1") != NULL){
            argv++;
            argc--;
            file1 = *argv;
            gen_fp = fopen(*argv,"r");
            if (gen_fp == NULL) {
                perror("Cannot open file");
                exit(EXIT_FAILURE);
            }
        }

        if (strstr(*argv , "-F2") != NULL){
            argv++;
            argc--;
            gen_fp2 = fopen(*argv,"r");
            if (gen_fp2 == NULL) {
                perror("Cannot open file");
                exit(EXIT_FAILURE);
            }
        }

        if (argc > 0){
            argv++;
        }
    }

    info = (HT_info *)malloc(sizeof(HT_info));
    if (info == NULL) {
        perror("Cannot allocate memory");
        fclose(gen_fp);
        fclose(gen_fp2);
        exit(EXIT_FAILURE);
    }

    sinfo = (SHT_info *)malloc(sizeof(SHT_info));
    if (sinfo == NULL) {
        perror("Cannot allocate memory");
        free(info);
        fclose(gen_fp);
        fclose(gen_fp2);
        exit(EXIT_FAILURE);
    }

    BF_Init();

    HT_CreateIndex("file1" , 'i' , "character" , 10 , 5);
    HT_info* tmp_info = HT_OpenIndex("file1");

    *info = *tmp_info;

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

        printf("INSERTED %d = %d\n" , cntr ,HT_InsertEntry(*info,rec));

        if (feof(gen_fp))
            break;

    }
    fclose(gen_fp);

    line = NULL;
    len = 0;
    cntr = 0;
    gen_fp = fopen(file1,"r");
    if (gen_fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    while(1){
        char* token = NULL;

        fscanf(gen_fp, "{%d",&rec.id);

        printf("%d\n",rec.id);

        if(getline(&line, &len, gen_fp) == -1) {
            break;
        }

        cntr++;

        if(rec.id % 4 == 0 || rec.id % 3 == 0){/*Example search*/
            printf("ID %d found after searching %d blocks\n" , rec.id , HT_GetAllEntries(*info , &rec.id));
        }

        if (feof(gen_fp))
            break;
    }
    fclose(gen_fp);

    SHT_CreateSecondaryIndex("sfile" , "Address" , 10 , 100 , "file1");
    SHT_info* tmp_sinfo = SHT_OpenSecondaryIndex("sfile");

    *sinfo = *tmp_sinfo;

    while (1)
    {
        char* token = NULL;

        fscanf(gen_fp2, "{%d",&rec.id);

        if(getline(&line, &len, gen_fp2) == -1) {
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

        secRec.record = rec;
        secRec.blockId = BlockSearch(*info , rec.id);

        printf("INSERTED SECONDARY %d = %d\n" , cntr , SHT_SecondaryInsertEntry(*sinfo , secRec));

        if (feof(gen_fp2))
            break;

    }
    fclose(gen_fp2);

    printf("SECONDARY GETALL: %d\n\n", SHT_GetAllEntries(*sinfo,*info,"Honolulu"));

    printf("SECONDARY BLOCK DELETE = %d\n", SHTBlockDelete(sinfo));
    printf("BLOCK DELETE = %d\n", BlockDelete(info));


    printf("CLOSING INDEX = %d\n" , HT_CloseIndex(info));
    printf("CLOSING SECONDARY INDEX = %d\n" , SHT_CloseSecondaryIndex(sinfo));

    free(line);
    return 0;
}