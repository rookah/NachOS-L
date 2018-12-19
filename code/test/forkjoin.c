#include "syscall.h"

int main()
{
	PutString("With Join: \n\n");

	int id = ForkExec("../build/prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("../build/prodcons");
	ForkJoin(id);
	PutChar('\n');

	PutString("Without Join: \n");

	ForkExec("../build/prodcons");
	ForkExec("../build/prodcons");

	PutString("\n\n");

	return 0;
}