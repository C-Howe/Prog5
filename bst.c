/* 
   Alexander Schnackenberg
   AS766116    
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "constants.h"
#include "struct_def.h"
#include "prototypes.h"

/*creates the root node of a tree*/
node *create_tree(off_t logical_size, time_t last_mod, char *fName){
	node *root;

	if ((root = (node *)malloc(sizeof(node))) == NULL){
		printf("Node allocation failed. \n"); exit(1); //stop program
	}
	
	root->logical_size = logical_size;
	root->last_mod = last_mod;
	strcpy(root->file_name, fName);
	root->left = root->right = NULL;
	return root;
}

/*inserts into bst in sorted order. no rebalancing of tree*/
void bst_insert(node **head, off_t logical_size, time_t last_mod, char *fName){
	if(*head == NULL){
		*head = create_tree(logical_size, last_mod, fName);
	}
	else{
		if((*head)->logical_size < logical_size){
			bst_insert(&((*head)->right), logical_size, last_mod, fName);
		}
		else{
			bst_insert(&((*head)->left), logical_size, last_mod, fName);
		}
	}
}

/*prints bst in non-decreasing order to stdout
assumes head is not null*/
void print_bst(node *head){
	struct tm *tp;
	char* month[NUM_MONTHS]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	if(head->left == NULL && head -> right==NULL){
		//we have reached a leaf node, print it out
		tp = localtime(&(head->last_mod));
		printf("%s %llu %s %d %d \n",head->file_name, (long long unsigned)(head->logical_size), month[(tp->tm_mon)], (tp->tm_mday), (tp->tm_year)+ CORRECT_YEAR);
		return;
	}
	else{
		//print the left subtree. print the current node. print the right subtree
		if(head->left != NULL){
			print_bst(head->left);
		}
		tp = localtime(&(head->last_mod));
		printf("%s %llu %s %d %d \n",head->file_name, (long long unsigned)(head->logical_size), month[(tp->tm_mon)], (tp->tm_mday), (tp->tm_year)+ CORRECT_YEAR);
		if(head->right != NULL){
			print_bst(head->right);
		}
		return;
	}
}
