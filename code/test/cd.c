#include "syscall.h"

int main()
{
	mkdir("test_dir");
	ls();
	cd("./test_dir");
	PutChar('\n');
	mkdir("test_dir");
	ls();
	cd("test_dir");
	ls();
	pwd();
	return 0;
}
