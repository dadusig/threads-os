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

int create_mutex(key_t key, int sflags);
int destroy_mutex(int sid);
int mutex_down (int sid);
int mutex_up(int sid);

int main(int argc, char *argv[])
{
	int done = 0;

	int n_strings = argc - 2; //number of given strings

	if (n_strings >= 1)
	{
		int mut = create_mutex(IPC_PRIVATE, 0600);

		int rep = atoi(argv[1]);
		argv = argv + 2;

		pid_t pid;

		for (int i = 0; i < n_strings; i++)
		{
			if (done == 0)
			{
				pid = fork();
				if (pid == 0)
				{
					for (int j = 0; j < rep; j++)
					{
						mutex_down(mut); //wait
						display(argv[i]);
						mutex_up(mut); //post
					}
					done = 1;
				}
			}

		}

		pid_t child_pid, wpid;
		int status = 0;

		while ((wpid = wait(&status)) > 0);

		if (pid > 0)
		{
			destroy_mutex(mut);
		}

	}

	return 0;
}

int create_mutex(key_t key, int sflags)
{
    //create, init to 1, return
    int sid = semget(key, 1, sflags);
    semctl(sid, 0, SETALL, 1);
    return sid;
}

int destroy_mutex(int sid)
{
    return semctl (sid, 1, IPC_RMID);
}

int mutex_up(int sid)
{
    //increment and wake a waiting process (if any)
    struct sembuf up = {0, 1, 0};
    return semop (sid, &up, 1);
}

int mutex_down (int sid)
{
    //decrement and block if the result is negative
    struct sembuf down = {0, -1, 0};
    return semop (sid, &down, 1);
}
