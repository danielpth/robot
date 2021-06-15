#pragma once
#include <mutex>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../../ctrl_wheel/Core/Inc/cmd.h"
#include "Protocol.h"

class Command
{
private:
	Protocol* pt;
	mutex mtx;
	//int RunCommand(struct s_cmd* cmd);
	int GetResponse();
public:
	Command(Protocol* protocol);
	int JumpBootloader();
	int TurnOnSpeedControl();
	int TurnOffSpeedControl();
	int SetVoltage(float left, float right);
	int SetSpeed(float left, float right);
	int SetPosition(float left, float right);
	int SetP(float p);
	int SetI(float i);
	int SetD(float d);
	int ClearPosition();
	int GetSpeed(unsigned long* ts, float* left, float* right);
	int GetPosition(unsigned long* ts, float* left, float* right);
	int GetBattery(unsigned long* ts, float* voltage);
	int CalibratePID();
	int System(string command);
};

