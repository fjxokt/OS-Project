
#include "inf.h"

#include <stdio.h>
#include <process.h>
#include <string.h>

void inf(int argc, char *argv[])
{
	int i=1, pid;
	char str[10];
	char buf[10];

	pid = 0;

	while (i)
	{
		strcpy(" : ", str);
		strcat(str, itos(i, buf));
		strcat(str, "\n");
		print(str);

		i++;
		sleep(200);
	}

	exit(0);
}
