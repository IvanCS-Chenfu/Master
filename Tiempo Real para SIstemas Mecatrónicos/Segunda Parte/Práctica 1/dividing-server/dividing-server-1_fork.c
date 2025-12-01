/*
 * File:   dividing-server-1_fork.c
 *
 * Compile: gcc dividing-server-1_fork.c -o dividing-server-1_fork -Wall
 *
 * Author: Guillermo Pérez Trabado <guille@ac.uma.es>
 * Modified by: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on 6 de abril de 2016, 14:09
 * Modified on April 20th, 2016
 *    fixed: some warnings in compilation (casting & format)
 *    fixed: clean stdin when not a valid entry
 *    fixed: child process show its own pid correctly
 *    added: stops on EOF when processing input from file
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/* This is the number to be divided */
#define N (1000000)
/* The input to make child proccess to access through a null pointer */
#define ZERO_PAGE_KEY (-1)

int main(int argc, char **argv)
{
  int number, result, status, n, *c = NULL;
  pid_t pid, pid2;

  if (argc > 1)
  {
    fprintf(stderr, "Usage: %s\n\tShows an example of isolation of exceptions inside workers created with fork().\n", argv[0]);
    fprintf(stderr, "\tInput values:\n\t\tAny integer number: Child process divides %d by given number.\n", N);
    fprintf(stderr, "\t\t0: Child process divides %d by 0.\n", N);
    fprintf(stderr, "\t\t-1: Child process uses a NULL pointer to access memory.\n");
    return (EXIT_FAILURE);
  }

  while (1)
  {
    printf("[PARENT SERVER] Give me a number: ");
    fflush(stdout);
    n = scanf("%d", &number);
    if (1 == n)
    {
      pid = fork();
      switch (pid)
      {
      case -1: // fork() failed, no child process created!
        fprintf(stderr, "[PARENT SERVER] fork() failed!");
        break;
      case 0: // CHILD PROCESS on successful fork()
		pid2 = getpid();
        if (ZERO_PAGE_KEY == number)
        {
          printf("\n[CHILD PROCESS %d] accesing through NULL pointer\n", pid);
          *c = 0;
        }
        else
        {
          printf("\n[CHILD PROCESS %d] Computing %d / %d\n", pid2, N, number);
          result = N / number;
          printf("\n[CHILD PROCESS %d] %d / %d = %d\n", pid2, N, number, result);
        }
        printf("\n[CHILD PROCESS %d] ending process\n", pid2);
        return (EXIT_SUCCESS);
        break;
      default: // PARENT PROCESS on successful fork()
        pid2 = waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
          printf("[PARENT SERVER] Child process %d exited with exit code %d\n", pid2, WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status))
        {
          printf("[PARENT SERVER] Child process %d killed after receiving signal %d (%s)\n", pid2, WTERMSIG(status), strsignal(WTERMSIG(status)));
        }
      }
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
