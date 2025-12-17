/*
 * File: main.c
 *
 * This file is part of the SimuSil library
 *
 * Compile: $ gcc -Wall -I./include -pthread -L./lib 3_Parallel.c
 *                -lsimusil -lrt -o 3_Parallel
 *
 * Author: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on October 27th, 2016
 * Modified 2016-11-01: added list of thread to be canceled
 * Modified 2016-11-02: added debug levels
 */

#include <stdio.h>  /* printf(3)                                      */
#include <stdlib.h> /* exit(3), EXIT_SUCCESS                          */
#include <signal.h> /* signal(2), SIGINT, SIG_DFL                     */
#include <time.h>   /* clock_nanosleep(2)                             */
#include <pthread.h>/* pthread stuff (_create,_exit,_setdettachstate) */
#include <limits.h>
#include "simusil.h"
#include "timing.h"

static pthread_mutex_t cannon_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t edf_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  edf_cv    = PTHREAD_COND_INITIALIZER;

/* WORKER STUFF                                                       */
/* struct to pass all info to thread                                  */
typedef struct{
  int id;
  pthread_t thid;
  Radar_ptr_t   r;
  Cannon_ptr_t  c;
  Missile_ptr_t m;
  
  long long deadline_ns;    /* EDF: deadline estimado (ns) = tiempo restante hasta impacto */
} Args_t;

/* GLOBALs: needed by SIGINT handlers                                 */
World_ptr_t w;   /* to be destroyed at exit                           */
Bomber_ptr_t b;  /* start/stop bombing                                */
List_ptr_t l;    /* list of living threads                            */
List_ptr_t l_edf;   /* lista EDF (ordenada por deadline) para decidir quién usa el cañón */
pthread_attr_t attr;

void destroyWorker(void *arg)
{
  Args_t *x=arg;
  pthread_cancel(x->thid);
  free(x);
}

void destroyer(int signum)
{
  signal(SIGINT,SIG_DFL); /* restore default-TERM during destroyWorld */
  destroyList(l,destroyWorker);
  destroyList(l_edf,NULL);
  pthread_attr_destroy(&attr);
  destroyWorld(w);
  exit(EXIT_SUCCESS);
}

void handler(int signum)
{
  stopBombing(b);
  printf("Press ctrl+C to finish\n"); /* bad idea: printf in handler! */
  signal(SIGINT,destroyer);
}

/* Devuelve 1 si 'a' debe ir después que 'b' (D_a > D_b) */
int edf_cmp(void *a, void *b)
{
  Args_t *x = (Args_t*)a;
  Args_t *y = (Args_t*)b;
  return (x->deadline_ns > y->deadline_ns);
}




static long long estimate_deadline_ns(Radar_ptr_t r, Missile_ptr_t m, Pos *p_out)
{
  Pos p1, p2;
  struct timespec t1, t2;
  MissileState sm;

  long long est = LLONG_MAX;

  clock_gettime(CLOCK_MONOTONIC, &t1);
  sm = radarReadMissile(r, m, &p1);
  if (sm == MISSILE_ACTIVE)
  {
    clock_nanosleep(CLOCK_MONOTONIC, 0, &TS_10ms, NULL);

    clock_gettime(CLOCK_MONOTONIC, &t2);
    sm = radarReadMissile(r, m, &p2);
    *p_out = p2;
  }
  else
  {
    *p_out = p1;
  }

  if (sm == MISSILE_ACTIVE)
  {
    long long dt = (long long)diff_ts(t2, t1); // ns 

    if (dt <= 0) 
    {
      dt = 1;
    }

    long long dy = (long long)(p2.y - p1.y);

    if (dy < 0) 
    {
      long long denom = -dy;
      
      est = ((long long)p2.y * dt) / denom;
      if (est < 0) 
      {
        est = 0;
      }
    }
  }

  return est;
}





