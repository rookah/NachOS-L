#include "system.h"

int do_Create(char *filename, bool isDir);
int do_Open(char *filename);
int do_Read(int fd, char* into, int numBytes);
int do_Write(int fd, char* from, int numBytes);
int do_Close(int fd);
int do_Rm(char *filename);
