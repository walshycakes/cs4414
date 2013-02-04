#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80

int main(int argc, char** argv) {
	pid_t pid;
	char command[MAX_LINE];

	int is_true = 1;

	printf("pid of our shell: %d\n", getpid());
	printf("ppid of the shell (i.e. this shell): %d\n", getppid());

	while (is_true) {
		printf("$");
		// parse input
		char *path = "/bin/";

		char buf[MAX_LINE];
		fgets(buf, MAX_LINE+1, stdin);
		int len = strlen(buf);
		if (buf[len-1] == '\n') {
			buf[len-1] = '\0';
		}

		strcpy(command, path);
		strcat(command, buf);
		if (strcmp(buf, "exit") == 0) {
			is_true = 0;
		}

	
		else{
			
		
			pid = fork();

			if (pid < 0) {
				fprintf(stderr, "Fork failed");
				return 1;
			}
			else if (pid == 0) { // child
				printf("[child] pid: %d\n", getpid());
				printf("[child] parent pid: %d\n", getppid());
				execlp(command, buf, NULL);
			}
			else { // parent
				printf("[parent]: pid = %d\n", pid);
				printf("[parent] get ppid: %d\n", getppid());
				wait(NULL);
			}
		}
	}

	return 0;
}
