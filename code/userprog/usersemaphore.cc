#include "semaphore.h"

#include "synch.h"
#include <unordered_map>

static Lock mtx("SemList Lock"); // TODO Use rwLock
static std::unordered_map<int, int> SEM_LIST;

int do_SemInit(int value)
{
	int sid = (int)new Semaphore("User Mode Sem", value);

	mtx.Acquire();

	int id = rand();
	while (SEM_LIST.count(id)) {
		id = rand();
	}

	SEM_LIST[id] = sid;
	mtx.Release();

	return id;
}

void do_SemWait(int sid)
{
	Semaphore *sem = NULL;

	mtx.Acquire();
	if (SEM_LIST.count(sid))
		sem = (Semaphore *)SEM_LIST[sid];
	mtx.Release();

	if (sem) {
		sem->Wait();
	}
}

void do_SemPost(int sid)
{
	Semaphore *sem = NULL;

	mtx.Acquire();
	if (SEM_LIST.count(sid))
		sem = (Semaphore *)SEM_LIST[sid];
	mtx.Release();

	if (sem) {
		sem->Post();
	}
}