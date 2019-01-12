/* File: HT_structs.h */

#ifndef __STRUCTS__
#define __STRUCTS__

#include <stdbool.h>

#include "./BF/BF.h"

typedef struct {
    int  id;
    char name[15];
    char surname[20];
    char address[40];
} Record;

#define MAX_PRIM_RECS ((BLOCK_SIZE - sizeof(int)) / sizeof(Record))

typedef struct {
    int    nextBlock;
    Record rec[MAX_PRIM_RECS];
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
    int  blockId; /* The block to which the record was inserted in the primary index */
    int  id;
    char secHashKey[40];
} SecondaryEntry;

#define MAX_SEC_RECS  ((BLOCK_SIZE - sizeof(int)) / sizeof(SecondaryEntry))

typedef struct {
    int               nextBlock;
    SecondaryEntry rec[MAX_SEC_RECS];
} SecondaryBlock;

typedef struct sec_info {
    int      sfileDesc;  /* File ID at block level                             */
    char*    attrName;   /* Name of field that is the Key for the current file */
    int      attrLength; /* Size of field that is the Key for the current file */
    long int numBuckets; /* Number of "buckets" of the hashing file            */
    char*    fileName;   /* Name of the primary index                          */
} SHT_info;

union info_types {
    HT_info  ht_info;
    SHT_info sht_info;
};

typedef struct {
    bool             hash_type;        /* This flag indicates the hashing that was used for a file */
    union info_types info;
    // HT_info  info;
    // SHT_info sec_info;
} Info;

// typedef struct {
//     bool hashFlag;        /* This flag indicates the hashing that was used for a file */
//     SHT_info* info;
// } SecInfo;

#endif // __STRUCTS__
