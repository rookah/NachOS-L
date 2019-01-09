#include "syscall.h"

int main()
{
	int socket = Connect(1, 2);
	char buff[11];

	for (int i = 0; i < 5; ++i) {
		Recv(socket, 11, buff);
		PutString("Received: ");
		PutString(buff);
	}

	PutChar('\n');

	return 0;
}
