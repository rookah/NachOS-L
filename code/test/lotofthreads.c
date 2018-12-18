#include <pthread.h>
#include "syscall.h"

void* thread(void* myId)
{
    PutInt((int)myId);

    return NULL;
}

int main()
{
    for (int i = 0; i < 500; ++i)
    {
        pthread_t id;
        pthread_create(&id, NULL, thread, (void*)i);
        for (int j = 0; j < 100; j++)
        {
            PutChar(' ');
        }
    }

    return 0;
}