#include "semaphore.h"

#include "synch.h"
#include <set>

static Lock mtx("CondList Lock"); // TODO Use rwLock
static std::set<int> COND_LIST;

int do_CondInit()
{
	int sid = (int)new Condition("User Mode Cond");

	mtx.Acquire();
	COND_LIST.insert(sid);
	mtx.Release();

	return sid;
}

void do_CondWait(int cid, int lid)
{
	Condition *cond = NULL;

	mtx.Acquire();
	if (COND_LIST.count(cid))
		cond = (Condition *)cid;
	mtx.Release();

	if (cond) {
		cond->Wait();
	}
}

void do_CondSignal(int cid, int lid)
{
	Condition *cond = NULL;

	mtx.Acquire();
	if (COND_LIST.count(cid))
		cond = (Condition *)cid;
	mtx.Release();

	if (cond) {
		cond->Signal();
	}
}

void do_CondBroadcast(int cid, int lid)
{
    Condition *cond = NULL;

    mtx.Acquire();
    if (COND_LIST.count(cid))
        cond = (Condition *)cid;
    mtx.Release();

    if (cond) {
        cond->Post();
    }
}