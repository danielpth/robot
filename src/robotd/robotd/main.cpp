#include <cstdio>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

using namespace std;

extern char* optarg;

int main(int argc, char* argv[])
{
	//string serial_port_path = "/dev/ttyUSB0";
	//string serial_port_path = "/dev/serial0";
	//string serial_port_path = "/dev/ttyS0";
	int opt, rc;
	bool daemonize = false, run = false;
	sigset_t sig_set;
	int sig;

	while ((opt = getopt(argc, argv, "hrdp:")) != -1) {
		switch (opt) {

		case 'h':
			printf("Help not ready yet\n");
			break;

		case 'r':
			run = true;
			break;

		case 'd':
			daemonize = true;
			break;

		case 'p':
			//serial_port_path = optarg;
			break;

		default: /* '?' */
			fprintf(stderr, "Usage: %s [options]\n", argv[0]);
			exit(0);
		}
	}

	setenv("DISPLAY", ":0.0", 1);

	if (run) {
		if (daemonize) {
			/*
			If nochdir is zero, daemon() changes the process's current working
			directory to the root directory ("/"); otherwise, the current working
			directory is left unchanged.

			If noclose is zero, daemon() redirects standard input, standard
			output and standard error to /dev/null; otherwise, no changes are
			made to these file descriptors.
			*/
			rc = daemon(1, 0);
			if (rc != 0) {
				printf("daemonize fail: %d\n", errno);
			}
		}

		// TODO: finish it
		sigemptyset(&sig_set);
		sigaddset(&sig_set, SIGQUIT);
		sigaddset(&sig_set, SIGUSR1);
		//s = pthread_sigmask(SIG_BLOCK, &set, NULL);
		sigwait(&sig_set, &sig);

	}

	return 0;
}