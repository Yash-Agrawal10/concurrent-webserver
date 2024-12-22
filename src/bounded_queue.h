#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdatomic.h>

typedef struct node_t {
    int value;
    struct node_t* next;
} node_t;

typedef struct queue_t {
    node_t* head;
    node_t* tail;
    pthread_mutex_t head_lock;
    pthread_mutex_t tail_lock;
    pthread_cond_t filled;
    pthread_cond_t emptied;
    int max_size;
    atomic_int size;
} queue_t;

typedef struct enqueuer_arg_t {
    queue_t* q;
    int count;
} enqueuer_arg_t;

typedef struct dequeuer_arg_t {
    queue_t* q;
    int count;
    int* nums;
} dequeuer_arg_t;

void queue_init(queue_t* q, int size);
void queue_destroy(queue_t* q);
void enqueue(queue_t* q, int value);
int dequeue(queue_t* q);
void* enqueuer(void* arg);
void* dequeuer(void* arg);

#endif