/* thread code */
void *searchAndDestroy(void *arg)
{
  Args_t *x=arg;
  MissileState sm;
  Pos p;
  const struct timespec stallTime=(struct timespec){0, 1000000};/* 1ms*/
  const struct timespec relaxTime=(struct timespec){0,10000000};/*10ms*/

  list_enqueue(x,x->id,l);

  sm=radarReadMissile(x->r,x->m,&p);
  if (sm != MISSILE_ACTIVE)
  {
    printf("[%03d] Warning: missing missile!\n",x->id);
    printf("[%03d] \tBetween Wait & Read:\n",x->id);
    printf("[%03d] \t\tMissile impacted on ground, or\n",x->id);
    printf("[%03d] \t\tMissile intercepted by a spurious previous shoot\n",x->id);
  }
  else
  {

    x->deadline_ns = estimate_deadline_ns(x->r, x->m, &p);
    pthread_mutex_lock(&edf_mutex);
    list_insert(x, edf_cmp, x->id, l_edf);
    pthread_cond_broadcast(&edf_cv);   // hay nueva info o puede cambiar el primero
    pthread_mutex_unlock(&edf_mutex);

    pthread_mutex_lock(&edf_mutex);
    Args_t *first = (Args_t*)list_dequeue(l_edf, 1);
    while (first != x) 
    {
      list_insert(first, edf_cmp, first->id, l_edf);

      first = (Args_t*)list_dequeue(l_edf, 1); /* bloquea hasta que haya alguien */
      
      pthread_cond_wait(&edf_cv, &edf_mutex);
    }
    pthread_mutex_unlock(&edf_mutex);

    pthread_mutex_lock(&cannon_mutex);
    printf("[%03d] ---> Moving cannon to position %d\n",x->id,p.x);
    cannonMove(x->c,p.x);
    clock_nanosleep(CLOCK_MONOTONIC,0,&stallTime,NULL); /*espera antes*/
    cannonFire(x->c);
    pthread_mutex_unlock(&cannon_mutex);

    pthread_mutex_lock(&edf_mutex);
    pthread_cond_broadcast(&edf_cv);
    pthread_mutex_unlock(&edf_mutex);


    /* bucle de monitorizacion hasta intercepcion */
    while ((sm=radarReadMissile(x->r,x->m,&p)) == MISSILE_ACTIVE)
    {
	  //printf("[%03d] ---> en seguimiento (%d,%d)\n",x->id,p.x,p.y);
      clock_nanosleep(CLOCK_MONOTONIC,0,&relaxTime,NULL);
    }
    switch (sm)
    {
      case MISSILE_INTERCEPTED:
           printf("[%03d] ---> Interceptado en (%d,%d)\n",x->id,p.x,p.y);
           break;
      case MISSILE_IMPACTED:
           printf("[%03d] ---> Impacta en suelo (%d)\n",x->id,p.x);
           break;
      case MISSILE_ERROR:
      default:
           printf("[%03d] ---> Error de seguimiento del misil\n",x->id);
    }
  }

  list_remove(x,l);
  free(x);
  pthread_exit(NULL);
}


/*
 * Main code
 *
 * Master thread: wait missile and creates dettached worker for it
 */
int main(int argc, char *argv[])
{
  Radar_ptr_t r;
  Cannon_ptr_t c;
  Args_t *x;
  static int workerCount=0;

  debug_setlevel(1);

  w=createWorld("TRSM 2016",1,2); /* worldname,1 cannon,debug level 2 */
  b=getBomber(w);
  r=getRadar(w);
  c=getCannon(w,0); /* [0..n-1] cannon number 0 (first of one)        */
  l=createList("Threads","worker",2); /* listname,elemname,debuglevel */

  l_edf = createList("EDF", "job", 2);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

  signal(SIGINT,handler);

  printf("Press ctrl+C to stop bombing\n");
  startBombing(b);
  while(1)
  {
    x=(Args_t*)malloc(sizeof(Args_t));
    x->id=workerCount++;
    x->r=r;
    x->c=c;
    x->m=radarWaitMissile(r);

    x->deadline_ns = LLONG_MAX;

    pthread_create(&x->thid,&attr,searchAndDestroy,(void*)x);
  }
  return 0; /* never reached!                                         */
}
