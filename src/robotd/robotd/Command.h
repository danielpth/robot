#pragma once
#include <mutex>
#include "Protocol.h"
/*
struct __attribute__((__packed__)) s_cmd {
	char cmd;
	char parameter[200];
};
*/

class Command
{
private:
	Protocol* pt;
	mutex mtx;
	//int RunCommand(struct s_cmd* cmd);
	int GetResponse();
public:
	int SetProtocol(Protocol* protocol);
	//int Handler();

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
};

