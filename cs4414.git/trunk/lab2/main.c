
/*------------------------------
CS 4414 > Lab2 > Shell
Jared Culp (jjc4fb)
Bryan Walsh (bpw7xx)

Submitted: 2/8/2013
------------------------------*/


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
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
/* global flag for wildcard handling */
unsigned int has_wildcard = 0;
/* global placebolder for wildcard expression */
char *wildcard_expr = NULL;
/* keep track of the number of tokens */
unsigned int argv_count = 0;

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

	argv_count = n;
	return argv;
}

int main(int argc, char** argv) {
	pid_t pid;
	glob_t globbuf;

	while (1) {
		printf("$");

		char buf[MAX_LINE];
		char **argvv = tokenizeInput(buf);

		if (strcmp(argvv[0], "exit") == 0) {
			kill(pid, SIGKILL);
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
				globbuf.gl_offs = argv_count - 1;

				// the GLOB_DOOFFS allocates gl_offs for us to merge
				glob(wildcard_expr, GLOB_DOOFFS, NULL, &globbuf);

				// merge the existing argvv array
				int i;
				for (i = 0; i < argv_count - 1; ++i) {
					globbuf.gl_pathv[i] = argvv[i];
				}
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
