#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096 // Corresponds to stdin buffer limit

void receive_thread(void *arg)
{
	int socket = (int)arg;
	char *buff = malloc(BUFFER_SIZE);

	int size = 0;

	while (1) {
		Recv(socket, 4, (char *)&size);
		Recv(socket, size, buff);
		puts("\n0: ");
		puts(buff);
	}

	free(buff);
}

int main()
{
	int socket = Connect(0, 2);
	char *buffer = malloc(BUFFER_SIZE);
	UserThreadCreate(receive_thread, (void *)socket);

	puts("Press <return> to quit\n");

	while (1) {
		fgets(buffer, BUFFER_SIZE, stdin);

		int i = 0;
		while (buffer[i] != '\0') {
			i++;
		}
		i++; // Takes \0 into account

		if (i <= 2)
			break;

		Send(socket, 4, (char *)&i);
		Send(socket, i, buffer);
	}

	CloseConn(socket);
	putchar('\n');
	free(buffer);

	return 0;
}
