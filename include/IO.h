#ifndef _IO_INCLUDED
#define _IO_INCLUDED 1

#include <upk.h>
#include <sys/stat.h>
#include <sys/types.h>

//file-structure to make it easier to read from files
typedef struct{
    u1* content;
    u4 length;
    u4 pos;
} UPK_FILE;

//IO.c funtion prototypes
UPK_FILE UPK_loadFile(char*);
void UPK_freeFile(UPK_FILE*);
u1 UPK_readU1(UPK_FILE*);
u4 UPK_readU3(UPK_FILE*);
u2 UPK_readU2(UPK_FILE*);
u4 UPK_readU4(UPK_FILE*);
u1 UPK_ensurePosition(UPK_FILE*);
const char* UPK_getFExt(char*);
u1 isBigEndian(void);
u1 createDirsRecursiv(char*);

#ifdef _WIN32
	#include <direct.h>
	#define UPK_DIRSEP '\\'
	#define MKDIR(d) _mkdir(d)
	#define RMDIR(d) _rmdir(d)
#else
	#define UPK_DIRSEP '/'
	#define MKDIR(d) mkdir(d, 0700)
	#define RMDIR(d) rmdir(d)
#endif //_WIN32

#endif //_IO_INCLUDED