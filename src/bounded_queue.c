#include "bounded_queue.h"

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

void bounded_queue_init(queue_t* q, int max_size){
    assert(max_size > 0);
    q->buffer = malloc(sizeof(int) * max_size);
    assert(q->buffer != NULL);
    q->count = 0;
    q->fill_ptr = 0;
    q->use_ptr = 0;
    q->size = 0;
    pthread_mutex_init(&q->mtx, NULL);
    pthread_cond_init(&q->emptied, NULL);
    pthread_cond_init(&q->filled, NULL);
}

void bounded_queue_destroy(queue_t* q){
    free(q->buffer);
    free(q);
    pthread_mutex_destroy(&q->mtx);
    pthread_cond_destroy(&q->emptied);
    pthread_cond_destroy(&q->filled);
}

void bounded_queue_put(queue_t* q, int value){
    pthread_mutex_lock(&q->mtx);
    while (q->count == q->size){
        pthread_cond_wait(&q->emptied, &q->mtx);
    }
    q->buffer[q->fill_ptr] = value;
    q->fill_ptr = (q->fill_ptr + 1) % q->size;
    q->count++;
    pthread_cond_signal(&q->filled);
    pthread_mutex_unlock(&q->mtx);
}

int bounded_queue_get(queue_t* q){
    pthread_mutex_lock(&q->mtx);
    while (q->count == 0){
        pthread_cond_wait(&q->filled, &q->mtx);
    }
    int value = q->buffer[q->use_ptr];
    q->use_ptr = (q->use_ptr + 1) % q->size;
    q->count--;
    pthread_cond_signal(&q->emptied);
    pthread_mutex_unlock(&q->mtx);
    return value;
}
