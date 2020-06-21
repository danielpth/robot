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

#include "Command.h"
class RemoteControl
{
private:
	Command* cmd;
	thread* threadBatteryMonitor;
	thread* trd;
	bool runBatteryMonitor;
	void batteryMonitor();

public:
	RemoteControl(Command* command);
	~RemoteControl();

	int StartServer();
	void Speak(string sentence);
};

