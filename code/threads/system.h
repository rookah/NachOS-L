// system.h
//      All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "interrupt.h"
#include "scheduler.h"
#include "stats.h"
#include "thread.h"
#include "timer.h"
#include "utility.h"
#include <unordered_map>

#define MAX_STRING_SIZE 256
#define MAX_OPEN_FILES 10

class SynchConsole;

// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); // Initialization,
                                               // called before anything else
extern void Cleanup();                         // Cleanup, called when
                                               // Nachos is done.

extern Thread *currentThread;       // the thread holding the CPU
extern Thread *threadToBeDestroyed; // the thread that just finished
extern Scheduler *scheduler;        // the ready list
extern Interrupt *interrupt;        // interrupt status
extern Statistics *stats;           // performance metrics
extern Timer *timer;                // the hardware alarm clock

#ifdef USER_PROGRAM
#include "machine.h"
#include "synchconsole.h"
extern Machine *machine; // user program memory and registers
extern SynchConsole *synchconsole;
extern std::unordered_map<int, Semaphore *> processList; // list of running (non-joined) user processes & their termination status
#endif

#ifdef FILESYS_NEEDED // FILESYS or FILESYS_STUB
#include "filesys.h"
extern FileSystem *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk *synchDisk;
extern std::unordered_map<int, OpenFile *> openFileList; // list of open files and their associated file descriptor
#endif

#ifdef NETWORK
#include "post.h"
#include "rconnpool.h"
extern PostOffice *postOffice;
extern RConnPool *connPool;
#endif

#endif // SYSTEM_H
