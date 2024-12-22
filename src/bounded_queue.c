#include "bounded_queue.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void queue_init(queue_t* q, int size){
    node_t* dummy_node = malloc(sizeof(node_t));
    assert(dummy_node != NULL);
    dummy_node->next = NULL;
    dummy_node->value = 0;
    q->head = dummy_node;
    q->tail = dummy_node;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
    pthread_cond_init(&q->filled, NULL);
    pthread_cond_init(&q->emptied, NULL);
    q->max_size = size;
    atomic_store(&q->size, 0);
}

void queue_destroy(queue_t* q){
    pthread_mutex_lock(&q->head_lock);
    pthread_mutex_lock(&q->tail_lock);
    while (q->head != NULL){
        node_t* to_free = q->head;
        q->head = q->head->next;
        free(to_free);
    }
    q->tail = NULL;
    pthread_mutex_unlock(&q->head_lock);
    pthread_mutex_unlock(&q->tail_lock);
    pthread_mutex_destroy(&q->head_lock);
    pthread_mutex_destroy(&q->tail_lock);
    pthread_cond_destroy(&q->filled);
    pthread_cond_destroy(&q->emptied);
    free(q);
}

void enqueue(queue_t* q, int value){
    node_t* new_tail = malloc(sizeof(node_t));
    assert(new_tail != NULL);
    new_tail->next = NULL;
    new_tail->value = value;

    pthread_mutex_lock(&q->tail_lock);
    while (atomic_load(&q->size) == q->max_size){
        pthread_cond_wait(&q->emptied, &q->tail_lock);
    }
    q->tail->next = new_tail;
    q->tail = new_tail;
    atomic_fetch_add(&q->size, 1);
    pthread_cond_signal(&q->filled);
    pthread_mutex_unlock(&q->tail_lock);
}

int dequeue(queue_t* q){
    pthread_mutex_lock(&q->head_lock);
    while (atomic_load(&q->size) == 0){
        pthread_cond_wait(&q->filled, &q->head_lock);
    }
    node_t* new_head = q->head->next;
    free(q->head);
    q->head = new_head;
    int value = q->head->value;
    atomic_fetch_sub(&q->size, 1);
    pthread_cond_signal(&q->emptied);
    pthread_mutex_unlock(&q->head_lock);
    return value;
}

void* enqueuer(void* arg){
    enqueuer_arg_t* args = (enqueuer_arg_t*) arg;
    queue_t* q = args->q;
    int count = args->count;
    for (int i = 0; i < count; i++){
        enqueue(q, i);
    }
    return NULL;
}

void* dequeuer(void* arg){
    dequeuer_arg_t* args = (dequeuer_arg_t*) arg;
    queue_t* q = args->q;
    int count = args->count;
    int* nums = args->nums;
    for (int i = 0; i < count; i++){
        int value = dequeue(q);
        nums[i] = value;
    }
    return (void*) nums;
}