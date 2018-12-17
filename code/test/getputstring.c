#include "syscall.h"
#include <stdio.h>

int main()
{
	char string[32];
	GetString(string, 32);
	PutString(string);

	Halt();
}
