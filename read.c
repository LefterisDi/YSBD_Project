#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    FILE* fp;

    fp = fopen(argv[1],"r");
    if (fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    int  id;
    char name[15];
    char surname[20];
    char address[40];

    while (!feof(fp))
    {
        char* token = NULL;

        fscanf(fp,"{%d,\"%15[^\"]\",\"%20[^\"]\",\"%40[^\"]\"}\n", &id, name, surname, address);

        printf("%d\n", id);
        printf("%s\n", name);
        printf("%s\n", surname);
        printf("%s\n\n", address);
    }

    fclose(fp);

    return 0;
}
