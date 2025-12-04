/*
 * File: barbershop_2.c
 *
 * Compile: $ gcc barbershop_2.c -o barbershop_2 -pthread
 *          or
 *          $ make barbershop_2
 *
 * Adapted by: Sergio Romero Montiel <sromero@uma.es>
 *
 * Last modified on January 12th, 2024
 */

#include <stdio.h>
#include <stdlib.h> /*exit(3)*/
#include <unistd.h> /*sleep(3)*/
#include <pthread.h>
#include <semaphore.h>

#define MAX_THREADS 32

#define core_barbero(t) core(t+1)
#define core_cajero(t) core(t+1)

/* rutinas de los threads */
void *cliente(void *);
void *barbero(void *);
void *cajero(void *);

/* rutina auxiliar para consumir CPU */
double a = 1.0;
double core(int times)
{
  for (long i=0; i<10000000*times; i++) a=1.0+1.0/a;
  return a;
}

/* inicializados a cero, para detectar los NO indicados en linea de comandos */
int maxcap=0;   /* maxima capacidad de la barberia */
int nsofa=0;     /* numero de sofas */
int nbarbero=0;  /* numero de barberos */
int ncliente=0;  /* numero de clientes */

/* coordinacion y sincronizacion de threads */
pthread_t tid[MAX_THREADS];      /* array de IDs thread (identificadores) */

sem_t capacidad_maxima;
sem_t sofa;
sem_t sillon, coord;
sem_t mutex1;
sem_t cliente_listo, liberar_sillon, pago, factura;
int count=0;


/* funciones para el manejo de la cola circular */
#define TAM_COLA MAX_THREADS
typedef struct{
  int number;
  sem_t* terminacion;
}ticket_t;
ticket_t q[TAM_COLA];
int head, tail;
sem_t hueco, item, me, md;

void iniCola()
{
  head=0;
  tail=0;
  sem_init(&hueco,0,TAM_COLA);
  sem_init(&item,0,0);
  sem_init(&me,0,1);
  sem_init(&md,0,1);
}
void encolar(ticket_t t)
{
  sem_wait(&hueco);
  sem_wait(&me);
  q[tail++]=t;
  if (tail>=TAM_COLA) tail=0;
  sem_post(&me);
  sem_post(&item);
}

ticket_t desencolar()
{
  sem_wait(&item);
  sem_wait(&md);
  ticket_t t=q[head++];
  if(head>=TAM_COLA) head=0;
  sem_post(&md);
  sem_post(&hueco);
  return t;
}

int main(int argc, char *argv[])
{
  long i, limite;

  while (--argc>0) {
    if      (sscanf(argv[argc],"-c%u",&ncliente)==1) ;
    else if (sscanf(argv[argc],"-b%u",&nbarbero)==1) ;
    else if (sscanf(argv[argc],"-m%u",&maxcap)==1)   ;
    else if (sscanf(argv[argc],"-s%u",&nsofa)==1)    ;
    else {
      fprintf(stderr, "Usar: %s [-b#] [-c#] [-m#] [-s#]\n\
where\t-b#\tnumero de barberos\n\
\t-c#\tnumero de cliente\n\
\t-m#\tmaxima capacidad de la barberia\n\
\t-s#\tnumero de plazas en el sofa de la barberia\n\
\t\tmaximo numero de personas (threads): %d\n",
        argv[0], MAX_THREADS);
      exit(1);
    }
  }

/* valores por defecto, si no se han pasado por linea de comandos */
  if ((nbarbero<=0)||(nbarbero+1>MAX_THREADS)) nbarbero=3;  /* numero de barberos */
  if ((ncliente<=0)||(ncliente+nbarbero+1>MAX_THREADS)) ncliente=MAX_THREADS-nbarbero-1; /* numero de clientes */
  limite=ncliente+nbarbero+1;
  if (maxcap<=nbarbero) maxcap=5*nbarbero;   /* maxima capacidad de la barberia */
  if (nsofa<=nbarbero) nsofa=nbarbero+1;     /* numero de sofas */
  iniCola(); /* inicializa la cola de tickets */

  printf("Usando: %d clientes, %d barberos, 1 cajero, %d sofas y maxcap=%d\n",
         ncliente,nbarbero,nsofa,maxcap);

  sem_init(&capacidad_maxima, 0, maxcap);
  sem_init(&sofa, 0, nsofa);
  sem_init(&sillon, 0, nbarbero);
  sem_init(&coord, 0, nbarbero); /* maximo numero de servidores en ejecucion */
  sem_init(&mutex1, 0, 1); /* seccion critica del contador de tickets */
  sem_init(&cliente_listo, 0, 0);
  sem_init(&liberar_sillon, 0, 0);
  sem_init(&pago, 0, 0);
  sem_init(&factura, 0, 0);

  /* crea cajero */
  pthread_create(&tid[0], NULL, cajero, (void *)0);
  /* crea los threads barberos, a cada uno se le asigna un numero */
  for (i=1; i<=nbarbero; i++)
    pthread_create(&tid[i], NULL, barbero, (void *)i);
  /* crea clientes, no se le asigna numero */
  for (i=nbarbero+1; i<limite; i++)
    pthread_create(&tid[i], NULL, cliente, (void *)0);
  /* esperar a que terminen todos los clientes
   * los barberos y cajeros (servidores) no terminan */
  for (i=nbarbero+1; i<limite; i++)
    pthread_join(tid[i], NULL);

  printf("Los clientes se han ido...\n");
  exit(0);
}



