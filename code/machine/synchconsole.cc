#include "system.h"
#include "synchconsole.h"
#include "synch.h"	
#include "copyright.h"
#include "utility.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) { 
	readAvail->V();
	
 }
void copyStringFromMachine(int from,char *to,unsigned size);

static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
readAvail = new Semaphore("read avail", 0);
writeDone = new Semaphore("write done", 0);
console = new Console (readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
delete console;
delete writeDone;
delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	console->PutChar(ch);
	writeDone->P ();// wait for write to finish
}

char SynchConsole::SynchGetChar()
{
	readAvail->P ();	// wait for character to arrive
	return console->GetChar ();
		
}
void SynchConsole::SynchPutString(char *s)
{
	
	int i=0;
	while(s[i]!='\0'){
		SynchPutChar(s[i]);
		i++;
	}
	
}


void SynchConsole::SynchGetString(char *s, int n)
{	
	while(n>1 && ( *s=SynchGetChar())!=EOF  ){
		n--;
		if(*s=='\n')
			n=0;
		s++;
	}
	*s='\0';

}

void SynchConsole::SynchPutInt(int n)
{
	char string[12];
	snprintf(string,12,"%d",n);
	SynchPutString(string);
		
	}

void SynchConsole::SynchGetInt(int *n)
{	char string[13];

	SynchGetString(string,13);
	sscanf(string,"%d",n);
	
	}