/* File: SHT_structs.h */

#ifndef __SHT_STRUCTS__
#define __SHT_STRUCTS__

typedef struct {
    int    blockId; /* The block to which the record was inserted in the primary index */
    Record record;
} SecondaryRecord;

typedef struct {
    int               nextBlock;
    SecondaryRecord** rec;
} SecondaryBlock;

typedef struct {
    int      sfileDesc;  /* File ID at block level                             */
    char*    attrName;   /* Name of field that is the Key for the current file */
    int      attrLength; /* Size of field that is the Key for the current file */
    long int numBuckets; /* Number of "buckets" of the hashing file            */
    char*    fileName;   /* Name of the primary index                          */
} SHT_info;

#endif // __SHT_STRUCTS__
