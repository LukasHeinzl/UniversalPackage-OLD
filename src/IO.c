#include <upk.h>
#include <IO.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//reads from the given file and saves it in an UPK_FILE structure
UPK_FILE UPK_loadFile(char* src){
	UPK_FILE f = {NULL, 0, 0};
	
    FILE* fp = fopen(src, "rb");
	
	//file not found
    if(fp == NULL){
        printf("File not found: %s", src);
		return f;
    }

	//get buffer size for allocation
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);

	//allocate buffer and do memory check
    u1* buffer = (u1*) malloc(sizeof(u1) * (len + 1));
    if(buffer == NULL){
        printf("Could not allocate file-buffer, out of memory...\n");
		return f;
    }
	
	//read into buffer
    fread(buffer, len, 1, fp);
    fclose(fp);
	
	//save into struct and return
    f.content = buffer;
    f.length = (u4) len;

    return f;
}

//function to free file-buffer
void UPK_freeFile(UPK_FILE* f){
    free(f->content);
}

//reads a single unsigned byte from file-buffer
u1 UPK_readU1(UPK_FILE* f){
    u1 u = (f->content)[f->pos];
    (f->pos)++;
    if(!UPK_ensurePosition(f)){
		printf("Terminating...\n");
		exit(1);
	}
    return u;
}

//reads 2 unsigned bytes from file-buffer
u2 UPK_readU2(UPK_FILE* f){
	if(isBigEndian()){
		u2 u = ((u2) UPK_readU1(f)) << 8;
		return u + UPK_readU1(f);
	}
    u2 u = (u2) (UPK_readU1(f));
	return (((u2) UPK_readU1(f)) << 8) + u;
}

//reads 3 unsigned bytes from file-buffer
u4 UPK_readU3(UPK_FILE* f){
	if(isBigEndian()){
		u4 u = ((u2) UPK_readU2(f)) << 8;
		return u + UPK_readU1(f);
	}
    u4 u = (u2) (UPK_readU2(f));
	return (((u2) UPK_readU1(f)) << 16) + u;
}

//reads 4 unsigned bytes from file-buffer
u4 UPK_readU4(UPK_FILE* f){
    if(isBigEndian()){
		u4 u = ((u2) UPK_readU2(f)) << 16;
		return u + UPK_readU2(f);
	}
    u4 u = (u2) (UPK_readU2(f));
	return (((u2) UPK_readU2(f)) << 16) + u;
}

//ensure that we don't have an buffer-read out of bounds
u1 UPK_ensurePosition(UPK_FILE* f){
    if(f->pos > f->length){
        printf("File read out of bounds!\n");
		return 0;
    }
	return 1;
}

//function to get the file extension from a give file-name
const char* UPK_getFExt(char* f){
    char* dot = strrchr(f, '.');
    if(!dot || dot == f){
        return "";
    }
    return dot + 1;
}

//to check if the current system has a big or little endian format
u1 isBigEndian(void){
	union{
		unsigned short s;
		unsigned char c[2];
	} end;
	
	end.s = 0x8000;
	if(end.c[0]){
		return 1;
	}
	return 0;
}

//used to create all necessary directories when extracting
u1 createDirsRecursiv(char* fName){
	//prepare first 'slash'
	char* slash = strchr(fName, UPK_DIRSEP);
	
	while(slash != fName && slash != NULL){		
		//store slash and set it to NUL in order to get the right directory
		char temp = *slash;
		*slash = '\0';
		
		//create dir and return ERROR (0) if an error happened
		// and the error wasn't an 'already existing'
		if(MKDIR(fName) != 0){
			if(errno != EEXIST){
				*slash = temp;
				return 0;
			}
		}
		
		//reset slash and restart with next slash
		*slash = temp;
		slash = strchr(slash + 1, UPK_DIRSEP);
	}
	return 1;
}