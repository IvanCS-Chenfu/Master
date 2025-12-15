/*
 * File: periodic_process_5.c
 *
 * Compile: $ gcc periodic_process_5.c -o periodic_process_5 -lrt
 *          or
 *          $ make periodic_process_5
 *
 * Author: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on October 20th, 2016
 */

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define NSECS_PER_SEC 1000000000UL
#define DIF_TS(a,b) ((a.tv_sec-b.tv_sec)*NSECS_PER_SEC+(a.tv_nsec-b.tv_nsec))


// periodo y estructuras de tiempo para observar el comportamiento
const struct timespec period={1,0}; // one second, zero nanos
struct timespec prev, next={0,0};



// el manejador
void payloadFunction()
{
  struct timespec ts;
  uint64_t i;

  // arrival time
  clock_gettime(CLOCK_MONOTONIC,&ts);
  if (next.tv_sec==0 && next.tv_nsec==0) next=ts;
  else printf("%010lu\t%010lu\t%010lu\t\t\t\t%010ld\n",
              ts.tv_sec,ts.tv_nsec, // arrival time [s,ns]
              DIF_TS(ts,prev),      // period
              DIF_TS(ts,next));     // cummulative drift
  prev=ts;
  next.tv_sec+=period.tv_sec;
  next.tv_nsec+=period.tv_nsec;
  if (next.tv_nsec >= NSECS_PER_SEC)
  {
    next.tv_nsec -= NSECS_PER_SEC;
    next.tv_sec++;
  }

  /*
   * Here, payload code, p.ej. compute golden ratio
   */
  double a=1.0;
  for (i=0; i<1000000; i++) a=1.0+1.0/a;
  return;
}



int main(int argc, char *argv[])
{
  timer_t timerid;    // para poder destruirlo cuando no haga falta
  struct itimerspec its;  // especificacion de tiempos del temporizador
  struct sigevent   sev;  // thread que crea el temposizador

  printf("Pulsa ENTER para terminar\n");
  printf("Tiempo de llegada [s,ns]\tPeriodo efectivo (P+deriva local)[ns]\tDeriva acumulativa [ns]\n");

  // Crear el timer
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_notify_function = payloadFunction;
  sev.sigev_notify_attributes = NULL;
  sev.sigev_value.sival_ptr = &timerid;
  if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1)
    errExit("timer_create");

  // Arranca el timer
  its.it_value = period;
  its.it_interval = period;
  if (timer_settime(timerid, 0, &its, NULL) == -1)
    errExit("timer_settime");

  getchar(); // espera a que se pulse una tecla para terminar
  timer_delete(timerid);

  exit(EXIT_SUCCESS);
}
