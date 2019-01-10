
    Block* block;
    int    entries = (BLOCK_SIZE - sizeof(Block)) / sizeof(Record);

    int blockID = ;

    while (blockID != -1)
    {
        if (BF_ReadBlock(header_info->fileDesc , blockID , (void **)&block) < 0) {
            BF_PrintError("Error getting block");
            return -1;
        }

        for (int j = 0 ; j < entries ; j++)
        {
            if (block->rec[j] == NULL)
                break;

            printf("BLOCK: %d\n", block->rec[j]->id);
            printf("BLOCK: %d\n", block->rec[j]->name);
            printf("BLOCK: %d\n", block->rec[j]->surname);
            printf("BLOCK: %d\n", block->rec[j]->address);
        } // for

        blockID = block->nextBlock;
    } // while
