#include "syscall.h"
#include "pthread.h"
#include <stddef.h>

static pthread_mutex_t mtx;
static pthread_cond_t cond;

void slave1(void *d)
{
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    PutInt(1);
    UserThreadExit();
}

void slave2(void *d)
{
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    PutInt(2);
    UserThreadExit();
}

void master(void *d)
{
    for (int i=0; i < 1000; i++) {}
    pthread_cond_broadcast(&cond); // Make sure it wakes up all the threads

    for (int i=0; i < 1000; i++) {}
    PutInt(3);
    UserThreadExit();
}

int main()
{
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);

    int slave1id = UserThreadCreate(slave1, (void *)0);
    int slave2id = UserThreadCreate(slave2, (void *)0);
    int masterid = UserThreadCreate(master, (void *)0);

    UserThreadJoin(slave1id);
    UserThreadJoin(slave2id);
    UserThreadJoin(masterid);

    PutChar('\n');

    Halt();
    return 0;
}
