/* File: HT_structs.h */

#ifndef __STRUCTS__
#define __STRUCTS__

#include <stdbool.h>

typedef struct {
    int  id;
    char name[15];
    char surname[20];
    char address[40];
} Record;

typedef struct {
    int      nextBlock;
    Record** rec;
} Block;

typedef struct {
    int      fileDesc;    /* File ID at block level                                         */
    char     attrType;    /* Type of field that is the Key for the current file: 'c' or 'i' */
    char*    attrName;    /* Name of field that is the Key for the current file             */
    int      attrLength;  /* Size of field that is the Key for the current file             */
    long int numBuckets;  /* Number of "buckets" of the hashing file                        */
} HT_info;

/* ----------------------------------------------------------------------------------------------------------------------- */

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

typedef struct {
    bool     hashFlag;        /* This flag indicates the hashing that was used for a file */
    HT_info  info;
    SHT_info sec_info;
} Info;

// typedef struct {
//     bool hashFlag;        /* This flag indicates the hashing that was used for a file */
//     SHT_info* info;
// } SecInfo;

#endif // __STRUCTS__
