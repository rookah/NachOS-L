#ifndef CODE_PTHREAD_H_H
#define CODE_PTHREAD_H_H

#include "semaphore.h"

#define __SIZEOF_PTHREAD_MUTEX_T 24
#define __SIZEOF_PTHREAD_ATTR_T 36
#define __SIZEOF_PTHREAD_RWLOCK_T 32
#define __SIZEOF_PTHREAD_BARRIER_T 20

#define __SIZEOF_PTHREAD_MUTEXATTR_T 4
#define __SIZEOF_PTHREAD_COND_T 48
#define __SIZEOF_PTHREAD_CONDATTR_T 4
#define __SIZEOF_PTHREAD_RWLOCKATTR_T 8
#define __SIZEOF_PTHREAD_BARRIERATTR_T 4

# define __SIZEOF_SEM_T	16

//typedef unsigned long int pthread_t;
typedef int pthread_t;


typedef union
{
    char __size[__SIZEOF_PTHREAD_MUTEXATTR_T];
    int __align;
} pthread_mutexattr_t;

typedef struct
{
    char __size[__SIZEOF_PTHREAD_CONDATTR_T];
    int __align;
} pthread_condattr_t;

typedef struct
{
    sem_t sem;
} pthread_mutex_t;

typedef union
{
    int queue_size;
    pthread_mutex_t mutex;
    sem_t sem_empty;
    sem_t sem_full;
} pthread_cond_t;

typedef union
{
    char __size[__SIZEOF_PTHREAD_ATTR_T];
    long int __align;
} pthread_attr_t;

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
void pthread_exit(void *retval);
int pthread_join(pthread_t __th, void **__thread_return);


int pthread_mutex_init (pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

int pthread_cond_init (pthread_cond_t *__restrict __cond, const pthread_condattr_t *__restrict __cond_attr);
int pthread_cond_signal (pthread_cond_t *__cond);
int pthread_cond_broadcast (pthread_cond_t *__cond);
int pthread_cond_wait (pthread_cond_t *__restrict __cond, pthread_mutex_t *__restrict __mutex);

#endif //CODE_PTHREAD_H_H
