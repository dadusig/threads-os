#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>          /* errno, ECHILD            */
#include "util.h"

/* We must define union semun ourselves.  */

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short int *array;
  struct seminfo *__buf;
};

/* Obtain a binary semaphore's ID, allocating if necessary.  */

int binary_semaphore_allocation (key_t key, int sem_flags)
{
  return semget (key, 1, sem_flags);
}

/* Deallocate a binary semaphore.  All users must have finished their
   use.  Returns -1 on failure.  */

int binary_semaphore_deallocate (int semid)
{
  union semun ignored_argument;
  return semctl (semid, 1, IPC_RMID, ignored_argument);
}

/* Initialize a binary semaphore with a value of one.  */

int binary_semaphore_initialize (int semid)
{
  union semun argument;
  unsigned short values[1];
  values[0] = 1;
  argument.array = values;
  return semctl (semid, 0, SETALL, argument);
}

/* Wait on a binary semaphore.  Block until the semaphore value is
   positive, then decrement it by one.  */

int binary_semaphore_wait (int semid)
{
  struct sembuf operations[1];
  /* Use the first (and only) semaphore.  */
  operations[0].sem_num = 0;
  /* Decrement by 1.  */
  operations[0].sem_op = -1;
  /* Permit undo'ing.  */
  operations[0].sem_flg = SEM_UNDO;

  return semop (semid, operations, 1);
}

/* Post to a binary semaphore: increment its value by one.  This
   returns immediately.  */

int binary_semaphore_post (int semid)
{
  struct sembuf operations[1];
  /* Use the first (and only) semaphore.  */
  operations[0].sem_num = 0;
  /* Increment by 1.  */
  operations[0].sem_op = 1;
  /* Permit undo'ing.  */
  operations[0].sem_flg = SEM_UNDO;

  return semop (semid, operations, 1);
}


int main(int argc, char *argv[])
{
	int done = 0;

	int n_strings = argc - 2; //number of given strings

	if (n_strings >= 1)
	{
		int mut = binary_semaphore_allocation(IPC_PRIVATE, 0600);
		binary_semaphore_initialize(mut);

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
						binary_semaphore_wait(mut);
						display(argv[i]);
						binary_semaphore_post(mut);
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
			binary_semaphore_deallocate(mut);
		}

	}

	return 0;
}
