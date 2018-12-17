#include "machine.h"
#include "system.h"

typedef struct bundle {
	int f;
	int arg;
	int userThreadExitAddr;
} bundle_t;

static void StartUserThread(int f);

int do_UserThreadCreate(int f, int arg, int userThreadExitAddr)
{
	bundle_t *b = new bundle_t;
	b->f = f;
	b->arg = arg;
	b->userThreadExitAddr = userThreadExitAddr;
	Thread *t = new Thread("newThread");
	t->Fork(StartUserThread, (int)b);
	return t->id;
}

void do_UserThreadJoin(int tid)
{
	currentThread->space->JoinThread(tid);
}

void do_UserThreadExit()
{
	currentThread->space->SignalThread(currentThread->id);
	currentThread->Finish();
}

static void StartUserThread(int f)
{

	// arguments retrieval
	bundle_t *b = (bundle_t *)f;

	// page table load
	currentThread->space->RestoreState();

	// PC update
	machine->WriteRegister(PCReg, b->f);
	machine->WriteRegister(NextPCReg, b->f + 4);
	machine->WriteRegister(31, b->userThreadExitAddr);

	// stack pointer assignation
	machine->WriteRegister(StackReg, machine->pageTableSize * PageSize - (UserStackSize / MaxThreadNum) * currentThread->id);
	DEBUG('a', "Initializing stack register to %d\n", machine->ReadRegister(StackReg));

	machine->Run();
	ASSERT(FALSE);
}
