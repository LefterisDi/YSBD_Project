
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"

int main(void){

    HT_info* info;
    Record rec , rec2 , rec3 , rec4 , rec5 , rec6 , rec7;

    rec.id = 1;
    strcpy(rec.name , "Mitsos");
    strcpy(rec.surname , "Alitiz");
    strcpy(rec.address , "House of your mother");

    rec2.id = 2;
    strcpy(rec2.name , "Mitsos");
    strcpy(rec2.surname , "Alitiz");
    strcpy(rec2.address , "House of your mother");

    rec3.id = 3;
    strcpy(rec3.name , "Mitsos");
    strcpy(rec3.surname , "Alitiz");
    strcpy(rec3.address , "House of your mother");

    rec4.id = 4;
    strcpy(rec4.name , "Mitsos");
    strcpy(rec4.surname , "Alitiz");
    strcpy(rec4.address , "House of your mother");

    rec5.id = 5;
    strcpy(rec5.name , "Mitsos");
    strcpy(rec5.surname , "Alitiz");
    strcpy(rec5.address , "House of your mother");

    rec6.id = 6;
    strcpy(rec6.name , "Mitsos");
    strcpy(rec6.surname , "Alitiz");
    strcpy(rec6.address , "House of your mother");

    rec7.id = 7;
    strcpy(rec7.name , "Mitsos");
    strcpy(rec7.surname , "Alitiz");
    strcpy(rec7.address , "House of your mother");

    BF_Init();
    HT_CreateIndex("file1" , 'c' , "character" , 10 , 3);
    info = HT_OpenIndex("file1");
    printf("%d\n",BF_GetBlockCounter(info->fileDesc));
    printf("%d\n" , HT_InsertEntry(*info,rec));
    printf("%d\n" , HT_InsertEntry(*info,rec2));
    printf("%d\n" , HT_InsertEntry(*info,rec3));
    printf("%d\n" , HT_InsertEntry(*info,rec4));
    printf("%d\n" , HT_InsertEntry(*info,rec5));
    printf("%d\n" , HT_InsertEntry(*info,rec6));
    printf("%d\n" , HT_InsertEntry(*info,rec7));

    printf("%d\n",BF_GetBlockCounter(info->fileDesc));
    
}

