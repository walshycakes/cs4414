#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <glob.h>
#include <assert.h>

/* we assume max chars entered is 80 */
#define MAX_LINE 80
/* global flag for background/foreground process */
unsigned int is_fg = 1;
/* global flags for redirecting stdin, stdout */
unsigned int is_redirect_out = 0;
unsigned int is_redirect_in = 0;
/* global placeholder for redirection path */
char *redirect_stream = NULL;
/* global flag for wildcard handling */
unsigned int has_wildcard = 0;
/* global placebolder for wildcard expression */
char *wildcard_expr = NULL;

glob_t globbuf;

char** tokenizeInput(char *buf) {
	char **argv = (char**) calloc(1, sizeof(char*));
	fgets(buf, MAX_LINE+1, stdin);

	// set all global flags
	is_fg = (strchr(buf, '&') != NULL) ? 0 : 1;
	is_redirect_out = (strchr(buf, '>') != NULL) ? 1 : 0;
	is_redirect_in = (strchr(buf, '<') != NULL) ? 1 : 0;
	has_wildcard = (strchr(buf, '*') != NULL) ? 1 : 0;

	char *tokens = strtok(buf, " \n");
	int n = 0;
	while (tokens) {
		// dynamically allocate space for this param
		argv[n] = (char*) calloc(strlen(tokens), sizeof(char));
		
		// if redirect pipe, store redirect stream and advance tokens
		if (strcmp(tokens, ">") == 0 || strcmp(tokens, "<") == 0) {
			redirect_stream = strtok(NULL, " \n");
			tokens = strtok(NULL, " \n");
		}
		else if (strchr(tokens, '*') != NULL) {
			wildcard_expr = tokens;
			tokens = strtok(NULL, " \n");
			tokens = strtok(NULL, " \n");
		}
		argv[n] = tokens;
		++n;

		// reallocate first index of argv to hold another param
		argv = (char**) realloc(argv, ((n+1) * sizeof(char*)));
		tokens = strtok(NULL, " \n");
	}

	return argv;
}

int main(int argc, char** argv) {
	// fd[0] input pipe, fd[1] output pipe
	int fd[2];
	pipe(fd);

	pid_t pid;

	while (1) {
		printf("$");

		char buf[MAX_LINE];
		char **argvv = tokenizeInput(buf);

		if (strcmp(argvv[0], "exit") == 0) {
			exit(1);
		}

		pid = fork();

		if (pid < 0) {
			fprintf(stderr, "Fork failed");
			exit(1);
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
				exit(1);
			}
			else if (is_redirect_out) {
				stdout_save = dup(STDOUT_FILENO);
				freopen(redirect_stream, "w", stdout);
				execvp(argvv[0], argvv);
				dup2(stdout_save, STDOUT_FILENO);
				exit(1);
			}
			else if (has_wildcard) {
				// usage as detailed in `man glob`
				glob(wildcard_expr, 0, NULL, &globbuf);
				int match_count = globbuf.gl_pathc;
				execvp(argvv[0], globbuf.gl_pathv);
				exit(1);
			}
			else {
				execvp(argvv[0], argvv);
				exit(1);
			}
		}
		else { // parent
			if (is_fg) {
				wait(NULL);
			}
		}
	}
	
	return 0;
}