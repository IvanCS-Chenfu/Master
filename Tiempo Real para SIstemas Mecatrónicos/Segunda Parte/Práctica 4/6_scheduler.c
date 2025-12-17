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

static pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  q_cv    = PTHREAD_COND_INITIALIZER;

static int sweep_dir = +1;
static int cannon_x = 0;

/* WORKER STUFF                                                       */
/* struct to pass all info to thread                                  */
typedef struct{
  int id;
  pthread_t thid;
  Radar_ptr_t   r;
  Cannon_ptr_t  c;
  Missile_ptr_t m;
  
  int target_x;                /* coordenada X fija del misil (objetivo del cañón) */
  pthread_mutex_t fired_mtx;
  pthread_cond_t  fired_cv;
  int fired;                   /* 0 = aún no disparado, 1 = ya disparado */

} Args_t;

/* GLOBALs: needed by SIGINT handlers                                 */
World_ptr_t w;   /* to be destroyed at exit                           */
Bomber_ptr_t b;  /* start/stop bombing                                */
List_ptr_t l;
List_ptr_t q_right; /* x >= cannon_x (orden ascendente) */
List_ptr_t q_left;  /* x <  cannon_x (orden descendente) */
int qn_right = 0;
int qn_left  = 0;

pthread_t scheduler_th;   /* Thread independiente para la función sheduler */

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
  destroyList(q_right, NULL);
  destroyList(q_left, NULL);
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

/* Devuelve 1 si las distancias son mayores/menores */
int cmp_x_asc(void *a, void *b)
{
  Args_t *x = (Args_t*)a;
  Args_t *y = (Args_t*)b;
  return (x->target_x > y->target_x);
}

int cmp_x_desc(void *a, void *b)
{
  Args_t *x = (Args_t*)a;
  Args_t *y = (Args_t*)b;
  return (x->target_x < y->target_x);
}




void* scheduler(void *arg)
{
  (void)arg;

  const struct timespec stallTime = TS_1ms;

  while (1)
  {
    Args_t *job = NULL;

    pthread_mutex_lock(&q_mutex);

    /* Esperar a que haya algo en alguna cola */
    while (qn_right == 0 && qn_left == 0) 
    {
      pthread_cond_wait(&q_cv, &q_mutex);
    }

    /* Si no hay objetivos en la dirección actual, invierte */
    if ((sweep_dir == +1 && qn_right == 0) || (sweep_dir == -1 && qn_left == 0)) 
    {
      sweep_dir = -sweep_dir;
    }
    else
    {
      sweep_dir = sweep_dir;
    }

    /* Extraer siguiente objetivo según dirección */
    if (sweep_dir == +1) 
    {
      job = (Args_t*)list_dequeue(q_right, 1);
      if (job != NULL)
      {
        qn_right--;
      } 
    }
    else
    {
      job = (Args_t*)list_dequeue(q_left, 1);
      if (job != NULL)
      {
        qn_left--;
      }
    }

    pthread_mutex_unlock(&q_mutex);

    if (job != NULL)
    {
      /* Usar el cañón (exclusión mutua) */
      pthread_mutex_lock(&cannon_mutex);

      printf("[SCH] ---> Move cannon to %d (dir=%d)\n", job->target_x, sweep_dir);

      cannonMove(job->c, job->target_x);
      clock_nanosleep(CLOCK_MONOTONIC, 0, &stallTime, NULL);
      cannonFire(job->c);

      cannon_x = job->target_x;

      pthread_mutex_unlock(&cannon_mutex);

      /* Avisar al worker de que ya se ha disparado por él */
      pthread_mutex_lock(&job->fired_mtx);
      job->fired = 1;
      pthread_cond_signal(&job->fired_cv);
      pthread_mutex_unlock(&job->fired_mtx);

      /* Despertar por si alguien estaba esperando cambios */
      pthread_mutex_lock(&q_mutex);
      pthread_cond_broadcast(&q_cv);
      pthread_mutex_unlock(&q_mutex);
    }
  }
}





/* thread code */
void *searchAndDestroy(void *arg)
{
  Args_t *x=arg;
  MissileState sm;
  Pos p;

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

    x->target_x = p.x;

    /* Insertar en la cola correcta según posición actual y dirección */
    pthread_mutex_lock(&q_mutex);
    if (x->target_x >= cannon_x)
    {
      list_insert(x, cmp_x_asc, x->id, q_right);
      qn_right++;
    }
    else
    {
      list_insert(x, cmp_x_desc, x->id, q_left);
      qn_left++;
    }
    pthread_cond_broadcast(&q_cv);
    pthread_mutex_unlock(&q_mutex);

    /* Esperar a que el scheduler dispare */
    pthread_mutex_lock(&x->fired_mtx);
    while (!x->fired)
    {
      pthread_cond_wait(&x->fired_cv, &x->fired_mtx);
    }
    pthread_mutex_unlock(&x->fired_mtx);


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

  q_right = createList("Q_RIGHT", "job", 2);
  q_left  = createList("Q_LEFT",  "job", 2);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

  signal(SIGINT,handler);

  pthread_create(&scheduler_th, NULL, scheduler, NULL);     /* Nuevo thread para realizar el scheduler */

  printf("Press ctrl+C to stop bombing\n");
  startBombing(b);
  while(1)
  {
    x=(Args_t*)malloc(sizeof(Args_t));
    x->id=workerCount++;
    x->r=r;
    x->c=c;
    x->m=radarWaitMissile(r);

    x->target_x = 0;
    x->fired = 0;

    pthread_create(&x->thid,&attr,searchAndDestroy,(void*)x);
  }
  return 0; /* never reached!                                         */
}
