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
	UserThreadCreate(thread3, (void *)0);

	for (int i = 0; i < 50; i++) {
		PutChar('2');
	}

	UserThreadExit();
}

int main()
{
	UserThreadCreate(thread1, (void *)0);
	UserThreadCreate(thread2, (void *)0);

	for (int i = 0; i < 100; ++i)
		PutInt(0);

	PutChar('\n');

	Halt();
	return 0;
}