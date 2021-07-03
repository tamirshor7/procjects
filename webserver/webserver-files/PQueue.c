#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "PQueue.h"
#include "Wrappers.h"
#include <assert.h>



struct PQueue_t {
    pthread_cond_t* cond_not_empty;
    pthread_cond_t* cond_not_full;
    
    int queueMaxSize;
    
    List ls;
};

struct listNode_t {
    int data;
    struct timeval arrival_time;
    struct listNode_t* next;
    struct listNode_t* prev;
};

struct List_t { //cond vars unnecessary - only insert entry point should be from PQueue in a snchronized function
    pthread_mutex_t* lock;
    Node head;
    Node last;
    int* occupancy;
    int list_occupancy; //for active thread num
};

void QInsert(List qlist, int fd, struct timeval cur_time);
void QRemoveAndFree(PQueue q, Node dest);


PQueue Init(int maxSize, int* shared_occupancy)
{//reutrn NULL on any error

    PQueue dest = (PQueue)malloc(sizeof(*dest));
    //dest->lock = malloc(sizeof(pthread_mutex_t));
    dest->ls = InitList(shared_occupancy);
    dest->cond_not_empty = malloc(sizeof(pthread_cond_t));
    dest->cond_not_full = malloc(sizeof(pthread_cond_t));
    if(!(dest->ls->lock && dest->cond_not_empty && dest->cond_not_full))
        return NULL;
    //*shared_occupancy = 0;
    //dest->occupancy = shared_occupancy;
    //dest->q_occupancy = 0;
    //dest->head = dest->ls->head;
    dest->queueMaxSize = maxSize;
    //dest->tail = dest->ls->last;
    //dest->ar = (int*)malloc(INT_SIZE*maxSize);
    
      
    // if(pthread_mutex_init(dest->lock,NULL))
    //     return NULL;
    pthread_cond_init(dest->cond_not_empty,NULL); //should always succeed
    pthread_cond_init(dest->cond_not_full,NULL);
    //printList(dest->ls);
    return dest;
}



void printList(List ls)
{

   Node runner = ls->head;
   while(runner)
    {
        printf("ddddddd %d, ",runner->data);
        runner=  runner->next;
    }
    printf("occ: %d\n", ls->list_occupancy);
}

void removeIdxsFromQueue(PQueue q, int* to_remove)
{//remove idx RELATIVE TO THE 0 IDX OF THE ARRAY OF Q  and close session

    int passed = 0;
    
    Node runner = q->ls->head;
    while(runner)
    {
        
        if(to_remove[passed])
        {
            Node next = runner->next;
            Close(runner->data);
            QRemoveAndFree(q,runner);
            runner = next;
            (*(q->ls->occupancy))--;
            passed++;
            continue;

        }
        passed++;
        runner = runner->next;

    }
}

void enqueue(PQueue queue, int fd, Alg policy, struct timeval cur_time)
{  
    pthread_mutex_lock(queue->ls->lock);
    while((*(queue->ls->occupancy)) == queue->queueMaxSize)
    {
        if(policy == DT)
        {
            Close(fd);
            pthread_mutex_unlock(queue->ls->lock);
            return;
        }

        if(policy == DH)
        {// dequeueToList is locked with same lock, so this execution is synchronized with it
            if(!(queue->ls->list_occupancy)) //according to piazza, if the pending queue is empty, drop the request
            {
                Close(fd);
                pthread_mutex_unlock(queue->ls->lock);
                return;
            }
            Close(queue->ls->head->data);
            QRemoveAndFree(queue,queue->ls->head); //remove oldest from queue
            (*(queue->ls->occupancy))--;
            break;

        }

        if(policy == RANDOM)
        {
            if(!(queue->ls->list_occupancy)) //according to piazza, if the pending queue is empty, drop the request
            {
                Close(fd);
                pthread_mutex_unlock(queue->ls->lock);
                return;
            }
            int num_to_remove = queue->ls->list_occupancy /4;
            num_to_remove += ((!(queue->ls->list_occupancy%4)) ? 0 : 1);
            int* hist  = (int*)(malloc (INT_SIZE*queue->ls->list_occupancy)); // to make things not really wasteful - make a histogram of all indices taken to the removal
            for(int i=0; i<queue->ls->list_occupancy;i++)
                hist[i] = 0;
            int removed = 0;
            while (removed != num_to_remove)
            {
                int candidate = rand()%(queue->ls->list_occupancy);
                if(!hist[candidate])
                {
                    hist[candidate] = 1;
                    removed++;
                }
                    
            }
            
            removeIdxsFromQueue(queue, hist); //handles shared and non-shared list size
            free(hist);
            break; 
        
        }

        //if this point is reached - Apply block policy
        assert(policy == BLOCK);
        pthread_cond_wait(queue->cond_not_full,queue->ls->lock);
    }
    //assert(fd!=0 && fd!=1);
    QInsert(queue->ls, fd, cur_time);
    //printf("aaaaaaaaaaaaaaaaaaaaa %d\n",fd);
    (*(queue->ls->occupancy))++;
    pthread_cond_signal(queue->cond_not_empty);
    //printQueue(queue);
    pthread_mutex_unlock(queue->ls->lock);
}

