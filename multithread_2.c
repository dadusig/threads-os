#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "util.h"

/*- thread runner parameters -*/
typedef struct
{
    int rep;
    char* string;
}thread_args_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t bar;

/*- thread function -*/
void* thread_runner(void *p);

int main(int argc, char *argv[])
{
    int n_strings = argc - 2; //number of given strings

    if (n_strings >= 1)
    {
        pthread_barrier_init(&bar, NULL, argc-2);

        thread_args_t params[argc-2];

        pthread_t tid[argc-2];

        for (int i = 2; i < argc; i++)
        {
            params[i-2].string = argv[i];
            params[i-2].rep = atoi(argv[1]);
            pthread_create(&tid[i-2], NULL, thread_runner, &params[i-2]);
        }

        for (int i = 0; i < argc-2; i++)
            pthread_join(tid[i], NULL);

        pthread_barrier_destroy(&bar);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}

void* thread_runner(void *p)
{
    pthread_mutex_lock(&mutex);
    init();
    pthread_mutex_unlock(&mutex);

    pthread_barrier_wait(&bar);

    thread_args_t *param = (thread_args_t *) p;

    for (int i = 0; i < param->rep; i++)
    {
        pthread_mutex_lock(&mutex);
        display(param->string);
        pthread_mutex_unlock(&mutex);
    }
}
