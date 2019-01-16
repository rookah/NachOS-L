#include "system.h"

int do_ForkExec(char *s, int userThreadExit);
void AddProcess(int pid);
void SignalProcess(int pid);
void do_ProcessJoin(int pid);
