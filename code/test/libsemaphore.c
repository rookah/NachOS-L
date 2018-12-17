
#include "syscall.h"
#include <semaphore.h>

// int    sem_close(sem_t *);
// int    sem_destroy(sem_t *);
// int    sem_getvalue(sem_t *, int *);

int sem_init(sem_t *sid, int pshared, unsigned int value)
{
	if (pshared)
		return -1;

	sid->__align = SemInit(value);
	return 0;
}

int sem_post(sem_t *sid) // FIXME return value posix like
{
	SemPost(sid->__align);
	return 0;
}

int sem_wait(sem_t *sid) // FIXME return value posix like
{
	SemWait(sid->__align);
	return 0;
}