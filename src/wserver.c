#include "request.h"
#include "io_helper.h"
#include "bounded_queue.h"
#include "worker.h"

#include <stdio.h>
#include <pthread.h>
#include <assert.h>

char default_root[] = ".";

//
// ./wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]
// 
int main(int argc, char *argv[]) {
	// Parse command line inputs
    int c;
    char *root_dir = default_root;	// -d
    int port = 10000;				// -p
	int num_threads = 3;			// -t
	int num_buffers = 5;			// -b
	char* schedule_alg = "FIFO";	// -s
    
    while ((c = getopt(argc, argv, "d:p:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
	    exit(1);
	}

    // Change directory to root directory and start listening
    chdir_or_die(root_dir);
    int listen_fd = open_listen_fd_or_die(port);

	// Set up bounded queue
	queue_t* q = malloc(sizeof(queue_t));
	assert(q != NULL);
	bounded_queue_init(q, num_buffers);

	// Create worker threads
	pthread_t threads[num_threads];
	worker_arg_t worker_arg = {q};
	for (int i = 0; i < num_threads; i++){
		pthread_create(&threads[i], NULL, worker, (void*) &worker_arg);
	}

	// Run producer loop
    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		bounded_queue_put(q, conn_fd);
    }

    return 0;
}


    


 
