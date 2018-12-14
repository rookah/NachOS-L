#include "semaphore.h"

#include "synch.h"
#include <set>

static Lock mtx("SemList Lock"); // TODO Use rwLock
static std::set<int> SEM_LIST;

int do_SemInit(int value)
{
	int sid = (int)new Semaphore("User Mode Sem", value);

	mtx.Acquire();
	SEM_LIST.insert(sid);
	mtx.Release();

	return sid;
}

void do_SemWait(int sid)
{
	Semaphore *sem = NULL;

	mtx.Acquire();
	if (SEM_LIST.count(sid))
		sem = (Semaphore *)sid;
	mtx.Release();

	if (sem) {
		sem->P();
	}
}

void do_SemPost(int sid)
{
	Semaphore *sem = NULL;

	mtx.Acquire();
	if (SEM_LIST.count(sid))
		sem = (Semaphore *)sid;
	mtx.Release();

	if (sem) {
		sem->V();
	}
}