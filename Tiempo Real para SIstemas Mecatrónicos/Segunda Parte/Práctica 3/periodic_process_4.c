/*
 * File: periodic_process_4.c
 *
 * Compile: $ gcc periodic_process_4.c -o periodic_process_4 -lrt
 *          or
 *          $ make periodic_process_4
 *
 * Author: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on October 20th, 2016
 */

#include <stdlib.h>  // exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <stdint.h>  // uint64_t
#include <stdio.h>   // printf(), getchar(), perror(), EOF
#include <signal.h>  // sigaction(), sigemptyset(), SIGRTMIN, SA_SIGINFO,...
#include <time.h>    // clock_gettime(), timer_create(), timer_settime(),CLOCK_MONOTONIC,...
#include <errno.h>   // errno, EINTR

// funciones de manejo de error y de aritmetica con el tiempo
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define NSECS_PER_SEC 1000000000UL
#define DIF_TS(a,b) ((a.tv_sec-b.tv_sec)*NSECS_PER_SEC+(a.tv_nsec-b.tv_nsec))
#define timeAdd(a,b)                  \
  do {                                \
    a.tv_sec+=b.tv_sec;               \
    a.tv_nsec+=b.tv_nsec;             \
    if (a.tv_nsec >= NSECS_PER_SEC) { \
      a.tv_sec++;                     \
      a.tv_nsec-=NSECS_PER_SEC;       \
    }                                 \
  } while(0)
#define timeSub(a,b)                  \
  do {                                \
    a.tv_sec-=b.tv_sec;               \
    a.tv_nsec-=b.tv_nsec;             \
    if (a.tv_nsec < 0) {              \
      a.tv_sec--;                     \
      a.tv_nsec+=NSECS_PER_SEC;       \
    }                                 \
  } while(0)




// periodo y estructuras de tiempo para observar el comportamiento
const struct timespec period={1,0}; // one second, zero nanos
static struct timespec prev, next={0,0};



// el manejador: la funcion objetivo
void payloadFunction()
{
  struct timespec ts;
  uint64_t i;

  // arrival time
  clock_gettime(CLOCK_MONOTONIC,&ts);
  if (next.tv_sec==0 && next.tv_nsec==0) next=ts;
  else printf("%010lu\t%010lu\t%010lu\t\t%010ld\n",
              ts.tv_sec,ts.tv_nsec, // arrival time [s,ns]
              DIF_TS(ts,prev),      // effective period
              DIF_TS(ts,next));     // cummulative drift
  prev=ts;
  timeAdd(next,period);  // next <- next + period

  /*
   * Here, payload code, p.ej. compute golden ratio
   */
  double a=1.0;
  for (i=0; i<1000000; i++) a=1.0+1.0/a;
  return;
}


int main(int argc, char *argv[])
{
  struct sigaction sa;  // la accion que realiza el receptor
  timer_t timerid;    // para poder destruirlo cuando no haga falta
  struct itimerspec its;  // especificacion de tiempos del temporizador
  struct sigevent   sev;  // el evento que envia el temporizador

  printf("Pulsa ENTER para terminar\n");
  printf("Tiempo de llegada [s,ns]\tPeriodo efectivo [ns]\tDeriva acumulativa [ns]\n");

  // Establecer el manejador de SIGNAL SIGRTMIN
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = payloadFunction;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGRTMIN, &sa, NULL) == -1)
    errExit("sigaction");

  // Crear el timer
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = &timerid;
  if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1)
    errExit("timer_create");

  // Arranca el timer
  its.it_value=period;
  its.it_interval=period;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
    errExit("timer_settime");

  while (EOF==getchar()) // espera a que se pulse una tecla para terminar
  {
    if (errno==EINTR) continue; // cada vez que llegue un signal getchar retorna con error EINTR
    else break;
  }
  timer_delete(timerid);

  exit(EXIT_SUCCESS);
}
