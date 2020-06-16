#include "Protocol.h"
#include "../../ctrl_wheel/Core/Inc/cmd.h"


int Protocol::SetSerialPort(SerialPort* serialport)
{
	sp = serialport;
	return 0;
}

int Protocol::Send(char cmd, void* parameter, char length)
{
	char data = COMMAND_SOF;
	char recv = 0;
	char* p = (char*)parameter;
	char crc = 0x55;
	int i;
	crc ^= data;
	crc ^= cmd;
	crc ^= length;
	sp->WriteByte(data);
	sp->DrainWriteBuffer();
	sp->ReadByte(recv, 100);
	if (data != recv) return PROTOCOL_ERR_INVALID_BYTE;
	sp->WriteByte(cmd);
	sp->DrainWriteBuffer();
	sp->ReadByte(recv, 100);
	if (cmd != recv) return PROTOCOL_ERR_INVALID_BYTE;
	sp->WriteByte(length);
	sp->DrainWriteBuffer();
	sp->ReadByte(recv, 100);
	if (length != recv) return PROTOCOL_ERR_INVALID_BYTE;
	i = 0;
	while (i < length) {
		crc ^= p[i];
		sp->WriteByte(p[i]);
		sp->DrainWriteBuffer();
		sp->ReadByte(recv, 100);
		if (p[i] != recv) return PROTOCOL_ERR_INVALID_BYTE;
		i++;
	}
	sp->WriteByte(crc);
	sp->DrainWriteBuffer();
	sp->ReadByte(recv, 100);
	if (length != recv) return PROTOCOL_ERR_INVALID_BYTE;
	return 0;
}

int Protocol::Receive(char* cmd, void* payload, char size_of_payload)
{
	char data;
	char state = STATE_START_FRAME;
	char length, index = 0;
	char* buffer = (char*)payload;
	char result = PROTOCOL_NONE;
	char crc = 0x55;

	do {
		try {
			sp->ReadByte(data, 100);
		}
		catch (ReadTimeout& rt) {
			//printf("Timeout\n");
			result = PROTOCOL_ERR_TIMEOUT;
			break;
		}

		crc ^= data;
		switch (state) {
		case STATE_START_FRAME:
			if (data == COMMAND_SOF)
				state = STATE_GET_CMD;
			//printf ("Recebeu SOF\n");
			break;

		case STATE_GET_CMD:
			*cmd = data;
			state = STATE_GET_LENGHT;
			break;

		case STATE_GET_LENGHT:
			length = data;
			if (length > size_of_payload) {
				result = PROTOCOL_ERR_LENGTH;
			}
			else if (length == 0) {
				state = STATE_GET_CRC;
				//printf ("Recebeu length %d\n", length);
			}
			else {
				state = STATE_GET_PAYLOAD;
				//printf ("Recebeu length %d\n", length);
			}
			break;

		case STATE_GET_PAYLOAD:
			buffer[index] = data;
			index++;
			if (index == length) {
				state = STATE_GET_CRC;
				//printf ("Recebeu payload\n");
			}
			break;

		case STATE_GET_CRC:
			if (crc == 0) {
				state = STATE_DONE;
				//printf("CRC OK\n");
			}
			else {
				result = PROTOCOL_ERR_CRC;
				//printf("CRC Fail\n");
			}
			break;

		}
	} while ((result == PROTOCOL_NONE) && (state != STATE_DONE));

	if (state == STATE_DONE) {
		result = PROTOCOL_OK;
	}

	return result;
}
