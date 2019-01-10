/* File: SHT.h */

#ifndef __SHT__
#define __SHT__

// #include "SHT_structs.h"
// #include "../HT/HT_structs.h"
#include "../structs.h"

int SHT_CreateSecondaryIndex(char* sfileName,      /* File name         */
                             char* attrName,       /* Key-field Name    */
                             int   attrLength,     /* Key-field Length  */
                             int   buckets,        /* Number of buckets */
                             char* primaryFileName /*Primary index name */ );

SHT_info* SHT_OpenSecondaryIndex(char* sfileName);

int SHT_CloseSecondaryIndex(SHT_info* header_info);

int SHT_SecondaryInsertEntry(SHT_info header_info,   /* File header       */
                             SecondaryRecord  record /* Record definition */ );

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, /* Secondary File header             */
                               HT_info  header_info_ht , /* Primary File header               */
                               void*    value            /* Value of Key-field to be searched */ );

int SHT_PrintStats(SHT_info sec_info);

#endif // __SHT__
