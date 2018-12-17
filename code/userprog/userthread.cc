#include "machine.h"
#include "system.h"

typedef struct bundle {
	int f;
	int arg;
	int stackreg;
	int userThreadExitAddr;
} bundle_t;

static void StartUserThread(int f);

int do_UserThreadCreate(int f, int arg, int userThreadExitAddr)
{
	bundle_t *b = new bundle_t;
	b->f = f;
	b->arg = arg;
	b->stackreg = machine->ReadRegister(StackReg);
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
	bundle_t *b = (bundle_t *)f;

	machine->WriteRegister(PCReg, b->f);
	machine->WriteRegister(NextPCReg, b->f + 4);
	machine->WriteRegister(31, b->userThreadExitAddr);

	machine->WriteRegister(StackReg, b->stackreg - PageSize * 2 * currentThread->id); // machine->ReadRegister(StackReg)machine->pageTableSize
	                                                                                  // * PageSize
	DEBUG('a', "Initializing stack register to %d\n", machine->ReadRegister(StackReg));

	currentThread->space->RestoreState();

	machine->Run();
	ASSERT(FALSE);
}