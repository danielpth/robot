#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <string.h>
#include <linux/reboot.h>
#include <errno.h>
#include <thread>
#include <list>

#include "Command.h"
#include "Speak.h"

class BatteryMonitor
{
private:
	// Properties
	const unsigned int interval_ms = 1000;
	Command* cmd;
	thread* threadBatteryMonitor;
	bool isThreadRunning;
	double filterAcc;
	double voltage;
	double percent;
	// Method
	void funcBatteryMonitor();
	double ConvertVoltagePercent(double voltage, bool isCharging);
	double FilterVoltage(double voltage);
	void FastProbe();
	void ProbeFilter();

public:
	BatteryMonitor(Command* command);
	~BatteryMonitor();

	// Callback
	void (*EventBattery)(double percent);
};

