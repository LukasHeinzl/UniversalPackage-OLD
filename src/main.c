#include <upk.h>
#include <IO.h>
#include <stdio.h>
#include <string.h>

//main.c function prototypes
void showHelp(void);

//main.c functions
int main(int argc, char** argv){
	u1 op;			//specifies the operation to do
	if(!checkArgs(argc, argv, &op)){	//check the arguments for correctness and operation
		printf("Use 'upk help' to show help!");
		return -1;
	}
	
	//decide what to do
	switch(op){
		case UPK_HELP:
			showHelp();
			break;
		case UPK_LIST:
			listContent(argv[2]);
			break;
		case UPK_EXTR:
			extractContent(argv[2], argv[3]);
			break;
		case UPK_PACK:
			if(strcmp(UPK_getFExt(argv[2]), "upk")){
				strcat(argv[2], ".upk");
			}
			packUPK(argc - 3, argv + 2);
			break;
		}
	
	return 0;
}

//only for better readabillity
void showHelp(void){
	printf("Universal Packager [Version 1.0] - Help:\n");
	printf("\tupk help\t\t\t\tTo show this help page\n");
	printf("\tupk list <.upk>\t\t\t\tTo list the contents of the given .upk file\n");
	printf("\tupk extract <toStore> <.upk>\t\tTo extract everthing from the given .upk file into to given folder\n");
	printf("\tupk pack <.upk> <file1> [files..]\tTo create a .upk file from the given files\n");
}