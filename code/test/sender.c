#include "syscall.h"

int main()
{
	int socket = Connect(0, 2);

	for (int i = 0; i < 5; ++i) {
		Send(socket, 11, "012345678\n");
		PutString("Sent\n");
	}

	PutChar('\n');

	return 0;
}
