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

	return 0;
}