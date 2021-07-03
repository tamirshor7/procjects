#ifndef __REQUEST_H__

typedef struct threadStatistics_t {
    int th_id;
    int req_count;
    int static_req;
    int dynamic_req;
} ThreadStatistics;

void requestHandle(int fd, ThreadStatistics * thread_info, struct timeval* arrival_time, struct timeval* dispatch);


#endif
