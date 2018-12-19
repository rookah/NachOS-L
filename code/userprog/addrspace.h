// addrspace.h
//      Data structures to keep track of executing user programs
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "translate.h"
#include <unordered_map>

class Lock;
class Semaphore;

#define UserStackSize 4096            // increase this as necessary!
#define MaxVirtPage (unsigned int)100 // VM Size

class AddrSpace
{
  public:
	AddrSpace(OpenFile *executable); // Create an address space,
	// initializing it with the program
	// stored in the file "executable"
	~AddrSpace(); // De-allocate an address space

	void InitThreadRegisters(); // Initialize user-level CPU registers,
	// before jumping to user code

	void SaveState();    // Save/restore address space-specific
	void RestoreState(); // info on a context switch
	int ThreadCount();
	void AddThread(int tid);
	void SignalThread(int tid);
	void JoinThread(int tid);

	void Exit();

	int AllocatePages(int nbPages, bool fromEnd = false);
	void FreePages(unsigned int vpn, unsigned int numPages = 1);

  private:
	TranslationEntry pageTable[MaxVirtPage];

	int numThreads;
	Lock *mtx;
	std::unordered_map<int, Semaphore *> threadList;
	unsigned int brk;
};

#endif // ADDRSPACE_H
