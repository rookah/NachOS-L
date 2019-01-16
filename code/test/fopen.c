#include "syscall.h"

int main(){	

	int i =Open("test");
	char buff[2];
	Read(i,buff,2);
	PutString(buff);
	return 0;
}