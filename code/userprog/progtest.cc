// progtest.cc
//      Test routines for demonstrating that Nachos can load
//      a user program and execute it.
//
//      Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "addrspace.h"
#include "console.h"
#include "copyright.h"
#include "synch.h"
#include "synchconsole.h"
#include "system.h"

//----------------------------------------------------------------------
// StartProcess
//      Run a user program.  Open the executable, load it into
//      memory, and jump to it.
//----------------------------------------------------------------------

void StartProcess(char *filename)
{
	OpenFile *executable = fileSystem->Open(filename);
	AddrSpace *space;

	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return;
	}
	space = new AddrSpace(executable);
	delete executable; // close file
	currentThread->space = space;

	space->RegisterThread(currentThread->id);
	currentThread->pid = currentThread->id; // pid = main thread's id

	Semaphore *sem = new Semaphore("main_process", 0);
	processList.insert(std::make_pair(currentThread->pid, sem));
	space->InitThreadRegisters(); // set the initial register values
	space->RestoreState();        // load page table register

	machine->Run(); // jump to the user progam
	ASSERT(FALSE);  // machine->Run never returns;
	                // the address space exits
	                // by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
//      Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg)
{
	readAvail->Post();
}
static void WriteDone(int arg)
{
	writeDone->Post();
}

//----------------------------------------------------------------------
// ConsoleTest
//      Test the console by echoing characters typed at the input onto
//      the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void ConsoleTest(char *in, char *out)
{
	char ch;

	console = new Console(in, out, ReadAvail, WriteDone, 0);
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);

	for (;;) {
		readAvail->Wait(); // wait for character to arrive
		ch = console->GetChar();
		// echo it!
		console->PutChar('<');
		writeDone->Wait(); // wait for write to finish

		console->PutChar(ch);
		writeDone->Wait();

		console->PutChar('>');
		writeDone->Wait();

		if (ch == 'q' || ch == EOF)
			return; // if q, quit
	}
}

void SynchConsoleTest(char *in, char *out)
{
	char ch;

	while ((ch = synchconsole->SynchGetChar()) != EOF)
		synchconsole->SynchPutChar(ch);

	fprintf(stderr, "Solaris: EOF detected in SynchConsole!\n");
}
