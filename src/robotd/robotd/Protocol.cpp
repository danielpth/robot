#include <string.h>
#include "Protocol.h"
#include "../../ctrl_wheel/Core/Inc/cmd.h"


int Protocol::SetSerialPort(SerialPort* serialport)
{
	sp = serialport;
	return 0;
}

int Protocol::Send(char cmd, void* parameter, char length)
{
#define PROTOCOL_SEND_TIMEOUT 20
	char data[200];
	char recv = 0;
	//char* p = (char*)parameter;
	char crc = 0x55;
	int i;

	mtx.lock();

	data[0] = COMMAND_SOF;
	data[1] = cmd;
	data[2] = length;
	memcpy(&data[3], parameter, length);

	for (i = 0; i < length + 3; i++)
	{
		crc ^= data[i];
		sp->WriteByte(data[i]);
		sp->DrainWriteBuffer();
		sp->ReadByte(recv, PROTOCOL_SEND_TIMEOUT);
		if (data[i] != recv) {
			printf("Different byte [%d] [0x%02x] [0x%02x] length [%d]\n", i, data[i], recv, length);
			mtx.unlock();
			return PROTOCOL_ERR_INVALID_BYTE;
		}
	}
	sp->WriteByte(crc);
	// verify crc
	sp->DrainWriteBuffer();
	sp->ReadByte(recv, PROTOCOL_SEND_TIMEOUT);
	if (crc != recv) {
		printf("Different CRC\n");
		mtx.unlock();
		return PROTOCOL_ERR_INVALID_BYTE;
	}

	// check ACK or NACK
	sp->ReadByte(recv, PROTOCOL_SEND_TIMEOUT);
	if (recv != ACK) {
		printf("NACK: 0x%02x\n", recv & 0x7F);
		mtx.unlock();
		return PROTOCOL_ERR_INVALID_BYTE;
	}

	mtx.unlock();
	return PROTOCOL_OK;
}

int Protocol::Receive(char* cmd, void* payload, char size_of_payload)
{
	char data;
	char state = STATE_START_FRAME;
	char length, index = 0;
	char* buffer = (char*)payload;
	char result = PROTOCOL_NONE;
	char crc = 0x55;

	mtx.lock();

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
			//printf("Recebeu SOF\n");
			break;

		case STATE_GET_CMD:
			*cmd = data;
			state = STATE_GET_LENGHT;
			//printf("Recebeu cmd [0x%02x]\n", data);
			break;

		case STATE_GET_LENGHT:
			length = data;
			//printf("Recebeu length %d\n", length);
			if (length > size_of_payload) {
				result = PROTOCOL_ERR_LENGTH;
			}
			else if (length == 0) {
				state = STATE_GET_CRC;
			}
			else {
				state = STATE_GET_PAYLOAD;
			}
			break;

		case STATE_GET_PAYLOAD:
			buffer[index] = data;
			index++;
			//printf("Recebeu payload 0x%02x\n", data);
			if (index == length) {
				state = STATE_GET_CRC;
			}
			break;

		case STATE_GET_CRC:
			if (crc == 0) {
				state = STATE_DONE;
				//printf("CRC OK\n");
			}
			else {
				result = PROTOCOL_ERR_CRC;
				printf("CRC Fail\n");
			}
			break;

		}
	} while ((result == PROTOCOL_NONE) && (state != STATE_DONE));

	if (state == STATE_DONE) {
		result = PROTOCOL_OK;
	}

	mtx.unlock();
	return result;
}
