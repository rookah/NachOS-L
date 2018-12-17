#include <stdio.h>
#include <stdlib.h>

#include "syscall.h"

FILE *stdin = 0;
FILE *stdout = (FILE *)1;
FILE *stderr = (FILE *)2;

int putchar(int chara)
{
	PutChar(chara);
	return chara;
}

int puts(const char *str)
{
	PutString(str);
	return 0;
}

int fgetc(FILE *stream)
{
	if (stream != stdin) {
		return EOF;
	}

	return GetChar();
}

int getc(FILE *stream)
{
	return fgetc(stream);
}

char *fgets(char *str, int n, FILE *stream)
{
	if (stream != stdin) {
		return NULL;
	}

	GetString(str, n);

	return str;
}