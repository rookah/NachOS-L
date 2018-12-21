#include "syscall.h"

void thread3(void *d)
{
	for (int i = 0; i < 5; i++) {
		PutChar('3');
	}

	UserThreadExit();
}

void thread1(void *d)
{
	for (int i = 0; i < 5; i++) {
		PutChar('1');
	}

	UserThreadExit();
}

void thread2(void *d)
{
	int id3 = UserThreadCreate(thread3, (void *)0);

	for (int i = 0; i < 5; i++) {
		PutChar('2');
	}

	UserThreadJoin(id3);
	UserThreadExit();
}

int main()
{
	int id1 = UserThreadCreate(thread1, (void *)0);
	int id2 = UserThreadCreate(thread2, (void *)0);

	UserThreadJoin(id1);
	UserThreadJoin(id2);

	PutChar('\n');

	return 0;
}
