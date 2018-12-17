#include <pthread.h>

#include "syscall.h"

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
	if (attr != NULL) // Not implemented
		return -1;

	*thread = UserThreadCreate((void (*)(void *))start_routine, arg);

	if (*thread != -1)
		return 0;
	else
		return -1;
}

void pthread_exit(void *retval)
{
	UserThreadExit();
}

int pthread_join(pthread_t __th, void **__thread_return)
{
	UserThreadJoin(__th);
	return 0;
}