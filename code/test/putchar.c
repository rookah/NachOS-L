#include "syscall.h"

void print(char c, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		PutChar(c + i);
	}
}

int main()
{
	print('a', 4);

	PutChar('\n');

	Halt();
}
