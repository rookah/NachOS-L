// exception.cc
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "semaphore.h"

#include "forkexec.h"
#include "syscall.h"
#include "system.h"
#include "usersemaphore.h"
#include "userthread.h"
//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void UpdatePC()
{
	int pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, pc);
	pc += 4;
	machine->WriteRegister(NextPCReg, pc);
}

//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions
//      are in machine.h.
//----------------------------------------------------------------------
void copyStringFromMachine(int from, char *to, unsigned size);
char *mipsPtrToKernelPtr(int mipsPtr);

void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	char string[MAX_STRING_SIZE];

	if (which == SyscallException) {
		switch (type) {
		case SC_Exit:
			SignalProcess(currentThread->pid);
			currentThread->space->Exit();
			currentThread->Finish();
			break;

		case SC_Halt: {
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
			break;
		}

		case SC_PutChar: {
			synchconsole->SynchPutChar((char)(machine->ReadRegister(4)));
			break;
		}

		case SC_GetChar: {
			machine->WriteRegister(2, synchconsole->SynchGetChar());
			break;
		}

		case SC_PutString: {
			int from = machine->ReadRegister(4);
			char *str = mipsPtrToKernelPtr(from);

			while (*str != '\0') {
				synchconsole->SynchPutChar(*str);
				++from;
				str = mipsPtrToKernelPtr(from);
			}
		} break;

		case SC_GetString: { // FIXME if mips string is accross multiple pages
			synchconsole->SynchGetString(mipsPtrToKernelPtr(machine->ReadRegister(4)), machine->ReadRegister(5));
			break;
		}

		case SC_PutInt:
			synchconsole->SynchPutInt(machine->ReadRegister(4));
			break;

		case SC_GetInt: {
			int *n = (int *)(mipsPtrToKernelPtr(machine->ReadRegister(4)));
			synchconsole->SynchGetInt(n);
			break;
		}

		case SC_UserThreadCreate:
			machine->WriteRegister(2, do_UserThreadCreate(machine->ReadRegister(4), machine->ReadRegister(5), machine->ReadRegister(6)));
			break;

		case SC_UserThreadExit:
			do_UserThreadExit();
			break;

		case SC_UserThreadJoin:
			do_UserThreadJoin(machine->ReadRegister(4));
			break;

		case SC_SemInit:
			machine->WriteRegister(2, do_SemInit(machine->ReadRegister(4)));
			break;

		case SC_SemWait:
			do_SemWait(machine->ReadRegister(4));
			break;

		case SC_SemPost:
			do_SemPost(machine->ReadRegister(4));
			break;

		case SC_ForkExec:
			copyStringFromMachine((machine->ReadRegister(4)), string, MAX_STRING_SIZE);
			machine->WriteRegister(2, do_ForkExec(string));
			break;

		case SC_Sbrk:
			machine->WriteRegister(2, currentThread->space->AllocatePages(machine->ReadRegister(4)) * PageSize);
			break;

		case SC_ForkJoin:
			do_ProcessJoin(machine->ReadRegister(4));
			break;

		default: {
			printf("Unexpected user mode exception %d %d\n", which, type);
			ASSERT(FALSE);
		}
		}
		UpdatePC();
	}
}

char *mipsPtrToKernelPtr(int mipsPtr)
{
	char *fromptr;
	machine->Translate(mipsPtr, (int *)&fromptr, 1, false);
	fromptr += (int)machine->mainMemory;
	return fromptr;
}

void copyStringFromMachine(int from, char *to, unsigned size)
{
	char *fromptr = mipsPtrToKernelPtr(from);

	while (*fromptr != '\0' && size > 1) {
		*to = *fromptr;
		from++;
		to++;
		size--;
		fromptr = mipsPtrToKernelPtr(from);
	}
	*to = '\0';
}
