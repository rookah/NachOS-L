#include "system.h"

int do_ForkExec(char *s);
void AddProcess(int pid);
void SignalProcess(int pid);
void do_ProcessJoin(int pid);
