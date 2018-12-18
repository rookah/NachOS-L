#ifndef CODE_SEMAPHORE_H_H
#define CODE_SEMAPHORE_H_H

#define __SIZEOF_SEM_T	16

typedef struct {
    int __align;
} sem_t;

//typedef union
//{
//    char __size[__SIZEOF_SEM_T];
//    long int __align;
//} sem_t;

int sem_init(sem_t *sid, int pshared, unsigned int value);
int sem_post(sem_t *sid);
int sem_wait(sem_t *sid);

#endif // CODE_SEMAPHORE_H_H