#include "syscall.h"

int main() {	
	int i=Open("ls");
	PutInt(i);
	int j=Close(i);
	PutInt(j);
	return 0;
}
