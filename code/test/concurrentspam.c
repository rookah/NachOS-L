#include "syscall.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096 // Corresponds to stdin buffer limit

pthread_mutex_t send_mutex;

void *send_thread1(void *arg)
{
	static const char string[] = "SPAM1\n";
	static const int length = sizeof(string);

	int socket = (int)arg;

	for (int i = 0; i < 1000; i++) {
		pthread_mutex_lock(&send_mutex);

		Send(socket, 4, (char *)&length);
		Send(socket, length, string);

		pthread_mutex_unlock(&send_mutex);
	}

	return NULL;
}

void *send_thread2(void *arg)
{
	static const char string[] = "SPAM2\n";
	static const int length = sizeof(string);

	int socket = (int)arg;

	for (int i = 0; i < 1000; i++) {
		pthread_mutex_lock(&send_mutex);

		Send(socket, 4, (char *)&length);
		Send(socket, length, string);

		pthread_mutex_unlock(&send_mutex);
	}

	return NULL;
}

int main()
{
	pthread_mutex_init(&send_mutex, NULL);
	int socket = Connect(1, 2);

	pthread_t tid1 = 0;
	pthread_t tid2 = 0;
	pthread_create(&tid1, NULL, send_thread1, (void *)socket);
	pthread_create(&tid2, NULL, send_thread2, (void *)socket);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	CloseConn(socket);
	putchar('\n');

	return 0;
}
