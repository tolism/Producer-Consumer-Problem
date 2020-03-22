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

#include <math.h>

#define QUEUESIZE 10000
#define LOOP 50000
#define nOfProducers 10
#define nOfConsumers 40
#define nOfFunctions 5
#define nOfArguments 16
#define PI 3.14159265

//FILE * fp ;

void * producer(void * args);
void * consumer(void * args);

void * circleArea(void * args);
void * circleCirCumf(void * args);
void * expo(void * args);
void * sinF(void * args);
void * cosF(void * args);

void * ( * functions[nOfFunctions])(void * x) = {
  circleArea,
  circleCirCumf,
  expo,
  sinF,
  cosF
};

int argu[16] = {  0,  5,  10,  15,  20,  25,  30,35,  40,  45,  60,  75,  90,  100,  120,  180};

typedef struct {
  void * ( * work)(void * );
  void * arg;
  time_t waitingTime;
}
workFunction;

//workFunction wf ;
//struct timeval tv;

int prodFinished = 0;
int terminateConFlag = 0;
struct timeval timeStamp;

double tempTime = 0;

typedef struct {
  workFunction buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t * mut;
  pthread_cond_t * notFull, * notEmpty;
}
queue;

queue * queueInit(void);
void queueDelete(queue * q);
void queueAdd(queue * q, workFunction in );
void queueDel(queue * q, workFunction * out);
int main() {

  srand(time(NULL));
  queue * fifo;
  pthread_t pro[nOfProducers], con[nOfConsumers];

  fifo = queueInit();
  if (fifo == NULL) {
    fprintf(stderr, "main: Queue Init failed.\n");
    exit(1);
  }

  for (int i = 0; i < nOfConsumers; i++) {
    pthread_create( & con[i], NULL, consumer, fifo);
  }

  for (int i = 0; i < nOfProducers; i++) {
    pthread_create( & pro[i], NULL, producer, fifo);
  }

  for (int i = 0; i < nOfProducers; i++) {
    pthread_join(pro[i], NULL);
  }

  for (int i = 0; i < nOfConsumers; i++) {
    pthread_join(con[i], NULL);
  }

  double averageTime = ((double) tempTime) / (nOfProducers * LOOP);
  printf("The average waiting time  is :  %lf \n", averageTime);

  queueDelete(fifo);

  return 0;
}

void * producer(void * q) {
  queue * fifo;
  int i;
  fifo = (queue * ) q;
  int randF, randAr;

  for (i = 0; i < LOOP; i++) {
    pthread_mutex_lock(fifo -> mut);

    while (fifo -> full) {
      //  printf ("producer: queue FULL.\n");
      pthread_cond_wait(fifo -> notFull, fifo -> mut);
    }
    workFunction wf;
    randF = rand() % nOfFunctions;
    wf.work = functions[randF];
    randAr = rand() % nOfArguments;
    wf.arg = & argu[randAr];
    gettimeofday( & timeStamp, NULL);
    wf.waitingTime = timeStamp.tv_usec;
    //wf.tim=clock();
    queueAdd(fifo, wf);
    pthread_mutex_unlock(fifo -> mut);
    pthread_cond_signal(fifo -> notEmpty);
  }

  prodFinished++;
  if (prodFinished == nOfProducers) {

    terminateConFlag = 1;


    pthread_cond_broadcast(fifo -> notEmpty);
  }

  return (NULL);
}

void * consumer(void * q) {

  queue * fifo;
  int i, d;
  fifo = (queue * ) q;

  while (1) {

    pthread_mutex_lock(fifo -> mut);

    while (fifo -> empty == 1 && terminateConFlag != 1) {


      pthread_cond_wait(fifo -> notEmpty, fifo -> mut);
    }

    if (terminateConFlag == 1 && fifo -> empty == 1) {
      pthread_mutex_unlock(fifo -> mut);
      pthread_cond_broadcast(fifo -> notEmpty);
      break;
    }
    workFunction wf;
    queueDel(fifo, & wf);
    gettimeofday( & timeStamp, NULL);
    //wf.waitingTime=timeStamp.tv_usec- wf.waitingTime;
    // wf.waitingTime = ((double) (clock() - wf.waitingTime) / CLOCKS_PER_SEC)*1000;
    printf("Eksodos : %ld , Eisodos : %ld     , Waiting time : %ld \n", timeStamp.tv_usec, wf.waitingTime, timeStamp.tv_usec - wf.waitingTime);
    wf.waitingTime = timeStamp.tv_usec - wf.waitingTime;
    tempTime += wf.waitingTime;


    wf.work(wf.arg);
    pthread_mutex_unlock(fifo -> mut);
    pthread_cond_signal(fifo -> notFull);
  }

  return (NULL);

}

queue * queueInit(void) {
  queue * q;

  q = (queue * ) malloc(sizeof(queue));
  if (q == NULL) return (NULL);

  q -> empty = 1;
  q -> full = 0;
  q -> head = 0;
  q -> tail = 0;
  q -> mut = (pthread_mutex_t * ) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(q -> mut, NULL);
  q -> notFull = (pthread_cond_t * ) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q -> notFull, NULL);
  q -> notEmpty = (pthread_cond_t * ) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q -> notEmpty, NULL);

  return (q);
}

void queueDelete(queue * q) {
  pthread_mutex_destroy(q -> mut);
  free(q -> mut);
  pthread_cond_destroy(q -> notFull);
  free(q -> notFull);
  pthread_cond_destroy(q -> notEmpty);
  free(q -> notEmpty);
  free(q);
}

void queueAdd(queue * q, workFunction in ) {
  q -> buf[q -> tail] = in ;
  q -> tail++;
  if (q -> tail == QUEUESIZE)
    q -> tail = 0;
  if (q -> tail == q -> head)
    q -> full = 1;
  q -> empty = 0;

  return;
}

void queueDel(queue * q, workFunction * out) {
  * out = q -> buf[q -> head];

  q -> head++;
  if (q -> head == QUEUESIZE)
    q -> head = 0;
  if (q -> head == q -> tail)
    q -> empty = 1;
  q -> full = 0;

  return;
}

void * circleArea(void * args) {
  double x = ( * (int * ) args);
  double circleAr = PI * x * x;
  //  printf("\nArea of circle is: %lf \n",circleAr );

}

void * circleCirCumf(void * args) {
  double x = ( * (int * ) args);
  double circleC = 2 * PI * x;
  //     printf("\nCircumference of circle is: %lf \n",circleC);
}

void * expo(void * args) {
  double x = ( * (int * ) args);
  double result = exp(x / 180);
  //    printf("Exponential of %lf = %lf \n", x, result);

}

void * sinF(void * args) {
  double x = * (int * ) args;
  double ret, val;
  val = PI / 180;
  ret = sin(x * val);
  //   printf("The sine of %lf is %lf degrees \n", x, ret);

}

void * cosF(void * args) {
  double x = * (int * ) args;
  double ret, val;
  val = PI / 180;
  ret = cos(x * val);
  //   printf("The cosine of %lf is %lf degrees \n", x, ret);

}
