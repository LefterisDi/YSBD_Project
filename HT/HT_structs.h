/* File: HT_structs.h */

#ifndef __HT_STRUCTS__
#define __HT_STRUCTS__

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

#endif // __HT_STRUCTS__
