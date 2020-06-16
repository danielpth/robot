#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "Command.h"
class RobotControl
{
private:
	Command* cmd;
public:
	void SetCommand(Command* command);
	int StartServer();
};

