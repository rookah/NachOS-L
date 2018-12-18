#include <pthread.h>
#include "syscall.h"

pthread_mutex_t protecc;
int counter = 0;

#define NUM_THREADS 7

void thread(void *d)
{
	for (int i=0; i < 200; i++) {
		pthread_mutex_lock(&protecc);
		counter += 1;
		pthread_mutex_unlock(&protecc);

	}

	UserThreadExit();
}

int main()
{
    int id[NUM_THREADS];

	pthread_mutex_init(&protecc, NULL);
	for (int i=0; i < NUM_THREADS; i++) {
        id[i] = UserThreadCreate(thread, (void *)0);
	}

    for (int i=0; i < NUM_THREADS; i++) {
        UserThreadJoin(id[i]);
    }


    PutInt(counter);
	PutChar('\n');

	Halt();
	return 0;
}
