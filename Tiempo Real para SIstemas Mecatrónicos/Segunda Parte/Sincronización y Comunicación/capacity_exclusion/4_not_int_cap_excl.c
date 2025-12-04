#include <stdio.h>
#include <stdlib.h> /*exit(3)*/
#include <pthread.h>

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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int available = 5;

int main(int argc, char *argv[])
{
  printf("Initial available resources: %d\n", available);
  for (long i=1; i < MAX_THREADS; i++)
    pthread_create(&tid[i], NULL, worker, (void *)i);
  for (long i=1; i < MAX_THREADS; i++)
    pthread_join(tid[i], NULL);

  exit(EXIT_SUCCESS);
}

int cond_evaluation(int id, int available, int required)
{
	int cond = (available >= required);
	printf("Worker %02d evaluates condition: %s (required: %d < available: %d)\n", id, cond? "True": "False", required, available);
	return cond;
}

void* worker(void *arg)
{
  int n = (long)arg;
  int required = 1 + rand()%3; // try %3, %4, %5
  int iters = required + rand()%5;
  printf("Worker %02d created (iters: %d), resources required %d\n", n, iters, required);
  pthread_mutex_lock(&mutex);
  while (!cond_evaluation(n, available, required)) pthread_cond_wait(&cond, &mutex);
  available -= required;
  pthread_mutex_unlock(&mutex);
  printf("Worker %02d enters CS using %d (remainder available: %d)\n", n, required, available);
  core(iters);
  printf("Worker %02d exits CS freeing %d\n", n,required);
  pthread_mutex_lock(&mutex);
  available += required;
  printf("Worker %02d ends, resources available: %d\n", n, available);
  pthread_cond_signal(&cond);
  //pthread_cond_broadcast(&cond);	// smaller ones overtake biggers
  pthread_mutex_unlock(&mutex);
  return NULL;
}

