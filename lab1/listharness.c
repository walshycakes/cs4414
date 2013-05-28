#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "list.h"

#define MAX_CHARS 40

void visitor_string(void *v);
int compare_string(const void *key, const void *with);
void datum_delete_string(void *v);

int main (int argc, char* argv[]) {
	if (argc != 3) {
		perror("Usage: ./a.out TEXTFILE COMMAND");
		exit(-1);
	}

	char* filename = argv[1];
	char* command = argv[2];

	list_t* theList = (list_t*) malloc(sizeof(list_t));
	list_init(theList, compare_string, datum_delete_string);

	FILE* fp = fopen(filename, "r");
	if (fp != NULL) {
		char buffer[MAX_CHARS];

		while (fgets(buffer, MAX_CHARS+1, fp) != NULL) {
			if (strcmp(command, "echo") == 0) {
				printf("%s", buffer);	
			}
			else {
				
				char* datum = (char*) malloc((strlen(buffer)+1) * sizeof(char));
				strcpy(datum, buffer);
				
				if (strcmp(command, "tail") == 0) {
					list_insert_tail(theList, (void*) datum);
				}
				else if (strcmp(command, "sort") == 0) {
					list_insert_sorted(theList, (void*) datum);
				}
				else if (strcmp(command, "tail-remove") == 0) {
				
					list_insert_tail(theList, (void*) datum);
				}
				else {
					perror("Invalid command. Possible choices: echo tail sort tail-remove");
					exit(-1);
				}
			}
		}
		
		if (strcmp(command, "tail-remove") == 0){
			int i;
			int theLength = theList->length;
			for (i = 0; i < theLength; ++i) {
				if (i == 0) {
					printf("-----\tFull list\n");
					list_visit_items(theList,visitor_string);
				}
				else if ((i % 3 == 0)) {
					printf("-----\tRemoved 3\n");
					list_visit_items(theList, visitor_string);
				}
				list_remove_head(theList);
			}
			printf("-----\tList empty\n");
		}

		else if (strcmp(command, "echo") != 0) {
			list_visit_items(theList, visitor_string);
			list_visit_items(theList, datum_delete_string);
		}

		free(theList);
	}
	return 0;
}

// v is a list_item_t
void visitor_string(void* v) {
	char* datum = ((list_item_t*) v)->datum;
	printf("%s", datum);
}

// key, with are both list_item_t
int compare_string(const void *key, const void *with) {
	char* left = ((list_item_t*) key)->datum;
	char* right = ((list_item_t*) with)->datum;
	
	return strcmp(left, right);
}

// v is a list_item_t
void datum_delete_string(void *v) {
	/* 4 cases
		1. pred, next are null, null (no action)
		2. pred, next are null, exists
		3. pred, next are exists, null
		4. pred, next are exists, exists
	*/

	// grab the node and its adjacent nodes
	list_item_t* theNode = (list_item_t*) v;
	list_item_t* pred = theNode->pred;
	list_item_t* next = theNode->next;

	// case 1 - no pointers need rearrange

	// case 2
	if (pred == NULL && next != NULL) {
		next->pred = theNode->pred;
	}

	// case 3
	else if (pred != NULL && next == NULL) {
		pred->next = theNode->next;
	}

	// case 4
	else if (pred != NULL && next != NULL) {
		pred->next = theNode->next;
		next->pred = theNode->pred;
	}

	// free datum and free node
	free(theNode->datum);
	free(theNode);
}
