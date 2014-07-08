/* 
   Alexander Schnackenberg
   AS766116    
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "constants.h"
#include "struct_def.h"
#include "prototypes.h"

/*creates the head lnode of a list*/
lnode *create_list(char *fname, size_t size_of_file, unsigned char size_of_file_name){
	lnode *head;
	
	if ((head = (lnode *)malloc(sizeof(lnode))) == NULL){
		printf("List node allocation failed. \n"); exit(1); //stop program
	}
	
	strcpy(head->file_name, fname);
	head->size_of_file = size_of_file;
	head->size_of_file_name = size_of_file_name;
	head->next = NULL;
	
	return head;
}

/*inserts into list*/
void list_insert(lnode **head, char *fname, unsigned int size_of_file, unsigned char size_of_file_name){
	if(*head == NULL){
		*head = create_list(fname, size_of_file, size_of_file_name);
	}
	else{
		list_insert(&((*head)->next), fname, size_of_file, size_of_file_name);
	}
}

/*print list to stdout*/
void print_list(lnode *head){
	if(head==NULL){
		return;
	}
	else{
		printf("%s %u \n", head->file_name, head->size_of_file);
	}
	print_list(head->next);
}