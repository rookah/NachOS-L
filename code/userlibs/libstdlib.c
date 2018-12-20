#include "syscall.h"

#include <stdlib.h>

void exit(int status)
{
	Exit(status);
}

void *memset(void *ptr, int value, size_t num)
{
	unsigned char *ptru = (unsigned char *)ptr;

	for (num -= 1; num >= 0; --num)
		ptru[num] = value;

	return ptr;
}