#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>

#include "Command.h"
class RobotControl
{
private:
	Command* cmd;
	thread* trd;
public:
	void SetCommand(Command* command);
	int StartServer();
};

