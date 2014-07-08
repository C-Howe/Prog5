/* 
   Alexander Schnackenberg
   AS766116    
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include "constants.h"
#include "struct_def.h"
#include "prototypes.h"

int main(int argc, char *argv[]){
	struct stat statStruct;
	DIR *directory;
	struct dirent *entry;
	int return_val = 0;
	node *head = NULL;
	char filepath[MAX_FILE_NAME];	
		
	//p5a takes 1 or 2 arguments
	//if it p5a has 2 arguments, the second MUST be "-s"
	if (argc != P5A_NUMARGS1 && argc != P5A_NUMARGS2){
		//bad stuff should happen
		fprintf(stderr, "Wrong number of arguments to p5a. Terminating execution. \n"); exit(1);
	}
	else{
		if (argc == P5A_NUMARGS2 && strcmp("-s",argv[P5A_FLAG])!=0){
			//more bad stuff happens
			fprintf(stderr, "The only acceptable flag is -s. Terminating execution. \n"); exit(1);
		}
	}
	//since the directions state that we may print out the file name, size and time in ANY order
	//without the -s flag, I will sort everything by default (sorting as I add to the bst)
	if((directory = opendir(argv[DIRECTORY_PATH]))==NULL){
		fprintf(stderr, "Could not open directory %s. Terminating execution. \n", argv[DIRECTORY_PATH]); exit(1);
	}
	while((entry=readdir(directory))){
		if((entry->d_ino)!=0){
			if((entry->d_name)[0] == '.'){
				//get stat struct for directory->d_name
				if(strcmp(".", (entry->d_name))!=0 && strcmp("..", (entry->d_name))!=0){
					strcpy(filepath, argv[DIRECTORY_PATH]); strcat(filepath, "/"); strcat(filepath, entry->d_name);
					return_val = 	stat(filepath, &statStruct);			
					if(return_val == ERROR){printf("Could not obtain stat information for %s . %s\n",filepath, strerror(errno));}
					else{
						bst_insert(&head, (statStruct.st_size), (statStruct.st_mtime), (entry->d_name));
					}
				}
			}
		}
		return_val = 0;
	}
	print_bst(head);
	return 0;
}
