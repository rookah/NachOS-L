#include "forkexec.h"
#include "system.h"
#include <string.h>


static void StartNewProcess(int space);

int do_ForkExec(char *s)
{	
	// open file
	OpenFile *executable = fileSystem->Open((char *) s);
	if (executable == NULL) {
		printf("Unable to open file %s\n", (char *) s);
		return -1;
	}
	AddrSpace *space;
	space = new AddrSpace(executable);
	delete executable; // close file
	currentThread->space->RestoreState();
	Thread *newProcess = new Thread("new process");
	AddProcess(newProcess->id); // add new process to the list of running processes
	newProcess->Fork(StartNewProcess, (int) space);
	return newProcess->id;
}

static void StartNewProcess(int space)
{
	currentThread->pid = currentThread->pid; // sets pid
	currentThread->space = (AddrSpace *) space;
	currentThread->space->InitRegisters(); // set the initial register values
	currentThread->space->RestoreState();  // load page table register

	machine->Run(); // jump to the user progam
	ASSERT(FALSE);  // machine->Run never returns;
	                // the address space exits
	                // by doing the syscall "exit"
}

void AddProcess(int pid)
{
	Semaphore *sem = new Semaphore("process", 0);
	processList.insert(std::make_pair(pid, sem));
}

void SignalProcess(int pid)
{
	processList.at(pid)->Post();
}

void do_ProcessJoin(int pid)
{
	// return if process isn't running
	auto it = processList.find(pid);
	if (it == processList.end())
		return;

	// wait on pid and erase entry from the map
	processList.at(pid)->Wait();
	delete processList.at(pid);
	processList.erase(pid);
}
