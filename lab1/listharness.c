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

				list_insert_tail(theList, (void*) datum);
			}
		}
		if (notEcho) {
			list_visit_items(theList, visitor_string);
		}
	}

	return 0;
}

void visitor_string(void* v) {
	printf("%s",(char*) v);
}

int compare_string(const void *key, const void *with){
	char* left = (char*) key;
	char* right = (char*) with;
	
	return strcmp(left, right);
}

void datum_delete_string(void *v) {
}