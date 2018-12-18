#include "forkexec.h"
#include "system.h"
#include <string.h>

static void StarNewProcess(int space);

int do_ForkExec(char *s)
{
	// arguments
	OpenFile *executable = fileSystem->Open((char *)s);
	if (executable == NULL) {
		printf("Unable to open file %s\n", (char *)s);
		return 0;
	}
	AddrSpace *space;
	space = new AddrSpace(executable);
	delete executable; // close file
	currentThread->space->RestoreState();
	Thread *newProcess = new Thread("new process");
	newProcess->Fork(StarNewProcess, (int)space);
	return 0;
}

static void StarNewProcess(int space)
{
	currentThread->space = (AddrSpace *)space;
	currentThread->space->InitThreadRegisters(); // set the initial register values
	currentThread->space->RestoreState();        // load page table register

	machine->Run(); // jump to the user progam
	ASSERT(FALSE);  // machine->Run never returns;
	                // the address space exits
	                // by doing the syscall "exit"
}
