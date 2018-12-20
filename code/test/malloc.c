#include <stdio.h>
#include <stdlib.h>

#define ALLOC_SIZE 4000 // Available pool size is a tad less than 4096

int main()
{
	char *string = malloc(ALLOC_SIZE);

	for (int i = 0; i < ALLOC_SIZE; i++) {
		string[i] = 'a';
	}

	string[ALLOC_SIZE - 2] = '\n';
	string[ALLOC_SIZE - 1] = '\0';
	puts(string);

	free(string);

	string = malloc(ALLOC_SIZE);

	for (int i = 0; i < ALLOC_SIZE; i++) {
		string[i] = 'b';
	}

	string[ALLOC_SIZE - 2] = '\n';
	string[ALLOC_SIZE - 1] = '\0';
	puts(string);

	free(string);

	string = malloc(ALLOC_SIZE);

	for (int i = 0; i < ALLOC_SIZE; i++) {
		string[i] = 'c';
	}

	string[ALLOC_SIZE - 2] = '\n';
	string[ALLOC_SIZE - 1] = '\0';
	puts(string);

	free(string);

	return 0;
}
