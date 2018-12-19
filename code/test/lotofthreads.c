#include "syscall.h"
#include <pthread.h>
#include <stdio.h>

#define NB_THREADS 200

void *thread(void *myId)
{
	PutInt((int)myId);
	putchar('-');

	return NULL;
}

int main()
{
	pthread_t id[NB_THREADS];

	for (int i = 0; i < NB_THREADS; ++i) {
		pthread_create(&id[i], NULL, thread, (void *)i);
		pthread_join(id[i], NULL);
	}

	putchar('\n');

	return 0;
}