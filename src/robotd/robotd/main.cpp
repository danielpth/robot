#include <cstdio>
#include <list>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

/*
Install init script:
/etc/init.d/robot.d

#! /bin/bash

### BEGIN INIT INFO
# Provides:          robotd
# Required-Start:    $local_fs $network
# Required-Stop:     $local_fs
# Default-Start:     3
# Default-Stop:      0 1 3 6
# Short-Description: robotd service
# Description:       Run robotd service
### END INIT INFO

# Carry out specific functions when asked to by the system
case "$1" in
  start)
	echo "Starting robotd..."
	#sudo -u foo-user bash -c 'cd /path/to/scripts/ && ./start-foo.sh'
	/home/pi/robotd -r -d
	;;
  stop)
	echo "Stopping robotd..."
	#sudo -u foo-user bash -c 'cd /path/to/scripts/ && ./stop-foo.sh'
	killall robotd
	sleep 2
	;;
  *)
	echo "Usage: /etc/init.d/robotd {start|stop}"
	exit 1
	;;
esac

exit 0

$ sudo systemctl enable robotd
$ sudo systemctl unmask robotd

*/
using namespace std;

struct s_process_data {
	pid_t pid;
	string name;
	string path;
};

extern char* optarg;

list<struct s_process_data> pids;
bool run = true;

list<struct s_process_data>::iterator FindProcess(pid_t pid)
{
	list<struct s_process_data>::iterator first = pids.begin();
	list<struct s_process_data>::iterator last = pids.end();
	while (first != last) {
		if (first->pid == pid) return first;
		++first;
	}
	return last;
}

void SendSignalAllChilds(int sig)
{
	list<struct s_process_data>::iterator first = pids.begin();
	list<struct s_process_data>::iterator last = pids.end();
	while (first != last) {
		printf("Sending signal [%d] to pid [%d]\n", sig, first->pid);
		kill(first->pid, sig);
		++first;
	}
}

int StartProcess(string name, string path)
{
	pid_t p;
	errno = 0;

	p = fork();
	if (p == 0)
	{
		// this is the child
		//On success, exec() does not return, on error -1 is returned,
		//and errno is set to indicate the error.
		int rc = execlp(path.c_str(), name.c_str(), "-r", NULL);
		printf("execlp error: %d errno: %d (%s)\n", rc, errno, strerror(errno));

		list<struct s_process_data>::iterator it;
		it = FindProcess(getpid());
		pids.erase(it);
	}
	else if (p > 0)
	{
		// this is the parent
		struct s_process_data pd;
		pd.pid = p;
		pd.name = name;
		pd.path = path;
		pids.push_back(pd);
		printf("Fork successful. Child pid: %d\n", p);
	}
	else
	{
		// error
		printf("Fork error (%d) (%d) (%s)\n", p, errno, strerror(errno));
	}

	return 0;
}


int Monitor()
{
	int wstatus;
	pid_t p;
	list<struct s_process_data>::iterator it;

	while (run) {
		errno = 0;
		wstatus = 0;
		p = wait(&wstatus);

		printf("Wait pid: [%d] status: [%d]\n", p, wstatus);

		if (p > 0)
		{
			if (WIFEXITED(wstatus)) {
				printf("Child exit status: [%d]\n", WEXITSTATUS(wstatus));
			}

			if (WIFSIGNALED(wstatus)) {
				printf("Child signal received: [%d] [%s]\n", WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
				if (WCOREDUMP(wstatus)) {
					printf("Child produced a core dump\n");
				}
				switch (WTERMSIG(wstatus))
				{
				case SIGABRT:
					continue;
					break;
					/*SIGABRT
					SIGALRM
					SIGBUS
					SIGCHLD
					SIGCLD
					SIGCONT
					SIGEMT
					SIGFPE
					SIGHUP
					SIGILL
					SIGINFO
					SIGINT
					SIGIO
					SIGIOT
					SIGKILL
					SIGLOST
					SIGPIPE
					SIGPOLL
					SIGPROF
					SIGPWR
					SIGQUIT
					SIGSEGV
					SIGSTKFLT
					SIGSTOP
					SIGTSTP
					SIGSYS
					SIGTERM
					SIGTRAP
					SIGTTIN
					SIGTTOU
					SIGUNUSED
					SIGURG
					SIGUSR1
					SIGUSR2
					SIGVTALRM
					SIGXCPU
					SIGXFSZ
					SIGWINCH*/
				}
			}

			if (WIFSTOPPED(wstatus)) {
				printf("child process was stopped by delivery of a signal: [%d]\n", WSTOPSIG(wstatus));
				continue;
			}

			if (WIFCONTINUED(wstatus)) {
				printf("Child process was resumed by delivery of SIGCONT\n");
				continue;
			}

			it = FindProcess(p);

			if (it != pids.end())
			{
				printf("Child pid [%d] name [%s] status [%d] died! Ressurecting!\n", it->pid, it->name.c_str(), wstatus);
				StartProcess(it->name, it->path);

				pids.erase(it);

				//printf("pid list size [%d]\n", pids.size());
			}
			else
			{
				printf("Can't find PID [%d]\n", p);
			}
		}
		else
		{
			printf("Wait failure [%d] [%s]\n", errno, strerror(errno));
			sleep(5);
		}

	}

	return 0;
}

static void handler(int sig, siginfo_t* si, void* unused)
{
	/* Note: calling printf() from a signal handler is not safe
	   (and should not be done in production programs), since
	   printf() is not async-signal-safe; see signal-safety(7).
	   Nevertheless, we use printf() here as a simple way of
	   showing that the handler was called. */

	printf("Got [%s] [%d]  at address: %p\n", strsignal(sig), sig, si->si_addr);

	switch (sig) {
	default:
		SendSignalAllChilds(sig);
		exit(EXIT_SUCCESS);
		break;
	}
}

int main(int argc, char* argv[])
{
	int opt, rc;
	bool daemonize = false, run = false;
	struct sigaction sa, sa_old;

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

		StartProcess("robotLowLevel", "/home/pi/robotLowLevel");

		sa.sa_flags = SA_SIGINFO;
		sigemptyset(&sa.sa_mask);
		sa.sa_sigaction = handler;
		rc = sigaction(SIGTERM, &sa, &sa_old);
		if (rc != 0) {
			printf("Error sigaction SIGTERM [%d]", rc);
		}
		rc = sigaction(SIGUSR1, &sa, &sa_old);
		if (rc != 0) {
			printf("Error sigaction SIGUSR1 [%d]", rc);
		}
		rc = sigaction(SIGUSR2, &sa, &sa_old);
		if (rc != 0) {
			printf("Error sigaction SIGUSR1 [%d]", rc);
		}
		rc = sigaction(SIGQUIT, &sa, &sa_old);
		if (rc != 0) {
			printf("Error sigaction SIGQUIT [%d]", rc);
		}
		/*
		rc = sigaction(SIGKILL, &sa, &sa_old);
		if (rc != 0) {
			printf("Error sigaction SIGKILL [%d]", rc);
		}
		*/
		Monitor();

	}

	return 0;
}