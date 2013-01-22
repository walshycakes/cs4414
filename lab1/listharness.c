#include <stdio.h>
#include <errno.h>
#include <list.h>
#define MAX_CHARS 40

char* readFile(const char* filename);

int main (int argc, char* argv[]) {
	if (argc != 3) {
		perror("Usage: ./a.out TEXTFILE COMMAND");
		exit(-1);
	}

	char* fileContent = readFile(argv[1]);
	
	char* input = argv[2];
	if (strcmp(input, "echo") == 0) {
		printf("echo");
	}
	else if (strcmp(input, "sort") == 0) {
		printf("sort");
	}
	else if (strcmp(input, "tail") == 0) {
		printf("tail");
	}
	else if (strcmp(input, "tail-remove") == 0) {
		printf("tail-remove");
	}
	else {
		perror("Invalid command. Must be echo, sort, tail, tail-remove");
		exit(-1);
	}
	
	return 0;
}

char* readFile(const char* filename) {
	// first pass, determine # lines
	FILE *fp = fopen(filename, "r");
	int i = 0, lines = 0;
	if (fp != NULL) {
		char buffer[MAX_CHARS];
		while (fgets(buffer, MAX_CHARS, fp) != NULL) {
			++lines;
		}
	}
	fclose(fp);

	// second pass, read in data
	fp = fopen(filename, "r");
	char fileContent[lines][MAX_CHARS];
	if (fp != NULL) {
		char buffer[MAX_CHARS];
		while (fgets(buffer, MAX_CHARS, fp) != NULL) {
			++i;
		}
	}

	return fileContent;
}















