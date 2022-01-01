#ifndef _UPK_INCLUDED
#define _UPK_INCLUDED 1

#include <stdio.h>

//platform independent byte-defines
#include <stdint.h>

typedef uint8_t u1;		//always 1 byte
typedef uint16_t u2;	//always 2 bytes
typedef uint32_t u4;	//always 4 bytes

//function prototypes from upk.c
u1 checkArgs(int, char**, u1*);
void listContent(char*);
void extractContent(char*, char*);
void packUPK(u4, char**);

//used to determin what to do
#define UPK_PACK 0
#define UPK_LIST 1
#define UPK_EXTR 2
#define UPK_HELP 3

#endif //_UPK_INCLUDED
