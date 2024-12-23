#include "bounded_queue.h"

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdio.h>

void bounded_queue_init(queue_t* q, int max_size, int sorted){
    assert(max_size > 0);

    q->buffer = malloc(sizeof(int) * max_size);
    assert(q->buffer != NULL);
    q->buffer_meta = malloc(sizeof(long long) * max_size);
    assert(q->buffer_meta != NULL);

    q->count = 0;
    q->fill_ptr = 0;
    q->use_ptr = 0;
    q->size = max_size;

    q->sorted = sorted;

    pthread_mutex_init(&q->mtx, NULL);
    pthread_cond_init(&q->emptied, NULL);
    pthread_cond_init(&q->filled, NULL);
}

void bounded_queue_destroy(queue_t* q){
    free(q->buffer);
    free(q->buffer_meta);
    free(q);
    pthread_mutex_destroy(&q->mtx);
    pthread_cond_destroy(&q->emptied);
    pthread_cond_destroy(&q->filled);
}

void bounded_queue_put(queue_t* q, int value, long long metadata){
    pthread_mutex_lock(&q->mtx);
    while (q->count == q->size){
        pthread_cond_wait(&q->emptied, &q->mtx);
    }

    q->buffer[q->fill_ptr] = value;
    q->buffer_meta[q->fill_ptr] = metadata;

    if (q->sorted){
        int curr_index = q->fill_ptr;
        for (int i = 0; i < q->count; i++){
            int test_index = (q->use_ptr + i) & q->size;
            if (q->buffer_meta[test_index] >= q->buffer_meta[curr_index]){
                int temp = q->buffer[test_index];
                q->buffer[test_index] = q->buffer[curr_index];
                q->buffer[curr_index] = temp;
                long long temp_meta = q->buffer_meta[test_index];
                q->buffer_meta[test_index] = q->buffer_meta[curr_index];
                q->buffer_meta[curr_index] = temp_meta;
                break;
            }
        }
    }

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