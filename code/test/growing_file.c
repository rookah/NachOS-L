#include "syscall.h"


static char *const test_file_name = "grow.txt";

int main(){
    Create(test_file_name);
	int fd = Open(test_file_name);
	char str[] = "C'est un Canary Bay.\n";
	Write(fd, str, sizeof(str) / sizeof(char));
	Close(fd);

	char buf[sizeof(str) / sizeof(char)];
	int fd2 = Open(test_file_name);

	Read(fd2, buf, sizeof(str) / sizeof(char));
	PutString(buf);

	return 0;
}