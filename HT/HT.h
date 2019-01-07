/* File: HT.h */

#ifndef __HT__
#define __HT__

#include "HT_structs.h"

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

#endif // __HT__
