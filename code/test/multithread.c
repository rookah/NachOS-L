#include "syscall.h"

void thread3(void *d) {

  // int i=0;
  // PutInt(8);
  for (int i = 0; i < 5; i++) {
    // while(1){
    PutChar('p');
    // PutChar('a');
  }
  UserThreadExit();
  // PutChar('g');
}

void thread(void *d) {

  // int i=0;
  // PutInt(8);
  for (int i = 0; i < 5; i++) {
    // while(1){
    PutChar('g');
    // PutChar('a');
  }
  UserThreadExit();
  // PutChar('g');
}
void thread2(void *d) {
  UserThreadCreate(thread3, (void *)0);

  // int i=0;
  // PutInt(8);
  for (int i = 0; i < 5; i++) {
    // while(1){
    PutChar('t');
    // PutChar('a');
  }
  UserThreadExit();
  // PutChar('g');
}

int main() {

  UserThreadCreate(thread2, (void *)0);
  UserThreadCreate(thread, (void *)0);
  PutChar('s');
  for (int j = 0; j < 1000; j++) {
    // int i =0;
    // while(1){
    // PutChar('b');
    // i++;
  }

  // Halt();
  return 0;
}