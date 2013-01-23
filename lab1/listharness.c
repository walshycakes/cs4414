#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "list.h"

#define MAX_CHARS 40

void visitor_string(void *v);

int main (int argc, char* argv[]) {
	if (argc != 3) {
		perror("Usage: ./a.out TEXTFILE COMMAND");
		exit(-1);
	}

	char* filename = argv[1];
	char* command = argv[2];
	void (*visitor)(void* v);
	visitor = &visitor_string;
	list_t* theList = (list_t*) malloc(sizeof(list_t));

	FILE* fp = fopen(filename, "r");
	if (fp != NULL) {
		char buffer[MAX_CHARS];
		while (fgets(buffer, MAX_CHARS, fp) != NULL) {
			if (strcmp(command, "echo") == 0) {
				//printf("%s", buffer);
				
				list_visit_items(theList, visitor);
				
			}
			else {
				char* datum = (char*) malloc((strlen(buffer)+1) * sizeof(char));
				strcpy(datum, buffer);

				if (strcmp(command, "sort") == 0) {
				}
				else if (strcmp(command, "tail") == 0) {
					list_insert_tail(theList, (void*) datum);
				}
				else if (strcmp(command, "tail-remove") == 0) {
				}
				else {
					perror("Invalid command. Must be echo, sort, tail, tail-remove");
					exit(-1);
				}
			}
		}
	}

	list_item_t* head = theList->head;
	int i = 0;
	while(head->next != NULL) {
		printf("node # : %d data : %s\n", i, (char*) head->datum);
		++i;
		head = head->next;
	}
	
	return 0;
}

//asumes we're passing datum to be compared between 2 nodes
int compare(const void *key, const void *with){
	
	char* left = (char*) key;
	char* right = (char*) with;
	
	return strcmp(left,right);

}

void visitor_string(void* v){

	list_item_t* x = (list_item_t*) v;
	
	printf("%s\n",(char*) x->datum);
		

}






















