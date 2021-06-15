#include <cstdio>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include "SerialPort.h"
#include "Protocol.h"
#include "Command.h"
#include "RemoteControl.h"

using namespace std;
using namespace LibSerial;

extern char* optarg;

int main(int argc, char* argv[])
{
	//string serial_port_path = "/dev/ttyUSB0";
	//string serial_port_path = "/dev/serial0";
	string serial_port_path = "/dev/ttyS0";
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
			serial_port_path = optarg;
			break;

		default: /* '?' */
			fprintf(stderr, "Usage: %s [options]\n", argv[0]);
			exit(EXIT_FAILURE);
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
		SerialPort* sp = new SerialPort(serial_port_path, LibSerial::BaudRate::BAUD_115200,
			LibSerial::CharacterSize::CHAR_SIZE_8,
			LibSerial::FlowControl::FLOW_CONTROL_NONE,
			LibSerial::Parity::PARITY_NONE,
			LibSerial::StopBits::STOP_BITS_1);
		Protocol* pt = new Protocol(sp);
		Command* cmd = new Command(pt);
		RemoteControl* ctrl = new RemoteControl(cmd);

		ctrl->StartServer();
		//sleep(10);
		//ctrl->StopServer();
		// Wait until some signal is received

		// TODO: finish it
		sigemptyset(&sig_set);
		sigaddset(&sig_set, SIGQUIT);
		sigaddset(&sig_set, SIGUSR1);
		//s = pthread_sigmask(SIG_BLOCK, &set, NULL);
		sigwait(&sig_set, &sig);

		delete ctrl;
		delete cmd;
		delete pt;
		delete sp;
	}

	return 0;
}