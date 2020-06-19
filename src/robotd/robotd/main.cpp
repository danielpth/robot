#include <cstdio>
#include <unistd.h>
#include "SerialPort.h"
#include "Protocol.h"
#include "Command.h"
#include "RobotControl.h"

using namespace std;
using namespace LibSerial;

int main()
{
	SerialPort sp;
	Protocol pt;
	Command cmd;
	RobotControl ctrl;
	int rc;

	// Configure serial port
	sp.Open("/dev/ttyUSB0");
	sp.SetDefaultSerialPortParameters();
	sp.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
	sp.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
	sp.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
	sp.SetParity(LibSerial::Parity::PARITY_NONE);
	sp.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

	pt.SetSerialPort(&sp);

	cmd.SetProtocol(&pt);

	cmd.TurnOnSpeedControl();
	//cmd.CalibratePID();

	ctrl.SetCommand(&cmd);

	ctrl.StartServer();

	//system("espeak \"Sai, da, frente\" --stdout | aplay -D 'default'");


	/*
	while (1) {
		unsigned long ts;
		float left, right;
		//rc = cmd.GetSpeed(&ts, &left, &right);
		//rc = cmd.GetPosition(&ts, &left, &right);
		rc = cmd.SetSpeed(40, 40);
		if (rc == 0) {
			printf("ts=%ul left=%f right=%f\n", ts, left, right);
		}
		else {
			printf("GetSpeed error: %d\n", rc);
			return 0;
		}
	}
	*/
	return 0;
}