#ifndef __SHT__
#define __SHT__

#include "../HT/HT.h"

typedef struct {
    Record record;
    int blockId; /*The block to which the record was inserted in the primary index*/
} SecondaryRecord;

typedef struct secondaryBlock {
    int               nextBlock;
    SecondaryRecord** rec;
} SecondaryBlock;

typedef struct {
    int      sfileDesc;    /* File ID at block level                                         */
    char*    attrName;    /* Name of field that is the Key for the current file             */
    int      attrLength;  /* Size of field that is the Key for the current file             */
    long int numBuckets;  /* Number of "buckets" of the hashing file                        */
    char*    fileName;     /* Name of the primary index*/
} SHT_info;

int SHT_CreateSecondaryIndex( char* sfileName,   /* File name                */
                     char* attrName,   /* Key-field Name           */
                     int   attrLength, /* Key-field Length         */
                     int   buckets,     /* Number of buckets        */ 
                     char* primaryFileName /*Primary index name*/);

SHT_info* SHT_OpenSecondaryIndex( char* sfileName );

int SHT_CloseSecondaryIndex( SHT_info* header_info );

int SHT_SecondaryInsertEntry( SHT_info header_info, /* File header       */
                              SecondaryRecord  record       /* Record definition */ );

int SHT_GetAllEntries( SHT_info header_info_sht, /* Secondary File header  */
                      HT_info header_info_ht ,    /* Primary File header */
                      void* value /* Value of Key-field to be searched */ );


#endif