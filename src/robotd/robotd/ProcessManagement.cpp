#include "ProcessManagement.h"

int ProcessManagement::StartProcess(string name, string path)
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

int ProcessManagement::StopProcess()
{
	return -1;
}

int ProcessManagement::StopAllProcess()
{

	return -1;
}

list<struct s_process_data>::iterator ProcessManagement::FindProcess(pid_t pid)
{
	list<struct s_process_data>::iterator first = pids.begin();
	list<struct s_process_data>::iterator last = pids.end();
	while (first != last) {
		if (first->pid == pid) return first;
		++first;
	}
	return last;
}

int ProcessManagement::Monitor()
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
		}

	}

	return 0;
}
