#include "Command.h"

int Command::GetResponse()
{
	return 0;
}

Command::Command(Protocol* protocol)
{
	if (protocol == NULL) {
		printf("Invalid protocol\n");
		exit(EXIT_FAILURE);
	}
	pt = protocol;
}

int Command::JumpBootloader()
{
	int rc;
	mtx.lock();
	rc = pt->Send(CMD_JUMP_BOOTLOADER, NULL, 0);
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::TurnOnSpeedControl()
{
	int rc;
	mtx.lock();
	rc = pt->Send(CMD_TURNON_SPEED_CONTROL, NULL, 0);
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::TurnOffSpeedControl()
{
	int rc;
	mtx.lock();
	rc = pt->Send(CMD_TURNOFF_SPEED_CONTROL, NULL, 0);
	if (rc == PROTOCOL_OK) {
		GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::SetVoltage(float left, float right)
{
	int rc;
	struct s_voltage voltage;
	mtx.lock();
	voltage.voltage1 = right;
	voltage.voltage2 = left;
	rc = pt->Send(CMD_SET_VOLTAGE, &voltage, sizeof(voltage));
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::SetSpeed(float left, float right)
{
	int rc;
	struct s_speed speed;
	mtx.lock();
	speed.speed1 = right;
	speed.speed2 = left;
	rc = pt->Send(CMD_SET_SPEED, &speed, sizeof(speed));
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::SetPosition(float left, float right)
{
	int rc;
	struct s_position pos;
	mtx.lock();
	pos.position1 = right;
	pos.position2 = left;
	rc = pt->Send(CMD_SET_POSITION, &pos, sizeof(pos));
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::SetP(float p)
{
	int rc;
	mtx.lock();
	rc = pt->Send(CMD_SET_P, &p, sizeof(p));
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::SetI(float i)
{
	int rc;
	mtx.lock();
	rc = pt->Send(CMD_SET_I, &i, sizeof(i));
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::SetD(float d)
{
	int rc;
	mtx.lock();
	rc = pt->Send(CMD_SET_D, &d, sizeof(d));
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}

int Command::ClearPosition()
{
	int rc;
	mtx.lock();
	rc = pt->Send(CMD_CLEAR_POSITION, NULL, 0);
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
	}
	mtx.unlock();
	return rc;
}
int Command::GetSpeed(unsigned long* ts, float* left, float* right)
{
	int rc;
	char cmd;
	struct s_speed speed;
	mtx.lock();
	rc = pt->Send(CMD_GET_SPEED, NULL, 0);
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return rc;
	}
	if (rc == PROTOCOL_OK) {
		rc = pt->Receive(&cmd, &speed, sizeof(speed));
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return rc;
	}
	if ((rc == PROTOCOL_OK) && (cmd == CMD_GET_SPEED)) {
		*ts = speed.timestamp;
		*left = speed.speed2;
		*right = speed.speed1;
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return PROTOCOL_ERR_INVALID_CMD;
	}
	mtx.unlock();
	return rc;
}
int Command::GetPosition(unsigned long* ts, float* left, float* right)
{
	int rc;
	char cmd;
	struct s_position pos;
	mtx.lock();
	rc = pt->Send(CMD_GET_POSITION, NULL, 0);

	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return rc;
	}

	if (rc == PROTOCOL_OK) {
		rc = pt->Receive(&cmd, &pos, sizeof(pos));
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return rc;
	}
	if ((rc == PROTOCOL_OK) && (cmd == CMD_GET_POSITION)) {
		*ts = pos.timestamp;
		*left = pos.position2;
		*right = pos.position1;
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return PROTOCOL_ERR_INVALID_CMD;
	}

	mtx.unlock();
	return rc;
}
int Command::GetBattery(unsigned long* ts, float* voltage)
{
	int rc;
	char cmd;
	struct s_battery bat;
	mtx.lock();
	rc = pt->Send(CMD_GET_BATTERY, NULL, 0);
	if (rc == PROTOCOL_OK) {
		rc = GetResponse();
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return rc;
	}

	if (rc == PROTOCOL_OK) {
		rc = pt->Receive(&cmd, &bat, sizeof(bat));
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return rc;
	}

	if ((rc == PROTOCOL_OK) && (cmd == CMD_GET_BATTERY)) {
		*ts = bat.timestamp;
		*voltage = bat.battery / 4096.0f * 19.8f;
	}
	else {
		printf("%s\n", __FUNCTION__);
		mtx.unlock();
		return PROTOCOL_ERR_INVALID_CMD;
	}

	mtx.unlock();
	return rc;
}
int Command::CalibratePID()
{
	float p = 1, i = 0, d = 0;
	float p1, p2, speed;
	unsigned long ts, ts0;
	int rc;

	SetP(p);
	SetI(i);
	SetD(d);
	TurnOnSpeedControl();

	printf("Ajustando P\n");
	SetSpeed(0, 0);
	speed = 0;
	sleep(1);
	p = 3.6f;
	//p = 7;
	SetP(p);
	/*
	do {
		rc = GetSpeed(&ts0, &p1, &p2);
	} while (rc != 0);
	SetSpeed(80, 80);
	while (1) {
		rc = GetSpeed(&ts, &p1, &p2);

		if (rc == 0) {
			if (p2 > speed) speed = p2;
			//printf("s1=%f s2=%f p=%f ts=%ul\n", p1, p2, i, ts);
			if (ts - ts0 > 1000) {
				// reset
				SetSpeed(0, 0);
				sleep(1);
				printf("p=%f s=%f max=%f\n", p, p2, speed);
				if (speed > 80.0f) {
					break;
				}
				p = p + 0.1f;
				SetP(p);
				speed = 0;
				do {
					rc = GetSpeed(&ts0, &p1, &p2);
				} while (rc != 0);
				SetSpeed(80, 80);
			}
		}
	}
	*/

	printf("Ajustando I\n");
	SetSpeed(0, 0);
	speed = 0;
	sleep(1);
	i = 6;
	SetI(i);
	/*
	do {
		rc = GetSpeed(&ts0, &p1, &p2);
	} while (rc != 0);
	SetSpeed(80, 80);
	while (1) {
		rc = GetSpeed(&ts, &p1, &p2);

		if (rc == 0) {
			if (p2 > speed) speed = p2;
			//printf("s1=%f s2=%f p=%f ts=%ul\n", p1, p2, i, ts);
			if (ts - ts0 > 1000) {
				// reset
				SetSpeed(0, 0);
				sleep(1);
				i = i + 0.1f;
				SetI(i);
				printf("i=%f s=%f max=%f\n", i, p2, speed);
				speed = 0;
				do {
					rc = GetSpeed(&ts0, &p1, &p2);
				} while (rc != 0);
				SetSpeed(80, 80);
			}
			if (p2 > 80.0f) {
				break;
			}
		}
	}

	i = i - 0.2f;
	SetI(i);
	*/


	printf("Ajustando D\n");
	SetSpeed(0, 0);
	speed = 0;
	sleep(1);
	d = 0.01f;
	SetD(d);
	do {
		rc = GetSpeed(&ts0, &p1, &p2);
	} while (rc != 0);
	SetSpeed(80, 80);
	while (1) {
		rc = GetSpeed(&ts, &p1, &p2);

		if (rc == 0) {
			if (p2 > speed) speed = p2;
			//printf("s1=%f s2=%f p=%f ts=%ul\n", p1, p2, i, ts);
			if (ts - ts0 > 1000) {
				// reset
				SetSpeed(0, 0);
				sleep(1);
				if (speed > 80) {
					printf("d=%f s=%f s=%f max=%f\n", d, p1, p2, speed);
					break;
				}
				d = d + 0.001f;
				SetD(d);
				printf("d=%f s=%f s=%f max=%f\n", d, p1, p2, speed);
				speed = 0;
				do {
					rc = GetSpeed(&ts0, &p1, &p2);
				} while (rc != 0);
				SetSpeed(80, 80);
			}

		}
	}


	printf("p=%f i=%f d=%f\n", p, i, d);

	SetSpeed(0, 0);
	return 0;
}

int Command::System(string command)
{
	int rc;
	rc = system(command.c_str());
	if (rc != 0) {
		printf("%s system fail: rc=[%d] errno=[%d]\n", __FUNCTION__, rc, errno);
	}
	return rc;
}
