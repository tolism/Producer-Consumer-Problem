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

// Defines for the queue and the prod/cons
#define QUEUESIZE 5000
#define LOOP 30000
#define nOfProducers 4
#define nOfConsumers 1
#define nOfFunctions 5
#define nOfArguments 16
#define PI 3.14159265



// Thread functions decleration
void * producer(void * args);
void * consumer(void * args);

// Functions decleration
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
}
workFunction;

struct timeval arrTime[QUEUESIZE];

// Global flags/counters/times
int prodFinished = 0;
int terminateConFlag = 0;
struct timeval timeStamp;
double tempTime = 0;

// The queue
typedef struct {
  workFunction buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t * mut;
  pthread_cond_t * notFull, * notEmpty;
}
queue;
// Queue functions
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
//Creating up the consumers / producers threads
  for (int i = 0; i < nOfConsumers; i++) {
    pthread_create( & con[i], NULL, consumer, fifo);
  }

  for (int i = 0; i < nOfProducers; i++) {
    pthread_create( & pro[i], NULL, producer, fifo);
  }

//Waiting for the threads to join
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

//Producer thread function
void * producer(void * q) {
  queue * fifo;
  int i;
  fifo = (queue * ) q;
  int randF, randAr;

  for (i = 0; i < LOOP; i++) {
    pthread_mutex_lock(fifo -> mut);

    while (fifo -> full) {
      pthread_cond_wait(fifo -> notFull, fifo -> mut);
    }
    //Workfunction object to add at the queue
    workFunction wf;
    //Randomly choose a function
    randF = rand() % nOfFunctions;
    wf.work = functions[randF];
    //Randomly choose an argument
    randAr = rand() % nOfArguments;
    wf.arg = & argu[randAr];
    //Getting the arrival time at the queue
    gettimeofday(&(arrTime[(fifo->tail)]),NULL);
    queueAdd(fifo, wf);
    pthread_mutex_unlock(fifo -> mut);
    pthread_cond_signal(fifo -> notEmpty);
  }

  prodFinished++;
  //Termination condition for the consumers
  if (prodFinished == nOfProducers) {

    terminateConFlag = 1;
    //Broadcast for the locked consumers
    pthread_cond_broadcast(fifo -> notEmpty);
  }

  return (NULL);
}

// Consumer thread function
void * consumer(void * q) {

  queue * fifo;
  int i, d;
  fifo = (queue * ) q;
  double waitingTime ;

  while (1) {


    pthread_mutex_lock(fifo -> mut);

    while (fifo -> empty == 1 && terminateConFlag != 1) {


      pthread_cond_wait(fifo -> notEmpty, fifo -> mut);
    }
    //Termination flag for the consumers
    if (terminateConFlag == 1 && fifo -> empty == 1) {
      pthread_mutex_unlock(fifo -> mut);
      pthread_cond_broadcast(fifo -> notEmpty);
      break;
    }

    //Workfunction object to remove from the queue
    workFunction wf ;
    //Getting the leave time from the queuee
    struct timeval leaveTime ;
    gettimeofday(&leaveTime,NULL);
    //Calculating the waiting time at the queue
    waitingTime= (leaveTime.tv_sec -(arrTime[fifo->head]).tv_sec) *1e6 + (leaveTime.tv_usec-(arrTime[fifo->head]).tv_usec) ;
    //printf("The waiting time is : %lf  \n " , waitingTime );
    queueDel(fifo, &wf);
    tempTime += waitingTime;


    pthread_mutex_unlock(fifo -> mut);
    pthread_cond_signal(fifo -> notFull);
    //Executing the work function outside the mutexes
    wf.work(wf.arg);

  }
  return (NULL);
}

//Queue function implementations
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

// Work function implementations
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
