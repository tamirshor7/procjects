#ifndef QUEUE
#define QUEUE

/* Implementation of inner Queue based data structures to maintain request pool out of which the worker threads can take and fulfill client requests */

static int INT_SIZE = 4;

typedef enum{BLOCK, DT, DH, RANDOM, ALG_ERROR} Alg;

typedef struct PQueue_t* PQueue;
typedef struct List_t* List;
typedef struct listNode_t* Node;

PQueue Init(int maxSize, int* shared_occupancy); //initialize pending queue
void enqueue(PQueue queue, int fd, Alg policy, struct timeval cur_time); //insert request to pending queue
int dequeueToList(PQueue queue, List ls, struct timeval* arrival_time, struct timeval* pickup_time); //move request from pending queue to handled list
pthread_cond_t* getCondNotFull(PQueue q);
int getReqs(PQueue pending);
List InitList(int* shared_occupancy); //initialize handled requests list
void Remove(List ls, int sd, pthread_cond_t* cond_not_full); // remove request from data structure
int getThreadsNum(List ls);
void printList(List ls);

#endif