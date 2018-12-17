#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "copyright.h"
#include "synch.h"
#include "utility.h"

class SynchConsole
{
  public:
	SynchConsole(char *readFile, char *writeFile); // initialize the hardware console device
	~SynchConsole();                               // clean up console emulation

	void SynchPutChar(const char ch); // Unix putchar(3S)
	int SynchGetChar();               // Unix getchar(3S)

	void SynchPutString(char *s);        // Unix puts(3S)
	void SynchGetString(char *s, int n); // Unix fgets(3S)

	void SynchPutInt(int n);
	void SynchGetInt(int *n);

  private:
	Console *console;
	Lock *mutex;
};
#endif // SYNCHCONSOLE_H
