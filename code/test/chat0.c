#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>

void receive_thread(void *arg)
{
	int socket = (int)arg;
	char *buff = malloc(2040);

	int size = 0;

	while (1) {
		Recv(socket, 4, (char *)&size);
		Recv(socket, size, buff);
		puts("\n1: ");
		puts(buff);
	}

	free(buff);
}

int main()
{
	int socket = Connect(1, 2);
	char *buffer = malloc(2040);
	UserThreadCreate(receive_thread, (void *)socket);

	while (1) {
		fgets(buffer, 2040, stdin);

		int i = 0;
		while (buffer[i] != '\0') {
			i++;
		}
		i++; // Takes \0 into account

		if (i <= 1)
			break;

		Send(socket, 4, (char *)&i);
		Send(socket, i, buffer);
	}

	CloseConn(socket);
	putchar('\n');
	free(buffer);

	return 0;
}
