/* File: SHT_structs.h */

#ifndef __SHT_STRUCTS__
#define __SHT_STRUCTS__

#include <stdbool.h>

#include "../HT/HT_structs.h"

typedef struct {
    int    blockId; /* The block to which the record was inserted in the primary index */
    Record record;
} SecondaryRecord;

typedef struct {
    int               nextBlock;
    SecondaryRecord** rec;
} SecondaryBlock;

typedef struct sec_info {
    int      sfileDesc;  /* File ID at block level                             */
    char*    attrName;   /* Name of field that is the Key for the current file */
    int      attrLength; /* Size of field that is the Key for the current file */
    long int numBuckets; /* Number of "buckets" of the hashing file            */
    char*    fileName;   /* Name of the primary index                          */
} SHT_info;

// typedef struct {
//     bool hashFlag;        /* This flag indicates the hashing that was used for a file */
//     SHT_info* info;
// } SecInfo;

#endif // __SHT_STRUCTS__
