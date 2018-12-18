#include <pthread.h>

#include "semaphore.h"
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


int pthread_mutex_init (pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr) {
//	ASSERT(__mutexattr == NULL);
    return sem_init((sem_t *)&__mutex->__align, 1, 1);
}


int pthread_mutex_lock(pthread_mutex_t *mutex) {
	return sem_wait((sem_t *)&mutex->__align);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	return sem_post((sem_t *)&mutex->__align);
}
