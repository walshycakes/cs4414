#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

int main(int argc, char** argv) {
	char *args[MAX_LINE/2 + 1];
	int should_run = 1;

	while (should_run) {
		printf("shell>");
		fflush(stdout);
	}

	return 0;
}