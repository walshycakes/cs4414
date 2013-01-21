#include <stdio.h>
#define MAX_CHARS 40

char* readFile(const char* filename);

int main (int argc, char* argv[]) {
	if (argc != 3) {
		printf("Error. Invalid Input.");
	}

	char* fileContent = readFile(argv[1]);

	char* input = argv[2];

	if (strcmp(input, "echo") == 0) {

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
	else{
		printf("Invalid command supplied.");
	}
	

	return 0;

}

char* readFile(const char* filename) {
	FILE *fp = fopen(filename, "r");
	int lines = 0, i = 0;
	if (fp != NULL) {
		char buffer[MAX_CHARS];
		while (fgets(buffer, MAX_CHARS, fp) != NULL) {
			++lines;
		}
	}
	fclose(fp);

	fp = fopen(filename, "r");
	char fileContent[lines][MAX_CHARS];
	if (fp != NULL) {
		char buffer[MAX_CHARS];
		while (fgets(buffer, MAX_CHARS, fp) != NULL) {
			//fileContent[i] = buffer;
			++i;
		}
	}

	return fileContent;
}















