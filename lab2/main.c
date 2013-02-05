#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80

char** tokenizeInput(char *buf) {
	int n = 0;
	char **argv = (char**) calloc(1, sizeof(char*));
	
	fgets(buf, MAX_LINE+1, stdin);
	char *tokens = strtok(buf, " \n");

	while (tokens) {
		argv[n] = calloc(strlen(tokens), sizeof(char));
		argv[n] = tokens;
		++n;
		argv = (char**) realloc(argv, ((n+1) * sizeof(char*)));
		tokens = strtok(NULL, " \n");
	}

	return argv;
}

int main(int argc, char** argv) {
	pid_t pid;
	char command[MAX_LINE];

	int is_true = 1;

	while (is_true) {
		printf("$");

		char buf[MAX_LINE];
		char **argvv = tokenizeInput(buf);

		if (strcmp(argvv[0], "exit") == 0) {
			is_true = 0;
		}

	
		else {
			pid = fork();

			if (pid < 0) {
				fprintf(stderr, "Fork failed");
				return 1;
			}
			else if (pid == 0) { // child
				// printf("[child] pid: %d\n", getpid());
				// printf("[child] parent pid: %d\n", getppid());
				execvp(argvv[0], argvv);
			}
			else { // parent
				// printf("[parent]: pid = %d\n", pid);
				// printf("[parent] get ppid: %d\n", getppid());
				wait(NULL);
			}
		}
	}

	return 0;
}