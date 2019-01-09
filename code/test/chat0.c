#include "syscall.h"
#include <stdio.h>

void receive_thread(void *arg)
{
	int socket = (int)arg;
	char buff[36] = {'\0'};

	while (1) {
		Recv(socket, 36, buff);
		puts("\n1: ");
		puts(buff);
	}
}

int main()
{
	int socket = Connect(1, 2);
	UserThreadCreate(receive_thread, (void *)socket);
	char buffer[36];

	while (1) {
		fgets(buffer, 36, stdin);

		int i = 0;
		while (buffer[i] != '\0') {
			i++;
		}

		if (i <= 1)
			break;

		Send(socket, 36, buffer);
	}

	CloseConn(socket);
	PutChar('\n');

	return 0;
}
