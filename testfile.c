#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	pid_t pid;

	pid = fork();
	if (pid == 0) {
		// CHILD
		for (int i = 0; i < 400; i++) {
			printf("Be ready... %d\n", i);
		}
		return -2;
	}
	else {
		// PARENT
		int status = 0;

		pid_t finished = wait(&status);

		printf("Child process id %d finished. Status: %d\n", finished, status);
		return 0;
	}
}
