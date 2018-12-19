#include "semaphore.h"
#include "pthread.h"
#include <stdio.h>

#define BUFFER_SIZE 10
#define DATA_SIZE 100

sem_t sem_empty;
sem_t sem_full;

int buffer[BUFFER_SIZE];
int in;
int out;

void *producer(void *d)
{
	for (int i = 0; i < DATA_SIZE; i++) {
		sem_wait(&sem_empty);

		buffer[in] = i;
		in = (in + 1) % BUFFER_SIZE;

		sem_post(&sem_full);
	}

	return NULL;
}

void *consumer(void *d)
{
	for (int i = 0; i < DATA_SIZE; ++i) {
		sem_wait(&sem_full);

		// putchar(buffer[out]); FIXME Display number
		putchar('-');

		out = (out + 1) % BUFFER_SIZE;

		sem_post(&sem_empty);
	}

	return NULL;
}

int main()
{
	sem_init(&sem_empty, 0, BUFFER_SIZE);
	sem_init(&sem_full, 0, 0);
	in = 0;
	out = 0;

	pthread_t id1;
	pthread_t id2;
	pthread_create(&id1, NULL, producer, (void *)0);
	pthread_create(&id2, NULL, consumer, (void *)0);

	pthread_join(id1, NULL);
	pthread_join(id2, NULL);

	putchar('\n');
	return 0;
}
