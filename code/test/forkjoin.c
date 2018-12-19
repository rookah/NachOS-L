#include "syscall.h"

int main()
{
	int id = ForkExec("../build/prodcons");
	ForkJoin(id);
	PutString("Waiting for process ");
	PutInt(id);
	PutChar('\n');

	id = ForkExec("../build/prodcons");
	ForkJoin(id);
	PutString("Waiting for process ");
	PutInt(id);
	PutChar('\n');

	return 0;
}