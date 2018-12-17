#include "syscall.h"

#define BUFFER_SIZE 10
#define DATA_SIZE 100

int sem_empty;
int sem_full;

int buffer[BUFFER_SIZE];
int in;
int out;

void producer(void *d)
{
	for (int i = 0; i < DATA_SIZE; i++) {
		SemWait(sem_empty);

		buffer[in] = i;
		in = (in + 1) % BUFFER_SIZE;

		SemPost(sem_full);
	}

	UserThreadExit();
}

void consumer(void *d)
{
	for (int i = 0; i < DATA_SIZE; ++i) {
		SemWait(sem_full);

		PutInt(buffer[out]);
		PutChar('-');

		out = (out + 1) % BUFFER_SIZE;

		SemPost(sem_empty);
	}

	UserThreadExit();
}

int main()
{
	sem_empty = SemInit(BUFFER_SIZE);
	sem_full = SemInit(0);
	in = 0;
	out = 0;

	int id1 = UserThreadCreate(producer, (void *)0);
	int id2 = UserThreadCreate(consumer, (void *)0);

	UserThreadJoin(id1);
	UserThreadJoin(id2);

	PutChar('\n');

	Halt();
	return 0;
}
