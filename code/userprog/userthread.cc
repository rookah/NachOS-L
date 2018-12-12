#include "system.h"
#include "machine.h"
// #include "mipssim.h"	

typedef struct bundle{

	int f;
	int arg;
}bundle_t;


static void StartUserThread(int f);	

int do_UserThreadCreate(int f, int arg){
	bundle_t *b = new bundle_t;
	b->f=f;
	b->arg=arg;
	Thread *t = new Thread ("newThread");
	t->Fork (StartUserThread, (int)b);
	return 0;
}

static void StartUserThread(int f){

	bundle_t *b=(bundle_t*) f;
	

	printf("%d",b->f);
    // for (i = 0; i < NumTotalRegs; i++);
	// machine->WriteRegister (i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister (PCReg, b->f);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister (NextPCReg, b->f + 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister (StackReg, machine->ReadRegister(StackReg)-PageSize*3 );
    DEBUG ('a', "Initializing stack register to %d\n",
	   machine->ReadRegister(StackReg));

    machine->WriteRegister(4,b->arg);
 //    scheduler->ReadyToRun(currentThread);
 //    interrupt->setStatus(UserMode);
 //    interrupt->SetLevel(IntOn);
 //    Instruction instr;
 //    for (;;) {
 //        machine->OneInstruction(&instr);
	// interrupt->OneTick();
 //    }
 //    // machine->Run ();		// jump to the user progam
 //    ASSERT (FALSE);		// machine->Run never returns;

}