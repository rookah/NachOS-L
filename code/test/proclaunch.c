#include "syscall.h"
#include <stdio.h>

int main()
{
	int newProc;
	char prompt[3], buffer[60];

	prompt[0] = '$';
	prompt[1] = ' ';
	prompt[2] = '\0';

	while (1) {
		puts(prompt);
		fgets(buffer, 60, stdin);

		int i = 0;
		while (buffer[i] != '\0') {
			i++;
		}

		if (buffer[i - 1] == '\n')
			buffer[i - 1] = '\0';

		if (i <= 1)
			break;

		newProc = ForkExec(buffer);
		ForkJoin(newProc);
	}
}
