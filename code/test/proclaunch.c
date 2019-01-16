#include "../userlibs/libstring.h"
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
		int j = 0;
		while (buffer[i] != '\0' && buffer[i] != ' ') {
			i++;
		}

		if (buffer[i] == ' ') {
			buffer[i] = '\0';
			i++;
			j = i;
			while (buffer[i] != '\0') {
				i++;
			}
		}

		if (buffer[i - 1] == '\n')
			buffer[i - 1] = '\0';

		if (i <= 1)
			break;
		if (streq(buffer, "ls")) {
			ls();
		} else if (streq(buffer, "pwd")) {
			pwd();
		} else if (streq(buffer, "touch")) {
			Create(buffer + j);
		} else if (streq(buffer, "mkdir")) {
			mkdir(buffer + j);
		} else if (streq(buffer, "cd")) {
			cd(buffer + j);
		} else if (streq(buffer, "rm")) {
			rm(buffer + j);
		} else if (streq(buffer, "rmdir")) {
			rm(buffer + j);
		} else if (streq(buffer, "exit")) {
			Halt();
		} else {
			newProc = ForkExec(buffer);
			ForkJoin(newProc);
		}
	}
}
