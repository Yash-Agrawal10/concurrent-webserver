#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

#include <pthread.h>

typedef struct queue_t {
    int* buffer;

    long long* buffer_meta;
    int sorted;

    int fill_ptr;
    int use_ptr;
    int count;
    int size;

    pthread_mutex_t mtx;
    pthread_cond_t emptied;
    pthread_cond_t filled;
} queue_t;

// Initialize concurrent bounded queue with given maximum size
void bounded_queue_init(queue_t* q, int max_size, int sorted);

// Destroy bounded queue
void bounded_queue_destroy(queue_t* q);

// Put value into bounded queue concurrently
// Blocks until queue is not full
void bounded_queue_put(queue_t* q, int value);

// Pull and return value from queue concurrently
// Blocks until queue is not empty
int bounded_queue_get(queue_t* q);

long long get_metadata(int fd);

#endif