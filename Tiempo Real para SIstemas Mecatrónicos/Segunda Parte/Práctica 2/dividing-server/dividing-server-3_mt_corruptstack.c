/*
 * File:   dividing-server-3_mt_corruptstack.c
 *
 * Compile: gcc dividing-server-3_mt_corruptstack.c -o dividing-server-3_mt_corruptstack -pthread -Wall
 *
 * Author: Guillermo Pérez Trabado <guille@ac.uma.es>
 * Modified by: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on 6 de abril de 2016, 14:09
 * Modified on April 20th 2016
 *    fixed: corrupt stack in some architectures
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

/* This is number to be divided */
#define N (1000000)
/* The input to make worker thread to access through a null pointer */
#define ZERO_PAGE_KEY (-1)
/* The input to make worker thread to corrupt the main thread stack */
#define CORRUPT_KEY (-2)

int *globalp1;
int *globalp2;

void *worker(void *argv)
{
  int local_number = *((int *) argv);
  int local_result, *stackp, *c = NULL;
  pthread_t tid;

  tid = pthread_self();
  if (ZERO_PAGE_KEY == local_number)
  {
    printf("\t\t[Worker 0x%08lx]: accesing through NULL pointer\n", (long) tid);
    *c = 0; // c points to NULL, trying to write into invalid page!
  }
  else if (CORRUPT_KEY == local_number)
  {
    sleep(1); // wait for the master to update global variables

    printf("\t\t[Worker 0x%08lx]: overwriting stack of main thread\n", (long) tid);
    for (stackp = globalp1; stackp < globalp2; stackp++)
      *stackp = 0;
  }
  else
  {
    printf("\t\t[Worker 0x%08lx]: Computing %d / %d\n", (long) tid, N, local_number);
    local_result = N / local_number;
    printf("\t\t[Worker 0x%08lx]: %d / %d = %d\n", (long) tid, N, local_number, local_result);
  }

  printf("\t\t[Worker 0x%08lx]: ending thread\n", (long) tid);
  pthread_exit(NULL);
}

void calling_something(int *b)
{
  int a;
  /* Get memory addresses surrounding the return stack of this call */
  globalp1 = &a; /* First local variable (on stack, *below* return addrress) */
  globalp2 = b; /* Argument (&number) points on the stack *above* the return address */

  printf("\t[main thread]: going to sleep in subroutine\n");
  sleep(2); /* Wait for thread to overwrite stack */
  printf("\t[main thread]: ending sleep in subroutine\n");
  return; // access to return address on stack (if still available)
}

int main(int argc, char **argv)
{
  int number, status, n;
  pthread_t th1;

  if (argc > 1)
  {
    fprintf(stderr, "Usage: %s\n\tShows an example of how exceptions in threads terminate the containing process.\n", argv[0]);
    fprintf(stderr, "\tInput values:\n\t\tAny integer number: Thread divides %d by given number.\n", N);
    fprintf(stderr, "\t\t0: Thread divides %d by 0.\n", N);
    fprintf(stderr, "\t\t-1: Thread uses a NULL pointer to access memory.\n");
    fprintf(stderr, "\t\t-1: Thread corrupts stack of thread main while it sleeps and exits. Main will crash when returning from function.\n");
    return (EXIT_FAILURE);
  }

  while (1)
  {
    printf("[main thread]: Give me a number: ");
    fflush(stdout);
    n = scanf("%d", &number);
    if (1 == n)
    {
      status = pthread_create(&th1, NULL, worker, (void *) &number);
      if (status != 0)
      {
        fprintf(stderr, "[main thread]: pthread_create() failed with error %d.\n", status);
      }
      else
      {
        /* Just calling something to be busy inside a subroutine */
        calling_something(&number);
        printf("[main thread]: returned from subroutine\n");
        pthread_join(th1, NULL);
      }
    }
    else if (EOF == n) break; // input from file
    else
    {
      scanf("%*[^\n]%*c"); // clear stdin
      fprintf(stderr, "[main thread]: Not a valid number!\n");
    }
  }
  return (EXIT_SUCCESS);
}
