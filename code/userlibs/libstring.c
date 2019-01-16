#include "syscall.h"
#include <stddef.h>

int streq(char *s1, char *s2)
{
	if (s1 == NULL || s2 == NULL)
		return 0;

	int i = 0;
	while (s1[i] != '\0') {
		if (s1[i] != s2[i])
			return 0;
		i++;
	}

	if (s2[i] == '\0')
		return 1;
	else
		return 0;
}
