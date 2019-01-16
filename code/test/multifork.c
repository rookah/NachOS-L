#include "syscall.h"

int main()
{
	PutString("With Join: \n\n");

	int id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	id = ForkExec("prodcons");
	ForkJoin(id);
	PutChar('\n');

	return 0;
}