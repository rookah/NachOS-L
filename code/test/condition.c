#include "syscall.h"

//static cond_t wake_up;
//static sem_t sem;

void thread1(void *d)
{

	UserThreadExit();
}

void thread2(void *d)
{

	UserThreadExit();
}

int main()
{
	int id1 = UserThreadCreate(thread1, (void *)0);
	int id2 = UserThreadCreate(thread2, (void *)0);

	UserThreadJoin(id1);
	UserThreadJoin(id2);

	PutChar('\n');

	Halt();
	return 0;
}
