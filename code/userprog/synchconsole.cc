#include "synchconsole.h"
#include "copyright.h"
#include "synch.h"
#include "system.h"
#include "utility.h"

void copyStringFromMachine(int from, char *to, unsigned size);

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg)
{
	readAvail->Post();
}

static void WriteDone(int arg)
{
	writeDone->Post();
}

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	mutexGet = new Lock("lock get console");
	mutexPut = new Lock("lock put console");
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
	delete mutexGet;
	delete mutexPut;
}

void SynchConsole::SynchPutChar(const char ch)
{
	mutexPut->Acquire();
	console->PutChar(ch);
	writeDone->Wait(); // wait for write to finish
	mutexPut->Release();
}

int SynchConsole::SynchGetChar()
{
	mutexGet->Acquire();
	readAvail->Wait(); // wait for character to arrive
	int c = console->GetChar();
	mutexGet->Release();
	return c;
}
void SynchConsole::SynchPutString(char *s)
{
	int i = 0;
	while (s[i] != '\0') {
		SynchPutChar(s[i]);
		i++;
	}
}

void SynchConsole::SynchGetString(char *s, int n)
{
	int c;
	while (n > 1 && (c = SynchGetChar()) != EOF) {
		*s = (char)c;
		n--;
		if (*s == '\n')
			n = 0;
		s++;
	}
	*s = '\0';
}

void SynchConsole::SynchPutInt(int n)
{
	char string[12];
	snprintf(string, 12, "%d", n);
	SynchPutString(string);
}

void SynchConsole::SynchGetInt(int *n)
{
	char string[13];
	SynchGetString(string, 13);
	sscanf(string, "%d", n);
}
