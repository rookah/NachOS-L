#include "syscall.h"

int main()
{
	SpaceId newProc;
	OpenFileId input = ConsoleInput;
	OpenFileId output = ConsoleOutput;
	char prompt[2], buffer[60];
	int i;

	prompt[0] = '-';
	prompt[1] = '-';

	while (1) {
		Write(output, prompt, 2);

		i = 0;

		do {

			Read( input,&buffer[i], 1);

		} while (buffer[i++] != '\n');

		buffer[--i] = '\0';

		if (i > 0) {
			newProc = Exec(buffer);
			Join(newProc);
		}
	}
}
