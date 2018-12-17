#include <stdio.h>

int main()
{
	char str[32];
	puts(fgets(str, 32, stdin));
	putchar(getc(stdin));

	putchar('\n');
	return 0;
}