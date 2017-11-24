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

int create_mutex(key_t key, int sem_flags);
int destroy_mutex(int sid);
int mutex_up(int sid);
int mutex_down (int sid);

int main(int argc, char *argv[])
{
	int done = 0; //each process must print only one argument, rep times

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
					mutex_down(mut); //wait
					init();
					mutex_up(mut); //post

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

		pid_t wpid;
		int status = 0;
		while ((wpid = wait(&status)) > 0);

		if (pid > 0) destroy_mutex(mut);
	}

	return 0;
}

typedef union
{
	int val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short *array;  /* Array for GETALL, SETALL */
	struct seminfo *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
}semun_t;

int create_mutex(key_t key, int sem_flags)
{
	int sem_id = semget(key, 1, sem_flags);
	semun_t mutex_union;
	unsigned short value[1];
	value[0]=1;
	mutex_union.array = value;
	semctl (sem_id, 0, SETALL, mutex_union);
	return sem_id;
}

int destroy_mutex(int sid)
{
    return semctl(sid, 1, IPC_RMID);
}

int mutex_up(int sid)
{
    //increment and wake a waiting process (if any)
    struct sembuf up = {0, 1, 0};
    return semop(sid, &up, 1);
}

int mutex_down (int sid)
{
    //decrement and block if the result is negative
    struct sembuf down = {0, -1, 0};
    return semop(sid, &down, 1);
}
