#include <pthread.h>

#include "semaphore.h"
#include "syscall.h"

// FIXME Redefined in several locations
#define MaxThreadNum 8

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

int pthread_cond_init (pthread_cond_t *__restrict __cond, const pthread_condattr_t *__restrict __cond_attr) {
    pthread_mutex_init((pthread_mutex_t*) &__cond->__data.__g_refs[0], NULL); // mutex
    __cond->__data.__g_refs[1] = 0; // queue_size
    sem_init((sem_t *) &__cond->__data.__g_signals[0], 0, MaxThreadNum); // sem_empty
    sem_init((sem_t *) &__cond->__data.__g_signals[1], 0, 0); // sem_full

    return 0; // FIXME return negative value on error
}

//int pthread_cond_destroy (pthread_cond_t *__cond)

int pthread_cond_signal (pthread_cond_t *__cond) {
    pthread_mutex_lock((pthread_mutex_t*) &__cond->__data.__g_refs[0]); // mutex

    int *queue_size = (int*) &__cond->__data.__g_refs[1]; // queue_size
    if (*queue_size > 0) {
        --(*queue_size);

        sem_post((sem_t *) &__cond->__data.__g_signals[0]); // sem_empty
        sem_wait((sem_t *) &__cond->__data.__g_signals[1]); // sem_full
    }

    pthread_mutex_unlock((pthread_mutex_t*) &__cond->__data.__g_refs[0]); // mutex

    return 0;
}

int pthread_cond_broadcast (pthread_cond_t *__cond) {
    pthread_mutex_lock((pthread_mutex_t*) &__cond->__data.__g_refs[0]); // mutex

    int *queue_size = (int*) &__cond->__data.__g_refs[1]; // queue_size
    for (int i=0; i < *queue_size; i++) {
        sem_post((sem_t *) &__cond->__data.__g_signals[0]); // sem_empty
    }

    while(*queue_size > 0) {
        --(*queue_size);

        sem_wait((sem_t *) &__cond->__data.__g_signals[1]); // sem_full
    }

    pthread_mutex_unlock((pthread_mutex_t*) &__cond->__data.__g_refs[0]); // mutex

    return 0;
}

int pthread_cond_wait (pthread_cond_t *__restrict __cond, pthread_mutex_t *__restrict __mutex) {
    pthread_mutex_lock((pthread_mutex_t*) &__cond->__data.__g_refs[0]); // mutex
    int *queue_size = (int*) &__cond->__data.__g_refs[1]; // queue_size
    ++(*queue_size);
    pthread_mutex_unlock((pthread_mutex_t*) &__cond->__data.__g_refs[0]); // mutex

    pthread_mutex_unlock(__mutex);

    sem_wait((sem_t *) &__cond->__data.__g_signals[0]); // sem_empty
    sem_post((sem_t *) &__cond->__data.__g_signals[1]); // sem_full

    pthread_mutex_lock(__mutex);

    return 0;
}