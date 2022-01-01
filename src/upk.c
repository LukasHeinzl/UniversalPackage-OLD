#include <upk.h>
#include <IO.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//used to dertermain what to do
u1 checkArgs(int argc, char** argv, u1* op){
	//2: upk <option>
	if(argc < 2){
		printf("No input files and/or operation\n");
		return 0;
	}
	
	if(!strcmp(argv[1], "pack")){
		if(argc < 4){
			printf("too few arguments, needs at least: upk pack <output> <file1>\n");
			return 0;
		}
		*op = UPK_PACK;
	}else if(!strcmp(argv[1], "list")){
		if(argc != 3){
			printf("wrong argument count, needs: upk list <.upk>\n");
			return 0;
		}
		*op = UPK_LIST;
	}else if(!strcmp(argv[1], "extract")){
		if(argc != 4){
			printf("wrong argument count, needs: upk extract <.upk>\n");
			return 0;
		}
		*op = UPK_EXTR;
	}else if(!strcmp(argv[1], "help")){
		if(argc != 2){
			printf("unnecessary arguments, only needs: upk help\n");
			return 0;
		}
		*op = UPK_HELP;
	}
	return 1;
}

//printf the content of the given .upk file to the console
void listContent(char* fName){
	if(!strcmp(UPK_getFExt(fName), ".upk")){
		printf("Input file must have a .upk extension, got: %s!\n", fName);
		return;
	}
	
	UPK_FILE f = UPK_loadFile(fName);
	if(f.length == 0){
		return;
	}
	
	//read in entry count (only 3 bytes in .upk file)
	u4 totalEntryCount = UPK_readU3(&f);
	
	//print table-header
	printf("\n\n%-13s|%-12s|%s\n", "Name Length", "File Size", "File Name");
	printf("-------------------------------------------------------------\n");
	
	//read the whole file-table and print it
	u4 i;
	for(i = 0; i < totalEntryCount; i++){
		//read name length
		u2 fileNameLength = UPK_readU2(&f);
		
		//read name
		char fileName[257];
		u2 j;
		for(j = 0; j < fileNameLength; j++){
			fileName[j] = (char) UPK_readU1(&f);
		}
		fileName[j] = '\0';
		
		//read file size and...
		u4 fileSize = UPK_readU4(&f);
		
		f.pos += fileSize; //... skip file content
		
		//print entry
		printf("%-13d", fileNameLength);
		printf("|%-12d", fileSize);
		printf("|%s\n", fileName);
	}
	
	//clean
	UPK_freeFile(&f);
}

//extracts the content from the given .upk file
void extractContent(char* toStore, char* toExtract){
	//open .upk file
	UPK_FILE f = UPK_loadFile(toExtract);
	if(f.length == 0){
		return;
	}
	
	//create directory to store the .upk's content
	if(MKDIR(toStore) != 0){
		if(errno != EEXIST){
			//didn't work: terminate
			printf("Could not create folder %s!", toStore);
			UPK_freeFile(&f);
			return;
		}
	}
	
	//store length of toStore
	size_t len = strlen(toStore);
	
	//read totalFileCount from .upk file
	u4 totalEntryCount = UPK_readU3(&f);
	u4 i;
	for(i = 0; i < totalEntryCount; i++){
		//read name length
		u2 nameLength = UPK_readU2(&f);
		
		//allocate memory for file name + toStore + / + NUL
		char* fileName = (char*) malloc(nameLength + len + 2);
		
		//store toStore
		sprintf(fileName, "%s%c", toStore, UPK_DIRSEP);
		
		//read name
		u4 j;
		for(j = 0; j < nameLength; j++){
			fileName[j + len + 1] = (char) UPK_readU1(&f);
		}
		fileName[j + len + 1] = '\0';
		
		//create any necessary directories for this file
		if(createDirsRecursiv(fileName) == 0){
			//error while creating: terminating
			printf("Could not create %s", fileName);
		}
		
		//get file size
		u4 fileSize = UPK_readU4(&f);
		
		//open file to write into
		FILE* fp = fopen(fileName, "w");
		
		u1 tmp;
		for(j = 0; j < fileSize; j++){
			tmp = UPK_readU1(&f);
			fwrite(&tmp, sizeof(u1), 1, fp);
		}
		
		//clean
		fclose(fp);
		free(fileName);
	}
	
	//clean
	UPK_freeFile(&f);
}

//packs the given files and folders into an .upk file
void packUPK(u4 fCount, char** fNames){
	//open file
	FILE* upkFile = fopen(fNames[0], "wb");
	
	//write entry count to .upk file
	fwrite(&fCount, 1, 3, upkFile);
	
	//loop through files and write them to .upk file
	u4 i;
	u4 tempVal;
	for(i = 0; i < fCount; i++){		
		//write file name length (i + 1 because 0 is .upk name)
		tempVal = strlen(fNames[i + 1]);
		fwrite(&tempVal, sizeof(u2), 1, upkFile);
		
		//write filename (tempVal still holds name length)
		fwrite(fNames[i + 1], sizeof(u1), tempVal, upkFile);
		
		//open file to store and write file size
		FILE* file = fopen(fNames[i + 1], "rb");
		fseek(file, 0, SEEK_END);
		tempVal = ftell(file);
		rewind(file);
		fwrite(&tempVal, 1, 4, upkFile);
		
		//write file contents (tempVal still holds fileSize)
		u4 j;
		u1 tmp;
		for(j = 0; j < tempVal; j++){
			if(!fread(&tmp, sizeof(u1), 1, file)){
				break;
			}
			fwrite(&tmp, sizeof(u1), 1, upkFile);
		}
		
		//clean
		fclose(file);
	}
	
	//clean
	fclose(upkFile);
}