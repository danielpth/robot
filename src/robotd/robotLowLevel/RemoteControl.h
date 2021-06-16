#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <linux/reboot.h>
#include <errno.h>
#include <thread>
#include <list>

#include "Command.h"
class RemoteControl
{
private:
	Command* cmd;
	thread* threadServerRemoteControl;
	thread* trd;
	bool remoteControl;
	int bindSockfd;
	list<int> clientSocketfd;
	
	int RemoteControlTread();
	int HandleClient(int clientSocket);

public:
	RemoteControl(Command* command);
	~RemoteControl();

	int StartServer();
	int StopServer();
	void Speak(string sentence);
};

