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
  for (int i = 0; i < 100; i++) {
    // while(1){
    PutChar('g');
    // PutChar('a');
  }
  UserThreadExit();
  // PutChar('g');
}
void thread2(void *d) {
  int id3 = UserThreadCreate(thread3, (void *)0);

  // int i=0;
  // PutInt(8);
  for (int i = 0; i < 5; i++) {
    // while(1){
    PutChar('t');
    // PutChar('a');
  }
  UserThreadJoin(id3);
  UserThreadExit();
  // PutChar('g');
}

int main() {

  int id1 = UserThreadCreate(thread2, (void *)0);
  int id2 = UserThreadCreate(thread, (void *)0);
  UserThreadJoin(id1);
  UserThreadJoin(id2);

  Halt();
  return 0;
}