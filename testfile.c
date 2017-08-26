#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	int a = 1;
	int *b = &a, *c = NULL;

	printf("%d\n", (b || c));
}
