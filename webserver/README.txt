This project implements a basic TCP/IP LINUX BASED webserver. 
Main focus is using parallel programming to implement a webserver that is able to fulfill client requests for static and dynamic contents.
Request overflow behaviour and thread number are determined by user running the server, as well as which virtual port should be used.

Project File Layout:
server.c - Implementing the server. 

main thread is due to only accept requests and insert them to the inner data structures (specified later on).
Requests are only handled by a pre-defined number of worker threads.

Synchronization is done with pthreads' library mutex locks and condition variables.

The server uses the following data structure:

*   Server listens to user defined port. Main thread puts all requests received in said port in a pending requests queue of a user-defined 
    maximum size. If the queue is full, the user-defined scheduling algorithm is applied.

*   At all times, a user-defined sized thread pool removes pending requests from the queue to a list of currently handled requests.
    If the queue is empty, the thread forfits the processor and waits for it to become full.
    All insertions and removes from list and queue are synchronized by pthreads' condition variables.

*   After a thread has fulfilled a request, it return to pull new requests form the pending queue.

Usage: ./server <port> <threads> <queue_size> <sched_alg>
        port    -        Which virtual port should be listened to
        threads -  How many active worker threads should there be for handle client requests (idle threads will wait for new unfulfilled requests).
        queue_size - Maximum number of requests to be kept in pending status (waiting for a worker thread to become available).
        sched_alg - Policy in case a request arrives and pending requests queue is full. 
        Should only be one out of the following:
                                            block  - Main thread blocks (forfits processor) until pending queue becomes available.
                                            dt     - Main thread declines any new requests until pending queue becomes available.
                                            dh     - Oldest request in pending queue that is not currently being handled by a worker thread is 
                                                     dropped, and new request is accepted instead of it.
                                            random - randomly drop 25% (round up) of currently pending requests, and accept new request.

request.c  - Inner handling of client request, done by a handling worker thread. NOT IMLEMENTED BY ME.

client.c   - Implements a client capable of making server requests.

Wrappers.c - Various wrappers for syscalls - NOT IMPLEMENTED BY ME (But included for conveniency).

output.c   - Some code used to "waste time" on the server - mostly for more convenient testing.

PQueue.h   - Data structures' API. Documented on header file. PQueue.c Implements the structures as well as 
             all of the Synchronization mechanisms. Playing with it might cause nasty deadlocks.
