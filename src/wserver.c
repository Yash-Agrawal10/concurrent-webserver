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
    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
	switch (c) {
		case 'd':
			root_dir = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 't':
			num_threads = atoi(optarg);
			break;
		case 'b':
			num_buffers = atoi(optarg);
			break;
		case 's':
			if (strcmp(optarg, "FIFO") != 0 && strcmp(optarg, "SFF") != 0){
				fprintf(stderr, "scheduling policy must either be FIFO or SFF");
				exit(1);
			}
			schedule_alg = optarg;
			break;
		default:
			fprintf(stderr, "./wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]\n");
			exit(1);
	}
	printf("rootdir: %s, port: %d, num_threads: %d, num_buffers: %d, schedule alg: %s\n", 
	root_dir, port, num_threads, num_buffers, schedule_alg);
	
	// Resolve schedule alg error (for now)
	(void) schedule_alg;

    // Change directory to root directory and start listening
    chdir_or_die(root_dir);
    int listen_fd = open_listen_fd_or_die(port);

	// Set up bounded queue
	queue_t* q = malloc(sizeof(queue_t));
	assert(q != NULL);
	if (strcmp(schedule_alg, "FIFO") == 0){
		bounded_queue_init(q, num_buffers, 0);
	}
	else if (strcmp(schedule_alg, "SFF") == 0){
		bounded_queue_init(q, num_buffers, 1);
	}
	else{
		fprintf(stderr, "Internal Error");
		exit(1);
	}

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
		int metadata = 0;
		// Resolve information about path
		// Valid, size (if sorted), ec.
		
		bounded_queue_put(q, conn_fd, metadata);
    }
    return 0;
}


    


 
