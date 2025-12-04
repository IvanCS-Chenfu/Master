#include <stdio.h>
#include <stdlib.h> /*exit(3)*/
#include <pthread.h>
#include <semaphore.h>

#define MAX_THREADS 20
pthread_t tid[MAX_THREADS];
void* worker(void*);

/* CPU intensive function */
double a = 1.0;
double core(int times)
{
  for (long i=0; i<10000000*times; i++) a=1.0+1.0/a;
  return a;
}

sem_t capacity;

int main(int argc, char *argv[])
{
  sem_init(&capacity, 0, 3);
  for (long i=1; i < MAX_THREADS; i++)
    pthread_create(&tid[i], NULL, worker, (void *)i);
  for (long i=1; i < MAX_THREADS; i++)
    pthread_join(tid[i], NULL);

  exit(EXIT_SUCCESS);
}

void* worker(void *arg)
{
  int n = (long)arg;
  int iters = 1 + rand()%5;
  printf("Worker %02d created (iters: %d)\n", n, iters);
  sem_wait(&capacity);
  printf("Worker %02d enters CS\n", n);
  core(iters);
  printf("Worker %02d exits CS\n", n);
  sem_post(&capacity);
  printf("Worker %02d ends\n", n);
  return NULL;
}

