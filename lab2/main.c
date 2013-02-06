#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_LINE 80
unsigned int is_fg = 1;


char** tokenizeInput(char *buf) {
	int n = 0;
	char **argv = (char**) calloc(1, sizeof(char*));
	
	fgets(buf, MAX_LINE+1, stdin);

	if (strchr(buf, '&') != NULL) {
		is_fg = 0;
	}

	char *tokens = strtok(buf, " \n");

	while (tokens) {
		if (strchr(tokens, '>') != NULL) {
			tokens = strtok(NULL, " \n");
			FILE *fp = freopen(tokens, "w", stdout);
			break;
		}
		if (strchr(tokens, '<') != NULL) {
			tokens = strtok(NULL, " \n");
			freopen(tokens, "r", stdin);
			break;
		}
		
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
			kill(pid, SIGTERM);
		}
		else {
			pid = fork();

			if (pid < 0) {
				fprintf(stderr, "Fork failed");
				return 1;
			}
			else if (pid == 0) { // child
				execvp(argvv[0], argvv);
			}
			else { // parent
				if (is_fg){
					wait(NULL);
				}
			}
		}
	}
	
	return 0;
}