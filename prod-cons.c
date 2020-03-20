/*
* FILE: prod-cons.c
* THMMY, 8th semester, Real Time Embedded Systems: 1st assignment
* Parallel implementation of producer - consumer problem
* Author:
*   Moustaklis Apostolos, 9127, amoustakl@auth.gr
* Compile command with :
*   make prod-con
* Run command example:
*   ./prod-con
* It will create a queue with QUEUESIZE size and nOfProducers / nOfConsumers
*/


#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define QUEUESIZE 15
#define LOOP 5

#define nOfProducers  4
#define nOfConsumers   3

#define nOfFunctions 4

void *producer (void *args);
void *consumer (void *args);



void * sum(void * a){
  int c = *(int *)a + *(int *)a  ;

  printf("HEEEY  (1) : %d \n " , c );
}
void * subtract(void *  a){
    int c =5* (*(int *)a)  ;
  printf("HEEEY  (2) : %d \n " , c );
}
void * mul(void * a){
  int c = *(int *)a ;
  printf("HEEEY (3)  \n");
}
void * dive(void * a){
    int c = *(int *)a ;
  printf("HEEEY  (4) \n");
}

void * (*functions[4]) (void * x) = {sum , subtract , mul , dive};



typedef struct {
  void * (*work)(void *);
  void * arg;
  time_t tim;
} workFunction  ;


workFunction wf ;
struct timeval tv;
int argu=2;

typedef struct {
  workFunction  buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} queue;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, workFunction in);
void queueDel (queue *q, workFunction *out);
int main ()
{

  srand(time(NULL));
  queue *fifo;
  pthread_t pro[nOfProducers], con[nOfConsumers];

  fifo = queueInit ();
  if (fifo ==  NULL) {
    fprintf (stderr, "main: Queue Init failed.\n");
    exit (1);
  }

  for(int i=0; i<nOfConsumers; i++){
    pthread_create(&con[i], NULL, consumer, fifo);
}

  for(int i=0; i<nOfProducers ; i++){
  pthread_create (&pro[i], NULL, producer, fifo);
}


for(int i=0; i<nOfProducers ; i++){
 pthread_join(pro[i], NULL);
}

for(int i=0; i<nOfConsumers ; i++){
  pthread_join(con[i], NULL);
}

  queueDelete(fifo);

  return 0;
}

void *producer (void *q){
  queue *fifo;
  int i;
  fifo = (queue *)q;


  for (i = 0; i < LOOP; i++) {
    pthread_mutex_lock (fifo->mut);

    while (fifo->full) {
      printf ("producer: queue FULL.\n");
      pthread_cond_wait (fifo->notFull, fifo->mut);
    }
    int epilogi = (int)(rand()%4);
    wf.work = functions[epilogi];
    argu = (int)(rand()%10);
    wf.arg = &argu;
    gettimeofday(&tv, NULL);
    wf.tim=tv.tv_usec;
    queueAdd (fifo, wf);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notEmpty);
  }

  return (NULL);
}

void *consumer (void *q)
{

  queue *fifo;
  int i, d;
  fifo = (queue *)q;

  while(1) {
    pthread_mutex_lock (fifo->mut);
    while (fifo->empty) {
      printf ("consumer: queue EMPTY.\n");
      pthread_cond_wait (fifo->notEmpty, fifo->mut);
  }

    queueDel (fifo, &wf);
    gettimeofday(&tv, NULL);
    wf.tim=tv.tv_usec - wf.tim ;
    printf("h wra einai %ld \n" , wf.tim);
    wf.work(wf.arg);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notFull);

  }

}


queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  return (q);
}

void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);
  free (q->mut);
  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  free (q);
}

void queueAdd (queue *q,workFunction in)
{
  q->buf[q->tail] = in;
  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel (queue *q, workFunction *out)
{
  *out = q->buf[q->head];

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}
