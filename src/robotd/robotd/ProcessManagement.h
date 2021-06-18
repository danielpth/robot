#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <thread>
#include <list>
#include <algorithm>   

using namespace std;

struct s_process_data {
	pid_t pid;
	string name;
	string path;
};

class ProcessManagement
{
private:
	bool run = true;
	list<struct s_process_data> pids;
	list<struct s_process_data>::iterator FindProcess(pid_t pid);

public:
	int StartProcess(string name, string path);
	int StopProcess();
	int StopAllProcess();
	int Monitor();
};

