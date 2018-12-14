#include "syscall.h"

int main() {
  char string[200];
  GetString(string, 200);
  PutString(string);

  Halt();
}
