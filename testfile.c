#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

#define bool char
#define true 1
#define false 0

struct thread_args {
	int prime;
	int thread_number;
};

struct thread_return {
	bool is_prime;
};

void *function(void *void_args) {
	struct thread_args *args = (struct thread_args *) void_args;
	bool is_prime = true;
	for (long long i = 2; i * i < args->prime; i++) {
		if (args->prime % i == 0) {
			is_prime = false;
			break;
		}
	}
	struct thread_return *ret = malloc(sizeof(struct thread_return));
	ret->is_prime = is_prime;
	printf("Thread number %d: is_prime = %d\n", args->thread_number, (int) is_prime);
	return (void *) ret;
}

int main() {
	struct thread_return *ret[10];
	int primes_to_check[10] = { 5, 997, 1e9 + 7, 1997, 349, 1e7 + 9, 234, 555587, 9993369, 349 };
	pthread_t v[10];
	for (int i = 0; i < 10; i++) {
		int prime = primes_to_check[i];
		struct thread_args *args = malloc(sizeof(struct thread_args));
		args->prime = prime;
		args->thread_number = i;
		pthread_create(&v[i], NULL, function, (void *) args);
	}
	for (int i = 0; i < 10; i++) {
		pthread_join(v[i], (void **) &ret[i]);
	}
	for (int i = 0; i < 10; i++) {
		printf("%d\t", *(ret[i]));
	}
	printf("\n");
	return 0;
}
