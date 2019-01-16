#include "syscall.h"


static char *const test_file_name = "grow.txt";

int main(){
    Create(test_file_name);
	int fd = Open(test_file_name);

	char str[] = "Hello, this is a very long sentence that makes sure that growing files work properly.";
	Write(fd, str, sizeof(str));
	Close(fd);


	char buf[sizeof(str)] = {0};
	int fd2 = Open(test_file_name);

	Read(fd2, buf, sizeof(str));
	PutString(buf);
	return 0;
}