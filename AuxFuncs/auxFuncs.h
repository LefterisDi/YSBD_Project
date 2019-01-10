/* File: auxFuncs.h */

#ifndef __AUX_FUNCS__
#define __AUX_FUNCS__

// #include "../HT/HT_structs.h"
// #include "../SHT/SHT_structs.h"
#include "../structs.h"

unsigned int strtoi(const char* str);
int HashFunc(const unsigned int id, const int mask);

int BlockInit(const int fileDesc);
// int BlockDelete(char* filename);
int BlockSearch(HT_info header_info, const int id);

int SHTBlockInit(const int fileDesc);
// int SHTBlockDelete(char* filename);

#endif // __AUX_FUNCS__
