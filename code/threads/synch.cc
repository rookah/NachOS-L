// synch.cc
//      Routines for synchronizing threads.  Three kinds of
//      synchronization routines are defined here: semaphores, locks
//      and condition variables (the implementation of the last two
//      are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::Post for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "synch.h"
#include "copyright.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      Initialize a semaphore, so that it can be used for synchronization.
//
//      "debugName" is an arbitrary name, useful for debugging.
//      "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char *debugName, int initialValue)
{
	name = debugName;
	value = initialValue;
	queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      De-allocate semaphore, when no longer needed.  Assume no one
//      is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
	delete queue;
}

//----------------------------------------------------------------------
// Semaphore::Wait
//      Wait until semaphore value > 0, then decrement.  Checking the
//      value and decrementing must be done atomically, so we
//      need to disable interrupts before checking the value.
//
//      Note that Thread::Sleep assumes that interrupts are disabled
//      when it is called.
//----------------------------------------------------------------------

void Semaphore::Wait()
{
	IntStatus oldLevel = interrupt->SetLevel(IntOff); // disable interrupts

	while (value == 0) {                      // semaphore not available
		queue->Append((void *)currentThread); // so go to sleep
		currentThread->Sleep();
	}
	value--; // semaphore available,
	// consume its value

	(void)interrupt->SetLevel(oldLevel); // re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::Post
//      Increment semaphore value, waking up a waiter if necessary.
//      As with Wait(), this operation must be atomic, so we need to disable
//      interrupts.  Scheduler::ReadyToRun() assumes that threads
//      are disabled when it is called.
//----------------------------------------------------------------------

void Semaphore::Post()
{
	Thread *thread;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	thread = (Thread *)queue->Remove();
	if (thread != NULL) // make thread ready, consuming the Post immediately
		scheduler->ReadyToRun(thread);
	value++;
	(void)interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments
// Note -- without a correct implementation of Condition::Wait(),
// the test case in the network assignment won't work!
Lock::Lock(const char *debugName) : sem(debugName, 1)
{
}

Lock::~Lock()
{
}
void Lock::Acquire()
{
	sem.Wait();
}
void Lock::Release()
{
	sem.Post();
}

Condition::Condition(const char *debugName)
: name(debugName), lock("condition_lock"), sem_empty("sem_empty", MaxThreadNum), sem_full("sem_full", 0)
{
}

Condition::~Condition()
{
}
void Condition::Wait(Lock *conditionLock)
{
	ASSERT(conditionLock != nullptr);

	lock.Acquire();
	++queue_size;
	lock.Release();

	conditionLock->Release();

	sem_empty.Wait();
	sem_full.Post();

	conditionLock->Acquire();
}

void Condition::Signal(Lock *conditionLock)
{
	ASSERT(conditionLock != nullptr);

	lock.Acquire();
	if (queue_size > 0) {
		conditionLock->Acquire();

	    --queue_size;
		sem_empty.Post();
		sem_full.Wait();
	}
	lock.Release();
}
void Condition::Broadcast(Lock *conditionLock)
{
	ASSERT(conditionLock != nullptr);

	lock.Acquire();

	for (int i=0; i < queue_size; i++) {
		sem_empty.Post();
	}

	while(queue_size > 0) {
		--queue_size;
		conditionLock->Acquire();

		sem_full.Wait();
	}

	lock.Release();
}
