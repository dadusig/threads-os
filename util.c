/* DO NOT EDIT THIS FILE!!!  */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "util.h"


void init()
{
  printf("STARTING: pid %i, tid %li\n", getpid(), pthread_self());
}

void display(char *str)
{
  printf("[%i,%li] ", getpid(), pthread_self());
  fflush(stdout);

  char *p;
  for (p=str; *p; p++)
  {
    write(1, p, 1);
    usleep(100);
  }

  putchar('\n');
}
