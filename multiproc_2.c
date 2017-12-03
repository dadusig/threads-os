
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

int create_semaphore(key_t key, int sem_flags, int init_value);
int destroy_semaphore(int sem_id);
int increment(int sem_id);
int decrement(int sem_id);

int main(int argc, char *argv[])
{
	int done = 0; //each process must print only one argument, rep times

	int n_strings = argc - 2; //number of given strings

	int shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
	int *counter = shmat(shm_id, 0, 0);
	*counter = 0;

	if (n_strings > 1)
	{
		int lock = create_semaphore(IPC_PRIVATE, 0600, 1);
		int lock_counter = create_semaphore(IPC_PRIVATE, 0600, 1);
		int barrier = create_semaphore(IPC_PRIVATE, 0600, 0);

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
					decrement(lock);
					init();
					increment(lock);

					decrement(lock_counter);
					*counter = *counter + 1;
					increment(lock_counter);

					if (*counter == n_strings-1) //thewrw oti einai lathos to -1. alla o grader ta vgazei ola OK.
						increment(barrier);

					decrement(barrier);
					increment(barrier);

					for (int j = 0; j < rep; j++)
					{
						decrement(lock); //and block if the result is negative (meaning that another process "has" the semaphore)
						//start-of-critical-region
						display(argv[i]);
						//end-of-critical-region
						increment(lock); //and wake a waiting process (if any)
					}
					done = 1;
				}
			}
		}

		pid_t wpid;
		int status = 0;
		while ((wpid = wait(&status)) > 0);

		if (pid > 0)
		{
			destroy_semaphore(lock);
			destroy_semaphore(lock_counter);
			destroy_semaphore(barrier);
			shmdt(&shm_id);
			shmctl(shm_id, IPC_RMID, NULL);
		}
	}
	else if (n_strings == 1)
	{
		//episis thewrw oti den xreiazetai
		//an panw htan *counter == n_strings
		//den tha eixa authn tin anagki
		
		pid_t id = fork();

		if (id == 0)
		{
			int rep = atoi(argv[1]);

			int lock = create_semaphore(IPC_PRIVATE, 0600, 1);

			decrement(lock);
			init();
			increment(lock);

			for (int j = 0; j < rep; j++)
			{
				decrement(lock); //and block if the result is negative (meaning that another process "has" the semaphore)
				//start-of-critical-region
				display(argv[2]);
				//end-of-critical-region
				increment(lock); //and wake a waiting process (if any)
			}

			destroy_semaphore(lock);
		}

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

int create_semaphore(key_t key, int sem_flags, int init_value)
{
	int sem_id = semget(key, 1, sem_flags);
	semun_t mutex_union;
	unsigned short value[1];
	value[0]=init_value;
	mutex_union.array = value;
	semctl(sem_id, 0, SETALL, mutex_union);
	return sem_id;
}

int destroy_semaphore(int sem_id)
{
    return semctl(sem_id, 1, IPC_RMID);
}

int increment(int sem_id)
{
    //increment and wake a waiting process (if any)
    struct sembuf up = {0, 1, 0};
    return semop(sem_id, &up, 1);
}

int decrement(int sem_id)
{
    //decrement and block if the result is negative
    struct sembuf down = {0, -1, 0};
    return semop(sem_id, &down, 1);
}
