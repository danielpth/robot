#pragma once
#include <mutex>
#include "SerialPort.h"

using namespace std;
using namespace LibSerial;

class Protocol
{
private:
	SerialPort* sp;
	mutex mtx;
public:
	Protocol(SerialPort* serialport);
	int Send(char cmd, void* parameter, char length);
	int Receive(char* cmd, void* payload, char size_of_payload);
};