void* cliente(void *arg)
{
  ticket_t ticket;
  int minumero;
  sem_t terminacion;

  sem_init(&terminacion, 0, 0);

  printf("Cliente caminando por la calle.\n");
  sem_wait(&capacidad_maxima);
  printf("Cliente entra en la barberia.\n");
  sem_wait(&mutex1);
  minumero=count;
  count++;
  sem_post(&mutex1);
  sem_wait(&sofa);
  printf("C%d se sienta en el sofa.\n",minumero);
  sem_wait(&sillon);
  printf("C%d se levanta del sofa.\n",minumero);
  sem_post(&sofa);
  printf("C%d se sienta en el sillon del barbero.\n",minumero);
  ticket.number=minumero;
  ticket.terminacion=&terminacion;
  encolar(ticket);
  printf("C%d listo para que le corten el pelo.\n",minumero);
  sem_post(&cliente_listo);
  /* Accion del barbero */
  sem_wait(&terminacion);
  printf("C%d se levanta del sillon del barbero.\n",minumero);
  sem_post(&liberar_sillon);
  printf("C%d paga al cajero y espera la factura.\n",minumero);
  sem_post(&pago);
  /* Accion del cajero */
  sem_wait(&factura);
  printf("C%d coge la factura y sale de la barberia.\n",minumero);
  sem_post(&capacidad_maxima);
  return NULL;
}


void* barbero(void *arg)
{
  long n=(long)arg;  /* numero del barbero */
  ticket_t ticket;
  int micliente;
  sem_t *terminacion;

  printf("B%ld listo para trabajar.\n", n);
  while (1) {
    printf("B%ld espera un cliente.\n", n);
    sem_wait(&cliente_listo);
    ticket=desencolar();
    micliente=ticket.number;
    terminacion=ticket.terminacion;
    sem_wait(&coord);
    printf("B%ld comienza a cortar el pelo al cliente C%d.\n",n,micliente);
    core_barbero(rand()%5); /* Accion del servidor barbero*/
    printf("B%ld termina de cortar el pelo al cliente C%d.\n",n,micliente);
    sem_post(&coord);
    sem_post(terminacion);
    sem_wait(&liberar_sillon);
    printf("B%ld limpia el sillon usado por el cliente C%d.\n", n, micliente);
    sem_post(&sillon);
  }
}


void* cajero(void *arg)
{
  printf("$ listo para trabajar.\n");
  while (1) {
    printf("$  espera a que un cliente pague.\n");
    sem_wait(&pago);
    sem_wait(&coord);
    printf("$  acepta el pago.\n");
    core_cajero(0); /* Accion del servidor cajero */
    printf("$  entrega la factura.\n");
    sem_post(&coord);
    sem_post(&factura);
  }
}

