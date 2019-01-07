
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../HT/HT.h"
#include "../BF/BF.h"
#include "../SHT/SHT.h"


int main(void){
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

        strtok(line,"\"");
        token = strtok(NULL,"\"");
        strcpy(rec.name, token);

        strtok(NULL,"\"");
        token = strtok(NULL,"\"");
        strcpy(rec.surname, token);

        strtok(NULL,"\"");
        token = strtok(NULL,"\"");
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


        if (rec.id % 4 == 0 || rec.id % 3 == 0){/*example search*/
            printf("ENTRY %d: %d\n\n", cntr , HT_GetAllEntries(*info, &rec.id));
        }

        if (feof(gen_fp))
            break;

        free(line);
        line = NULL;
    }
    fclose(gen_fp);
}
