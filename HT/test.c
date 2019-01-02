
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "../BF/BF.h"

int main(void){
    BF_Init();
    HT_CreateIndex("file1" , 'c' , "character" , 10 , 1);
}

