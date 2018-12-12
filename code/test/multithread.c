#include "syscall.h"

void thread(void *d){

	while(1)
	PutChar('a');
}	


int main(){

	UserThreadCreate(thread,(void *)0);
	for(int i=0;i<10;i++){
		PutChar('b');
	}
	// Halt();
	return 0;
}