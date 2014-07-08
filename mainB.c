#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "constants.h"
#include "struct_def.h"
#include "prototypes.h"

int main(int argc, char *argv[]){
	struct stat statStruct;
	int output;
	int input;
	int i = 0;
	unsigned char N;
	unsigned char L;
	mode_t mode = MODE;
	int ret;
	size_t buf_size;
	ssize_t nread;
	char *buffer;
	char filepath[FSIZE_BUFFER];
	char fpath[FSIZE_BUFFER];
	char file_name[MAX_FILE_NAME];
	unsigned char size_of_file_name;
	unsigned int size_of_file;
	unsigned int sizef;
	lnode *head = NULL;
	
	if(getcwd(filepath, buf_size)==NULL){
		fprintf(stderr,"Error getting current working directory \n");exit(1);
	}
	strcat(filepath, "/");
	strcpy(fpath, filepath);

	if(strcmp("-c", argv[BFLAG])==0){
	/*
		Create the archive, which consists of N input files
		Then, after writing 'N' to the archive, we write the 
		file information for each of the input files, and then
		write each of the input files to the archive		  
	*/
		buf_size = FSIZE_BUFFER;	
		strcat(filepath, argv[ARCHIVE1]);
		output = creat(filepath, mode);
		if (output==ERROR){
			fprintf(stderr,"Error opening output file \n");exit(1);
		}
		N = (unsigned char)(argc-(ARCHIVE1+1));
		write(output, &N, sizeof(unsigned char));
			
		/*
			Writing file information to archive:
			Format for each input file: L,S,Z
				where L is the length of the file name
					  S is the string containing the file name
					  Z is the logical size of the file (which can be a maximum of (2^32-1) bytes, ie, 4 GB	
		*/
		for(i =FILE1;i<argc; i++){
			input = open(argv[i], O_RDONLY);
			if (input==ERROR){
				printf("Error opening input file \n");
			}
			else{
				ret = fstat(input, &statStruct);
				if (ret == ERROR){
					printf("Could not obtain stat information for input file: %s\n", strerror(errno));fflush(stdout);
				}
				buffer = (char*)malloc(strlen(argv[i])); //create a buffer to store S
				sprintf(buffer,"%c",(unsigned char)strlen(argv[i]));
				write(output, &L, sizeof(unsigned char)); //write L 
				write(output, argv[i], strlen(argv[i])); //write S
				write(output,&statStruct.st_size,sizeof(unsigned int));//write Z
				free(buffer);		
			}
			if(close(input)==ERROR){
				printf("Error closing input file \n");
			}
		}	
		/*
			Writing the input files to the archive
				Reads in buffer[FSIZE_BUFFER] of bytes at a time
				and writes buffer to to archive				
		*/
		buffer = (char*)malloc(FSIZE_BUFFER * sizeof(char));
		for(i =FILE1;i<argc; i++){
			input = open(argv[i], O_RDONLY);
			if (input==ERROR){
				printf("Error opening input file \n");
			}
			else{
				ret = fstat(input, &statStruct);
				if (ret == ERROR){
					printf("Could not obtain stat information for input file: %s\n", strerror(errno));
				}
				while((nread = read(input, buffer, (size_t)(FSIZE_BUFFER)))>0){ //read the entire input file, store in string buffer
					if (write(output, buffer, nread) < nread){	//write input file to archive
						close(input);close(output);unlink(argv[BFLAG]);					
					}									
				}
				if(close(input)==ERROR){
					printf("Error closing input file \n");
				}
			}		
		}
		free(buffer);
		free(head);
	}
	
	
	else if(strcmp("-x", argv[BFLAG])==0){
		/*
			First, get all of the file information, and store that in a linked list
			First, get all of the file information, and store that in a linked list
			Then, using that linked list, extract and create files
		*/
		strcat(filepath, argv[ARCHIVE1]);
		if((input = open(filepath, O_RDONLY))==ERROR){fprintf(stderr,"Error opening input file \n");}
		read(input, &N, sizeof(unsigned char));
		for(i = 0;i<(int)N; i++){
			read(input, &size_of_file_name, sizeof(unsigned char));
			read(input, file_name, (size_t)(size_of_file_name));
			file_name[size_of_file_name+1] = '\0';
			read(input, &size_of_file, sizeof(unsigned int));
			list_insert(&head, file_name, size_of_file, size_of_file_name);
		}
		/*
			Writing the archive to the new files
				Reads in buffer[FSIZE_BUFFER] of bytes at a time
				and writes buffer to output file 				
		*/
		buffer = (char*)malloc(FSIZE_BUFFER * sizeof(char));
		for(i = 0;i<(int) N; i++){
			sizef = (head)->size_of_file;
			strcpy(filepath, fpath);
			strcat(filepath, (head)->file_name);
			if((output = creat(filepath, mode))==ERROR){printf("Error opening output file. %s\n",strerror(errno));}
			while(sizef>FSIZE_BUFFER){ //read the entire input file, store in string buffer
				nread = read(input, buffer, (size_t)(FSIZE_BUFFER));
				if (write(output, buffer, nread) < nread){	//write input file to archive
					close(input);close(output);unlink(argv[ARCHIVE1]);					
				}				
				sizef-= nread;
			}
			nread = read(input, buffer, (size_t)(sizef));
			if (write(output, buffer, nread) < nread){	//write input file to archive
				close(input);close(output);unlink(filepath);					
			}
			head = (head)->next;
		}
		free(buffer);
		free(head);
	}


	else if(strcmp("-p", argv[BFLAG])==0){
	/*
	 search archive for prefix
	*/
		strcat(filepath, argv[FILE1]);
		if((input = open(filepath, O_RDONLY))==ERROR){fprintf(stderr,"Error opening input file \n");}
		read(input, &N, sizeof(unsigned char));
		for(i = 0;i<(int)N; i++){
			read(input, &size_of_file_name, sizeof(unsigned char));
			read(input, file_name, (size_t)(size_of_file_name));
			file_name[size_of_file_name+1] = '\0';
			read(input, &size_of_file, sizeof(unsigned int));
			if(strncmp(argv[ARCHIVE1], file_name, strlen(argv[ARCHIVE1]))==0){
				list_insert(&head, file_name, size_of_file, size_of_file_name);
			}
		}
		if(head==NULL){
			printf("No matching files found. \n");
		}
		print_list(head);
		free(head);
	}
	else{
		printf("argv[BFLAG=1]: %s strcmp(\"-c\", argv[BFLAG]): %d\n", argv[BFLAG], strcmp("-c", argv[BFLAG]));		
		fprintf(stderr, "The only acceptable flags are -c, -x, and -p. Terminating execution. \n"); exit(1);
	}
	return 0;
}
