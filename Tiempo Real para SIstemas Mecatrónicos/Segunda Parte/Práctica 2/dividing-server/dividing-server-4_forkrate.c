/*
 * File:   dividing-server-4_forkrate.c
 *
 * Compile: gcc dividing-server-4_forkrate.c -o dividing-server-4_forkrate -Wall
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
#include <time.h>

/* This is number divided */
#define N (1000000)
/* Number of child process to fork */
#define TIMES (10000)


int main(int argc, char **argv)
{
  int number, result, status, i, n;
  pid_t pid;
  struct timespec time1, time2;
  double interval;

  if (argc > 1)
  {
    fprintf(stderr, "Usage: %s\n\tTesting fork()/exit() rate of the system.\n", argv[0]);
    fprintf(stderr, "\tInput values:\n\t\tAny integer number: Child process divides %d by given number.\n", N);
    return (EXIT_FAILURE);
  }

  while (1)
  {
    printf("[PARENT SERVER] Give me a number: ");
    fflush(stdout);
    n = scanf("%d", &number);
    if ((1 == n) && (number > 0))
    {
      clock_gettime(CLOCK_REALTIME, &time1);
      for (i = 0; i < TIMES; i++)
      {
        pid = fork();
        switch (pid)
        {
        case -1:
          fprintf(stderr, "[PARENT SERVER] fork() failed!");
          break;
        case 0:
          result = N / number;
          return (result);
          break;
        default:
          waitpid(pid, &status, 0);
        }
      }
      clock_gettime(CLOCK_REALTIME, &time2);
      interval = (time2.tv_sec * 1E9 + time2.tv_nsec)
                -(time1.tv_sec * 1E9 + time1.tv_nsec);
      fprintf(stderr, "[PARENT SERVER] %d forks in %lf seconds (%lf workers/sec, %lf sec/worker)\n",
                TIMES, interval/1E9, TIMES/(interval/1E9), (interval/1E9)/TIMES);
    }
    else if (EOF == n) break; // input from file
    else
    {
      scanf("%*[^\n]%*c"); // clear stdin
      fprintf(stderr, "[PARENT SERVER] Not a valid number!\n");
    }
  }
  return (EXIT_SUCCESS);
}
