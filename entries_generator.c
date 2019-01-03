/* File: entries_generator.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "entries_generator.h"

int main(int argc, char const *argv[])
{
    FILE* gen_fp;

    int entries;

    if (argc < 2)
        entries = rand() % 1000;
    else
        entries = atoi(argv[1]);

    gen_fp = fopen("entries.txt", "w");
    if (gen_fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    for (int i = 0; i < entries; i++)
    {
        int   id       = rand() % (10 * entries);
        char* lastname = lastNames[rand() % LAST_NAMES_ENTRIES];
        char* address  = cities   [rand() % CITIES_NAMES_ENTRIES];
        char* name;

        if (rand() % 2)
            name = femaleNames[rand() % FEMALE_NAMES_ENTRIES];
        else
            name = maleNames[rand() % MALE_NAMES_ENTRIES];

        fprintf(gen_fp,"{%d,\"%s\",\"%s\",\"%s\"}\n", id,name,lastname,address);
    }

    fclose(gen_fp);

    return 0;
}
