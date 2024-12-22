#ifndef WORKER_H
#define WORKER_H

#include "bounded_queue.h"

typedef struct worker_arg_t {
    queue_t* q;
} worker_arg_t;

// Worker function for threads
// Blocks until HTTP request is available in queue
// Completes HTTP request
// Loops this forever
void* worker(void* arg);

#endif