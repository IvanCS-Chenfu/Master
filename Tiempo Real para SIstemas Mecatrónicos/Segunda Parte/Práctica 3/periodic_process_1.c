/*
 * File: periodic_process_1.c
 *
 * Compile: $ gcc periodic_process_1.c -o periodic_process_1 -pthread
 *          or
 *          $ make periodic_process_1
 *
 * Author: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on October 20th, 2016
 */

#include <stdlib.h> // exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <stdint.h> // uint64_t
#include <stdio.h> // printf(), getchar(), perror(), fprintf()
#include <pthread.h> // pthread_(FAMILY), clock_(FAMILY)



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



// tipos de datos para crear el thread periodico
typedef void (*p_function_t)();

typedef struct {
  p_function_t function;
  struct timespec period;
} periodicProcessArgument_t;


// El manejador: encargado de dormir un periodo e invocar a la funcion
// nanosleep: usa un intervalo de tiempo relativo para dormir (1 segundo)
// Problemas: No cuenta el tiempo de ejecución de la función, o sea,
//            el periodo es siempre mayor del indicado
static void* periodicProcess(void *argument)
{
  periodicProcessArgument_t *perProc = argument;
  struct timespec delay;

  while(1) {
    perProc->function();
    delay=perProc->period;
    // duerme hasta el instante de la siguiente ejecucion
    clock_nanosleep(CLOCK_MONOTONIC,0,&delay,NULL);
  }
  return NULL;
}


// Esta es la funcion que queremos ejecutar cada segundo
// Hace un calculo para que sea apreciable el tiempo de computacion
// Tambien lleva la cuenta de en que instante es invocada para mostrar
// la deriva local y la deriva acumulativa
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
  pthread_t thid;
  periodicProcessArgument_t periodicProcessArgument;

  printf("Pulsa ENTER para terminar\n");
  printf("Tiempo de llegada [s,ns]\tPeriodo efectivo [ns]\tDeriva acumulativa [ns]\n");

  // configurar el thread periodico: funcion a invocar + periodo
  periodicProcessArgument.period=period;
  periodicProcessArgument.function=payloadFunction;

  // abre el thread detached (para no tener que hacer join)
  if (0 == pthread_create(&thid,NULL,periodicProcess,&periodicProcessArgument))
    pthread_detach(thid);
  else
    fprintf(stderr,"pthread_create: error\n");

  getchar(); // espera a que se pulse una tecla para terminar
  pthread_cancel(thid);

  exit(EXIT_SUCCESS);
}
