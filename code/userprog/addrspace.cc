// addrspace.cc
//      Routines to manage address spaces (executing user programs).
//
//      In order to run a user program, you must:
//
//      1. link with the -N -T 0 option
//      2. run coff2noff to convert the object file to Nachos format
//              (Nachos object code format is essentially just a simpler
//              version of the UNIX executable object code format)
//      3. load the NOFF file into the Nachos file system
//              (if you haven't implemented the file system yet, you
//              don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "addrspace.h"
#include "copyright.h"
#include "frameprovider.h"
#include "noff.h"
#include "synch.h"
#include "system.h"
#include <set>
#include <strings.h> /* for bzero */

static int process_count = 0;
static void ReadAtVirtual(OpenFile *executable, int virtualaddr, int numBytes, int position);
static FrameProvider fp(NumPhysPages);

//----------------------------------------------------------------------
// SwapHeader
//      Do little endian to big endian conversion on the bytes in the
//      object file header, in case the file was generated on a little
//      endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void SwapHeader(NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//      Create an address space to run a user program.
//      Load the program from a file "executable", and set everything
//      up so that we can start executing user instructions.
//
//      Assumes that the object code file is in NOFF format.
//
//      First, set up the translation from program memory to physical
//      memory.  For now, this is really simple (1:1), since we are
//      only uniprogramming, and we have a single unsegmented page table
//
//      "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable) : mtx(new Lock("thread countlock"))
{
	NoffHeader noffH;
	unsigned int size;

	executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost(noffH.noffMagic) == NOFFMAGIC))
		SwapHeader(&noffH);
	ASSERT(noffH.noffMagic == NOFFMAGIC);

	// how big is address space?
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size; // we need to increase the size to leave room for the stack
	unsigned int numPages = divRoundUp(size, PageSize);
	size = numPages * PageSize;

	ASSERT(numPages <= MaxVirtPage); // check we're not trying to run anything too big -- at least until we have virtual memory

	DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);

	for (unsigned int i = 0; i < MaxVirtPage; i++) {
		pageTable[i].virtualPage = i;
		pageTable[i].valid = FALSE;
	}

	// first, set up the translation
	for (unsigned int i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;
		pageTable[i].physicalPage = fp.GetEmptyFrame();
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE; // if the code segment was entirely on a separate page, we could set its pages to be read-only
	}

	// write page table info to the machine
	machine->pageTable = pageTable;
	machine->pageTableSize = MaxVirtPage;

	// zero out the entire address space, to zero the unitialized data segment
	// and the stack segment
	// bzero(machine->mainMemory, size);

	// then, copy in the code and data segments into memory
	if (noffH.code.size > 0) {
		DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
				noffH.code.virtualAddr, noffH.code.size);
		ReadAtVirtual(executable, noffH.code.virtualAddr, noffH.code.size, noffH.code.inFileAddr);

	}
	if (noffH.initData.size > 0) {
		DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
				noffH.initData.virtualAddr, noffH.initData.size);
		ReadAtVirtual(executable, noffH.initData.virtualAddr, noffH.initData.size, noffH.initData.inFileAddr);
	}
	numThreads = 0;

	mtx->Acquire();
	process_count++;
	mtx->Release();
}

static void ReadAtVirtual(OpenFile *executable, int virtualaddr, int numBytes, int position)
{
	char buff[numBytes];
	numBytes = executable->ReadAt(buff, numBytes, position);
	for (int i = 0; i < numBytes; i++) {
		machine->WriteMem(virtualaddr + i, 1, buff[i]);
	}
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Deallocate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	// LB: Missing [] for delete
	// delete pageTable;
	for (unsigned i = 0; i < MaxVirtPage; i++) {
		if (pageTable[i].valid) {
			fp.ReleaseFrame(pageTable[i].physicalPage);
			pageTable[i].valid = false;
		}
	}
	// End of modification
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//      Set the initial values for the user-level register set.
//
//      We write these directly into the "machine" registers, so
//      that we can immediately jump to user code.  Note that these
//      will be saved/restored into the currentThread->userRegisters
//      when this thread is context switched out.
//----------------------------------------------------------------------

void AddrSpace::InitThreadRegisters()
{
	int i;

	for (i = 0; i < NumTotalRegs; i++)
		machine->WriteRegister(i, 0);

	// Initial program counter -- must be location of "Start"
	machine->WriteRegister(PCReg, 0);

	// Need to also tell MIPS where next instruction is, because
	// of branch delay possibility
	machine->WriteRegister(NextPCReg, 4);

	// Set the stack register to the end of the address space, where we
	// allocated the stack; but subtract off a bit, to make sure we don't
	// accidentally reference off the end!
	currentThread->userStack = AllocatePages(UserStackSize / PageSize, true);
	int stackAddr = currentThread->userStack * PageSize + UserStackSize - 16;

	machine->WriteRegister(StackReg, stackAddr);
	DEBUG('a', "Initializing stack register to %d\n", stackAddr);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//      On a context switch, save any machine state, specific
//      to this address space, that needs saving.
//
//      For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
	machine->pageTable = pageTable;
	machine->pageTableSize = MaxVirtPage;
}

int AddrSpace::ThreadCount()
{
	mtx->Acquire();
	int a = ++numThreads;
	mtx->Release();
	return a;
}

void AddrSpace::AddThread(int tid)
{
	Semaphore *sem = new Semaphore("thread", 0);
	threadList.insert(std::make_pair(tid, sem));
}

void AddrSpace::SignalThread(int tid)
{
	threadList.at(tid)->Post();
}

void AddrSpace::JoinThread(int tid)
{
	// return if tid isn't running
	std::unordered_map<int, Semaphore *>::iterator it = threadList.find(tid);
	if (it == threadList.end())
		return;

	// wait on tid and erase entry from the map
	threadList.at(tid)->Wait();
	delete threadList.at(tid);
	threadList.erase(tid);
}

void AddrSpace::Exit()
{
	mtx->Acquire();

	process_count--;
	if (process_count == 0)
		interrupt->Halt();

	mtx->Release();
}

int AddrSpace::AllocatePages(int numPages, bool fromEnd)
{
	std::set<int> usedVPN;

	for (unsigned int i = 0; i < MaxVirtPage; ++i) {
		if (pageTable[i].valid)
			usedVPN.insert(pageTable[i].virtualPage);
	}

	unsigned int nextAvailablePage = 0;

	if (!fromEnd) {
		for (nextAvailablePage = 0; nextAvailablePage < MaxVirtPage - numPages; nextAvailablePage++) {
			bool good = true;
			for (int i = 0; i < numPages; ++i) {
				if (usedVPN.count(nextAvailablePage + i)) {
					good = false;
					break;
				}
			}

			if (good)
				break;
		}
	} else {
		for (nextAvailablePage = MaxVirtPage - numPages; nextAvailablePage >= 0; nextAvailablePage--) {
			bool good = true;
			for (int i = 0; i < numPages; ++i) {
				if (usedVPN.count(nextAvailablePage + i)) {
					good = false;
					break;
				}
			}

			if (good)
				break;
		}
	}

	for (unsigned int i = nextAvailablePage; i < nextAvailablePage + numPages; ++i) {
		pageTable[i].physicalPage = fp.GetEmptyFrame();
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;
	}

	return nextAvailablePage;
}

void AddrSpace::FreePages(unsigned int vpn, unsigned int numPages)
{
	for (unsigned int j = vpn; j < vpn + numPages; j++) {
		if (pageTable[j].valid) {
			fp.ReleaseFrame(pageTable[j].physicalPage);
			pageTable[j].valid = false;
		}
	}
}