
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

    rec.id = 1;
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
    HT_CreateIndex("file1" , 'c' , "character" , 10 , 3);
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CHECKPOINT 3\n");
    info = HT_OpenIndex("file1");
    printf("BLOCK COUNTER = %d\n",BF_GetBlockCounter(info->fileDesc));
    printf("INSERTED 1 = %d\n" , HT_InsertEntry(*info,rec));
    printf("INSERTED 2 = %d\n" , HT_InsertEntry(*info,rec2));
    printf("INSERTED 3 = %d\n" , HT_InsertEntry(*info,rec3));
    printf("INSERTED 4 = %d\n" , HT_InsertEntry(*info,rec4));
    printf("INSERTED 5 = %d\n" , HT_InsertEntry(*info,rec5));
    printf("INSERTED 6 = %d\n" , HT_InsertEntry(*info,rec6));
    printf("INSERTED 7 = %d\n" , HT_InsertEntry(*info,rec7));
    printf("INSERTED 7 = %d\n" , HT_InsertEntry(*info,rec7));

    printf("BLOCK COUNTER = %d\n",BF_GetBlockCounter(info->fileDesc));

}
