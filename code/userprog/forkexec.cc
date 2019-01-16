#include "forkexec.h"
#include "system.h"
#include <string.h>

#include "synch.h"

#ifdef USER_PROGRAM
#include "addrspace.h"
#endif

static void StartNewProcess(int space);

static Lock processListLock("fork exec lock");

typedef struct bundle {
	AddrSpace *space;
	int userThreadExitAddr;
} bundle_t;

int do_ForkExec(char *s, int userThreadExitAddr)
{
	OpenFile *executable = fileSystem->Open((char *)s);
	if (executable == nullptr) {
		printf("Unable to open file %s\n", (char *)s);
		return -1;
	}

	AddrSpace *space = new AddrSpace(executable);
	delete executable; // close file

	currentThread->space->RestoreState();
	Thread *newProcess = new Thread("new process");
	space->RegisterThread(newProcess->id);
	AddProcess(newProcess->id); // add new process to the list of running processes

	bundle_t *b = new bundle_t;
	b->space = space;
	b->userThreadExitAddr = userThreadExitAddr;

	newProcess->pid = newProcess->id; // sets pid
	newProcess->Fork(StartNewProcess, (int)b);
	return newProcess->id;
}

static void StartNewProcess(int bundle)
{
	// arguments retrieval
	bundle_t *b = (bundle_t *)bundle;

	currentThread->space = b->space;
	currentThread->space->InitThreadRegisters(); // set the initial register values
	currentThread->space->RestoreState();        // load page table register

	machine->WriteRegister(31, b->userThreadExitAddr);

	delete b;

	machine->Run(); // jump to the user progam
	ASSERT(FALSE);  // machine->Run never returns;
	                // the address space exits
	                // by doing the syscall "exit"
}

void AddProcess(int pid)
{
	Semaphore *sem = new Semaphore("process", 0);
	processListLock.Acquire();
	processList.insert(std::make_pair(pid, sem));
	processListLock.Release();
}

void SignalProcess(int pid)
{
	processListLock.Acquire();

	processList.at(pid)->Post();
	processList.erase(pid);

	processListLock.Release();
}

void do_ProcessJoin(int pid)
{
	processListLock.Acquire();
	// return if process isn't running
	auto it = processList.find(pid);
	if (it == processList.end()) {
		processListLock.Release();
		return;
	}

	// wait on pid and erase entry from the map
	Semaphore *sem = processList.at(pid);

	processListLock.Release();

	sem->Wait();
	delete sem;
}
