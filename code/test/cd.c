#include "syscall.h"

int main() {
	mkdir("test_dir");
	cd("./test_dir");
	ls();
	pwd();
	return 0;
}
