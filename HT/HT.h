/* File: HT.h */

#ifndef __HT__
#define __HT__

typedef struct {
    int  id;
    char name[15];
    char surname[20];
    char address[40];
} Record;

typedef struct block {
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

int HT_CreateIndex( char* fileName,   /* File name                */
                    char  attrType,   /* Key-field Type: 'c', 'i' */
                    char* attrName,   /* Key-field Name           */
                    int   attrLength, /* Key-field Length         */
                    int   buckets     /* Number of buckets        */ );

HT_info* HT_OpenIndex( char* fileName );

int HT_CloseIndex( HT_info* header_info );

int HT_InsertEntry( HT_info header_info, /* File header       */
                    Record  record       /* Record definition */ );

int HT_DeleteEntry( HT_info header_info, /* File header                      */
                    void*   value        /* Value of Key-field to be deleted */ );

int HT_GetAllEntries( HT_info header_info, /* File header                       */
                      void*   value        /* Value of Key-field to be searched */ );

int BlockDelete(HT_info* header_info);

#endif // __HT__
