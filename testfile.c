#include <stdio.h>
#include <stdlib.h>

int main() {
	char* test;
	test = getenv("PATH");
	printf("%s\n", test);
	return 0;
}
