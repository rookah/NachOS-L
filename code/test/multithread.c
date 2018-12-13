#include "syscall.h"

void thread(void *d){

	while(1);
	// PutChar('a');
}	


int main(){

	UserThreadCreate(thread,(void *)0);
	while(1){
		// PutChar('b');
	}
	// Halt();
	return 0;
}