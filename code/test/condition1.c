#include "syscall.h"
#include "pthread.h"
#include <stddef.h>

static pthread_mutex_t mtx;
static pthread_cond_t cond;

void slave1(void *d)
{
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cond, &mtx);
    PutInt(1);
	UserThreadExit();
}

void slave2(void *d)
{
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cond, &mtx);
    PutInt(2);
    UserThreadExit();
}

void master(void *d)
{
    for (int i=0; i < 1000; i++) {}

    pthread_cond_signal(&cond);
	UserThreadExit();
}

int main()
{
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);

    int id1 = UserThreadCreate(slave1, (void *)0);
    UserThreadCreate(slave2, (void *)0);
	int id3 = UserThreadCreate(master, (void *)0);

	UserThreadJoin(id1);
    UserThreadJoin(id3);
    // Never halts
    //UserThreadJoin(id2);

	PutChar('\n');

	Halt();
	return 0;
}
