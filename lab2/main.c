#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

/* we assume max chars entered is 80 */
#define MAX_LINE 80
/* global flag for background/foreground process */
unsigned int is_fg = 1;
/* global flags for redirecting stdin, stdout */
unsigned int is_redirect_out = 0;
unsigned int is_redirect_in = 0;
/* global placeholder for redirection path */
char *redirect_stream = NULL;

char** tokenizeInput(char *buf) {
	int n = 0;
	char **argv = (char**) calloc(1, sizeof(char*));
	
	fgets(buf, MAX_LINE+1, stdin);

	// if the buffer contains an '&', lower is_fg flag
	if (strchr(buf, '&') != NULL) {
		is_fg = 0;
	}

	// determine is stdout or stdin need to be redirected
	is_redirect_out = (strchr(buf, '>') != NULL) ? 1 : 0;
	is_redirect_in = (strchr(buf, '<') != NULL) ? 1 : 0;

	char *tokens = strtok(buf, " \n");

	while (tokens) {
		// dynamically allocate space for this param
		argv[n] = (char*) calloc(strlen(tokens), sizeof(char));
		
		// once we hit a redirect pipe, stop building argv
		if (strcmp(tokens, ">") == 0 || strcmp(tokens, "<") == 0) {
			argv[n] = NULL;
			redirect_stream = strtok(NULL, " \n");
			break;
		}
		argv[n] = tokens;
		++n;

		// reallocate first index of argv to hold another param
		argv = (char**) realloc(argv, ((n+1) * sizeof(char*)));
		tokens = strtok(NULL, " \n");
	}

	return argv;
}

void redirectStream(char** argv) {
	// TODO
}

void restoreStream() {
	// TODO
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
			return 0;
		}
		else {
			pid = fork();

			if (pid < 0) {
				fprintf(stderr, "Fork failed");
				return 1;
			}
			else if (pid == 0) { // child
				// backup stdout, stdin
				int stdin_save, stdout_save;

				// exec, redirecting streams as necessary
				if (is_redirect_in) {
					stdin_save = dup(STDIN_FILENO);
					freopen(redirect_stream, "r", stdin);
					execvp(argvv[0], argvv);
					dup2(stdin_save, STDIN_FILENO);
				}
				else if (is_redirect_out) {
					stdout_save = dup(STDOUT_FILENO);
					freopen(redirect_stream, "w", stdout);
					execvp(argvv[0], argvv);
					dup2(stdout_save, STDOUT_FILENO);
				}
				else {
					execvp(argvv[0], argvv);
				}
			}
			else { // parent
				if (is_fg) {
					wait(NULL);
				}
			}
		}
	}
	
	return 0;
}