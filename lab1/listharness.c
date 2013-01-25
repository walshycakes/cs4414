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

	int notEcho = 0;

	FILE* fp = fopen(filename, "r");
	if (fp != NULL) {
		char buffer[MAX_CHARS];
		while (fgets(buffer, MAX_CHARS+1, fp) != NULL) {
			if (strcmp(command, "echo") == 0) {
				printf("%s", buffer);	
			}
			else {
				notEcho = 1;
				char* datum = (char*) malloc((strlen(buffer)+1) * sizeof(char));
				strcpy(datum, buffer);
				
				if (strcmp(command, "tail") == 0) {
					list_insert_tail(theList, (void*) datum);
				}
				else if (strcmp(command, "sort") == 0) {
					list_insert_sorted(theList, (void*) datum);
				}
				else {
					printf("blargh");
				}
			}
		}
		if (notEcho) {
			list_visit_items(theList, visitor_string);
		}
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
	// grab the node and its adjacent nods
	list_item_t* theNode = (list_item_t*) v;
	list_item_t* pred = theNode->pred;
	list_item_t* next = theNode->next;

	// case pred, next is null, null
	// no pointers need rearranging

	// case pred, next is null, exists
	if (pred == NULL && next != NULL) {
		next->pred = theNode->pred;
	}

	// case pred, next is exists, null
	else if (pred != NULL && next == NULL) {
		pred->next = theNode->next;
	}

	else if (pred != NULL && next != NULL) {
		pred->next = theNode->next;
		next->pred = theNode->pred;
	}

	// free datum and free node
	free(theNode->datum);
	free(theNode);
}