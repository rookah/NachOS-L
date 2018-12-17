#ifndef CONDITION_H
#define CONDITION_H

int do_CondInit();

void do_CondWait(int cid, int lid);

void do_CondSignal(int cid, int lid);

void do_CondBroadcast(int cid, int lid);

#endif