int dequeueToList(PQueue queue, List target, struct timeval* arrival_time, struct timeval* pickup_time)
{
    
    pthread_mutex_lock(queue->ls->lock);
    while(!(*(queue->ls->occupancy)) || !(queue->ls->list_occupancy))
    {
        pthread_cond_wait(queue->cond_not_empty,queue->ls->lock);
    }
    int fd = queue->ls->head->data;
    struct timeval cur_time = queue->ls->head->arrival_time;
    QRemoveAndFree(queue, queue->ls->head);


    //insert new node to list, from the end
    //printf("aaaaaaaaaaaaaaaaaaaaa %d\n",fd);
    QInsert(target,fd, cur_time);
    if (arrival_time != NULL)
        *arrival_time = cur_time;
    if (pickup_time != NULL)
        gettimeofday(pickup_time, NULL);
    pthread_mutex_unlock(queue->ls->lock);
    return fd;
    
}

pthread_cond_t* getCondNotFull(PQueue q)
{
    return q->cond_not_full;
}

int getThreadsNum(List ls)
{
    return ls->list_occupancy;
}

//List



List InitList(int* shared_occupancy)
{//reutrn NULL on any error

    List dest = (List)malloc(sizeof(*dest));
    dest->lock = malloc(sizeof(pthread_mutex_t));
    //dest->cond_not_full = malloc(sizeof(pthread_cond_t));
    //if(!(dest->lock && dest->cond_not_full))
    //    return NULL;
    *shared_occupancy = 0;
    dest->occupancy = shared_occupancy;
    dest->list_occupancy = 0;
    Node dummy = (Node)malloc(sizeof(*dummy));
    dummy->next = NULL;
    dummy->prev = NULL;
    dummy->data = -1;
    dest->head = dummy;
    dest->last = dummy;

      
    if(pthread_mutex_init(dest->lock,NULL))
        return NULL;
    //pthread_cond_init(dest->cond_not_empty,NULL); //should always succeed
    //pthread_cond_init(dest->cond_not_full,NULL);
    return dest;
}

void QRemoveAndFree(PQueue q, Node dest)
{//DOESN'T CHANGE SHARED SIZE
    assert(dest);
    // printf("\n");
    // printList(q->ls);

    if(dest->prev) //should always enter this if - shouldn't try to delete dummy head
        dest->prev->next = dest->next;
    if(dest->next)
        dest->next->prev = dest->prev;

    if(dest == q->ls->head)
        q->ls->head = (q->ls->last == dest) ? dest->prev : dest->next; // if first node is also last - there's only one node in the list - move head to be dummy head. Otherwise move head to be next node
    if(dest == q->ls->last)
        q->ls->last = dest->prev;
    q->ls->list_occupancy--;
    // printList(q->ls);
    // printf("\n");
    //free(dest);
    
}

void QInsert(List qlist, int fd, struct timeval cur_time) ///////////
{//DOESN'T CHANGE SHARED OCCUPNACY
    //   printf("\n");
    // printList(qlist);
      //  assert(fd!=0 && fd!=1);
    Node dest = (Node)malloc(sizeof(*dest));
    dest->data = fd;
    Node prev = qlist->last;
    prev->next = dest;
    dest->prev = prev;
    dest->next = NULL;
	dest->arrival_time = cur_time;
    qlist->last = dest;
    if(!(qlist->list_occupancy)) //update head if list was empty
        qlist->head = dest;
    qlist->list_occupancy++;
   
    // printList(qlist);
    // printf("\n");
   
    //printList(qlist);
}

void Remove(List ls, int sd, pthread_cond_t* cond_not_full) //remove by sd
{
    // printf("\n");
    // //printList(queue->ls);
    // printList(ls);
    // printf("\n");
  
    pthread_mutex_lock(ls->lock);
    Node runner = ls->head;
    while(runner != NULL)
    {
        if(runner->data == sd)
        {
           
            if(runner->prev) //should always enter this if - shouldn't try to delete dummy head
                runner->prev->next = runner->next;
            if(runner->next)
                runner->next->prev = runner->prev;

            if(runner == ls->head)
                ls->head = (ls->last == runner) ? runner->prev : runner->next; // if first node is also last - there's only one node in the list - move head to be dummy head. Otherwise move head to be next node
            if(runner == ls->last)
                ls->last = runner->prev;
            ls->list_occupancy--;
            (*(ls->occupancy))--;

            //free(runner);
            
            
           
            
            pthread_cond_signal(cond_not_full);
            pthread_mutex_unlock(ls->lock);
            //printQueue(queue);
            return;
        }
        runner = runner->next;
    }
    // for(int i=0;i<100;i++)
    //     printf("cccccccccccccccccccccccc %d",sd);
    
    assert(0);
    pthread_mutex_unlock(ls->lock); //just for safety - should never get here - we assume the input sd is in the list


}