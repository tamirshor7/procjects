#include "Wrappers.h"
#include "PQueue.h"
#include "request.h"

#define WHITESPACE ' '


PQueue pending;
List handled;




void getargs(int *port, int* threads, int* queue_size, char** sched_alg, int argc, char *argv[])
{
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <port> <threads> <queue_size> <sched_alg>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    *sched_alg = argv[4];
    if((*threads) <= 0 || (*queue_size)<= 0)
    {
        fprintf(stderr, "Invalid arg\n");
        exit(1);
    }

}

void* threadWrapper (void* arg)
{//actual request handling by each thread
    struct timeval arrival_time, pickup_time;

    
    ThreadStatistics statistics = {.th_id = *((int*)arg), .req_count = 0, .static_req = 0, .dynamic_req = 0};
    free(arg);
    while(1)
    {
        int sd = dequeueToList(pending,handled, &arrival_time, &pickup_time);
        statistics.req_count++;
        struct timeval dispatch;
        timersub(&pickup_time, &arrival_time, &dispatch);
        requestHandle(sd, &statistics, &arrival_time, &dispatch);

        Remove(handled,sd,getCondNotFull(pending));

        Close(sd);
    }
}

Alg validateAlg(char* str_arg, char* str_expected)
{ //make sure inserted scheduling algorithm is valid
    int i=0;
    while(str_expected[i] != '\0')
    {
        if(str_arg[i] != str_expected[i])
            return ALG_ERROR;
        i++;
    }
    Alg ans = str_expected[0] == 'b' ? BLOCK : RANDOM;
    return str_expected[i] == '\0' ? ans : ALG_ERROR;
}

Alg parseDrop(char* str)
{
    int i = 0;
    while(str[i] != '\0')
        i++;
    if(i != 2)
        return ALG_ERROR;
    switch (str[1])
    {
        case 'h' : return DH; break;
        case 't':  return DT; break;
        default: return ALG_ERROR;
    }
}


Alg getSchedAlg(char* sched_str)
{
    switch (sched_str[0])
    {
        case 'b': return validateAlg(sched_str, "block"); break;
        case 'r': return validateAlg(sched_str, "random"); break;
        case 'd': return parseDrop(sched_str); break;
        default: return ALG_ERROR;
    }
}
int main(int argc, char *argv[])
{
    
    int listenfd, connfd, port, clientlen, threads, queue_size;
    char* sched_alg;
    struct sockaddr_in clientaddr;

    getargs(&port, &threads, &queue_size, &sched_alg, argc, argv);

    Alg sched  = getSchedAlg(sched_alg);
    if(sched == ALG_ERROR) //arbitrary default
        sched = BLOCK;
    //initialize request queues
    int* shared_occupancy = (int*) malloc(INT_SIZE);
    pending = Init(queue_size, shared_occupancy);
    handled = InitList(shared_occupancy);

    // Create some threads...
    pthread_t* worker_threads = (pthread_t*) malloc(sizeof(pthread_t) * threads);

    for(int i=0; i < threads; i++) {
        int* thread_id = (int*) malloc(sizeof(int));
        *thread_id = i;
        pthread_create(worker_threads + i, NULL, threadWrapper, (void*)thread_id);
    }

    listenfd = Open_listenfd(port);

    struct timeval cur_time;

    while (1) { //infinite loop to accept requests - main thread only accepts them - no handling.
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        gettimeofday(&cur_time, NULL);
        enqueue(pending,connfd, sched, cur_time);

      
    }
    for(int i=0; i < threads; i++)
        pthread_join(worker_threads[i], NULL);

}


    


 
