#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *string = malloc(3000);

	fgets(string, 3000, stdin);
	puts(string);

	free(string);

	return 0;
}
