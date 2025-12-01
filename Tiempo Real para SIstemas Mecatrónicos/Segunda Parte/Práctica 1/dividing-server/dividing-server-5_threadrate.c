/*
 * File:   dividing-server-5_threadrate.c
 *
 * Compile:  gcc dividing-server-5_threadrate.c -o dividing-server-5_threadrate -lpthread -Wall
 *
 * Author: Guillermo Pérez Trabado <guille@ac.uma.es>
 * Modified by: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on 6 de abril de 2016, 14:09
 * Modified on April 20th, 2016
 *    fixed: some warnings in compilation (casting & format)
 *    fixed: clean stdin when not a valid entry
 *    added: stops on EOF when processing input from file
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>

/* This is number divided */
#define N (1000000)
/* Number of threads to create */
#define TIMES (10000)


void *worker(void *argv)
{
  int local_number = *((int *) argv);
  int local_result = 0;

  local_result += N / local_number;
  pthread_exit(NULL);
}

int main(int argc, char **argv)
{
  int number, status, i, n;
  pthread_t th1;
  struct timespec time1, time2;
  double interval;

  if (argc > 1)
  {
    fprintf(stderr, "Usage: %s\n\tShows an example of how exceptions in threads kill the containing process.\n", argv[0]);
    fprintf(stderr, "\tInput values:\n\t\tAny integer number: Thread divides %d by given number.\n", N);
    fprintf(stderr, "\t\t0: Thread divides %d by 0.\n", N);
    fprintf(stderr, "\t\t-1: Thread uses a NULL pointer to access memory.\n");
    return (EXIT_FAILURE);
  }

  while (1)
  {
    printf("[main thread] Give me a number: ");
    fflush(stdout);
    n = scanf("%d", &number);
    if ((1 == n) && (number > 0))
    {
      clock_gettime(CLOCK_REALTIME, &time1);
      for (i = 0; i < TIMES; i++)
      {
        status = pthread_create(&th1, NULL, worker, (void *) &number);
        if (status != 0)
        {
          fprintf(stderr, "[main thread] pthread_create() failed with error %d.\n", status);
        }
        else
        {
          pthread_join(th1, NULL);
        }
      }
      clock_gettime(CLOCK_REALTIME, &time2);
      interval = (time2.tv_sec * 1E9 + time2.tv_nsec)
                -(time1.tv_sec * 1E9 + time1.tv_nsec);
      fprintf(stderr, "[main thread] %d threads in %lf seconds (%lf workers/sec, %lf sec/worker)\n",
              TIMES, interval/1E9, TIMES/(interval/1E9), (interval/1E9)/TIMES);
    }
    else if (EOF == n) break; // input from file
    else
    {
      scanf("%*[^\n]%*c"); // clear stdin
      fprintf(stderr, "[main thread] Not a valid number!\n");
    }
  }
  return (EXIT_SUCCESS);
}
