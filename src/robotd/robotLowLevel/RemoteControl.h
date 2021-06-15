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
	thread* threadBatteryMonitor;
	thread* threadServerRemoteControl;
	thread* trd;
	bool runBatteryMonitor;
	void batteryMonitor();
	bool remoteControl;
	int bindSockfd;
	list<int> clientSocketfd;
	
	int RemoteControlTread();

public:
	RemoteControl(Command* command);
	~RemoteControl();

	int StartServer();
	int StopServer();
	int HandleClient(int clientSocket);
	void Speak(string sentence);
};

