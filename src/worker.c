#include "worker.h"
#include "request.h"
#include "io_helper.h"
#include "bounded_queue.h"

void* worker(void* arg){
    // Unpack arguments
    worker_arg_t* args = (worker_arg_t*) arg;
    queue_t* q = args->q;

    // Run worker loop
    while (1){
        int conn_fd = bounded_queue_get(q);
        request_handle(conn_fd);
        close_or_die(conn_fd);
    }

    // Never actually returns
    return NULL;
}