#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "util.h"

int main(int argc, char const *argv[])
{
	int rep = atoi(argv[1]);
	int flag=0;

	int counter = 0;

	for (int i = 2; i < argc; i++)
	{

		rep = atoi(argv[1]);

		if (flag == 0)
		{
			pid_t pid = fork();

			if (pid > 0)
			{
				/* code */
				//waitpid(pid, NULL, 0);
			}
			else if ( pid==0 )
			{
				//printf("%s\n", "I am a child");
				for (int j = 0; j < rep; j++)
				{
					display((char*) argv[i]);
				}
				flag = 1;
			}
		}

	}



	return 0;
}
