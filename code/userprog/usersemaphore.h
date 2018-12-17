#ifndef SEMAPHORE_H
#define SEMAPHORE_H

int do_SemInit(int value);

void do_SemWait(int sid);

void do_SemPost(int sid);

#endif