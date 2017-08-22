#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	FILE *stream = fopen("inexistent", "r");
	FILE *stream2 = fopen("error_handler.c", "r");
	printf("%p\n", stream);
	printf("%p\n", stream2);
	return 0;
}
