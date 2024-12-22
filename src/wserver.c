#include <stdio.h>
#include "request.h"
#include "io_helper.h"

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

    // change directory to root directory
    chdir_or_die(root_dir);

    // run server loop
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		request_handle(conn_fd);
		close_or_die(conn_fd);
    }
    return 0;
}


    


 
