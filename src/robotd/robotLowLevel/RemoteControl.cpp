#include "RemoteControl.h"

void RemoteControl::batteryMonitor()
{
	int rc;
	unsigned long ts;
	float voltage;
	printf("batteryMonitor\n");

	runBatteryMonitor = true;

	while (runBatteryMonitor) {
		if (cmd) {
			rc = cmd->GetBattery(&ts, &voltage);
			if (rc == PROTOCOL_OK)
			{
				printf("Battery: %.2f\n", voltage);
				
				if (voltage < 12.0f) {
					Speak("Bateria muito baixa!");
					sleep(3);
					Speak("Desligando");
					cmd->System("sudo poweroff");
				}
				else if (voltage < 12.8f) {
					Speak("Bateria baixa");
				}
			}
		}
		sleep(3);
	}
}

RemoteControl::RemoteControl(Command* command)
{
	if (command == NULL) {
		printf("Invalid command\n");
		exit(EXIT_FAILURE);
	}
	cmd = command;
	runBatteryMonitor = false;
	remoteControl = false;
	threadBatteryMonitor = new thread(&RemoteControl::batteryMonitor, this);
	cmd->TurnOffSpeedControl();
	cmd->SetVoltage(0, 0);
}

RemoteControl::~RemoteControl()
{
	runBatteryMonitor = false;
	if (threadBatteryMonitor)
		threadBatteryMonitor->join();
	StopServer();
	cmd->TurnOffSpeedControl();
	cmd->SetVoltage(0, 0);
	delete threadBatteryMonitor;
}

int RemoteControl::StopServer()
{
	printf("Stop remote control\n");
	if (remoteControl) {
		printf("Disconnecting all clients\n");
		for (std::list<int>::iterator it = clientSocketfd.begin(); it != clientSocketfd.end(); ++it)
		{
			printf("Disconnecting (%d)\n", *it);
			shutdown(*it, SHUT_RDWR);
			close(*it);
		}
		remoteControl = false;

		shutdown(bindSockfd, SHUT_RDWR);
		close(bindSockfd);
	}
	return 0;
}

int RemoteControl::StartServer()
{
	threadServerRemoteControl = new thread(&RemoteControl::RemoteControlTread, this);
	return 0;
}

int RemoteControl::RemoteControlTread()
{
	int clilen, newsockfd;
	struct sockaddr_in serv_addr, cli_addr;

	printf("Starting Remote control\n");
	cmd->SetSpeed(0, 0);
	cmd->TurnOnSpeedControl();

	bindSockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (bindSockfd < 0) {
		printf("ERROR opening socket\n");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	//portno = 7632;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(7632);
	if (bind(bindSockfd, (struct sockaddr*)&serv_addr,
		sizeof(serv_addr)) < 0) {
		printf("ERROR on binding\n");
		return -1;
	}

	listen(bindSockfd, 2);
	clilen = sizeof(cli_addr);

	remoteControl = true;

	while (remoteControl) {
		newsockfd = accept(bindSockfd, (struct sockaddr*)&cli_addr, (socklen_t*)&clilen);
		if (newsockfd < 0) {
			printf("ERROR on accept\n");
		}
		else {
			printf("Client connected\n");
			thread* t = new thread(&RemoteControl::HandleClient, this, newsockfd);
			clientSocketfd.push_back(newsockfd);
		}
	}

	printf("Closing remote control\n");

	shutdown(bindSockfd, SHUT_RDWR);
	close(bindSockfd);

	cmd->TurnOffSpeedControl();
	cmd->SetVoltage(0, 0);

	return 0;
}

int RemoteControl::HandleClient(int clientSocket)
{
	float speed1_old = 0, speed2_old = 0;
	char buffer[256];
	size_t n;
	int rc;
	fd_set read_sd;
	FD_ZERO(&read_sd);
	FD_SET(clientSocket, &read_sd);

	while (1) {
		fd_set rsd = read_sd;

		int sel = select(clientSocket + 1, &rsd, 0, 0, 0);

		if (sel > 0) {
			// client has performed some activity (sent data or disconnected?)
			bzero(buffer, 256);
			n = recv(clientSocket, buffer, sizeof(buffer), 0);

			if (n > 0) {
				// got data from the client.
				/*for (int i = 0; i < n; i++) {
					printf("Buffer(%x) valor: 0x%x\n");
				}*/

				float speed1, speed2, diff;
				speed1 = speed2 = (((float)buffer[1]) - 127.0f) * 180.0f / 127.0f;
				diff = (((float)buffer[2]) - 127.0f) * 180.0f / 127.0f;
				speed1 += diff;
				speed2 -= diff;
				if ((speed1 != speed1_old) || (speed2 != speed2_old))
				{
					do {
						rc = cmd->SetSpeed(speed1, speed2);
					} while (rc != PROTOCOL_OK);

					if (rc == PROTOCOL_OK)
					{
						speed1_old = speed1;
						speed2_old = speed2;
					}
				}
				if (buffer[5]) {
					Speak("Sai da frente");
				}

			}
			else if (n == 0) {
				// client disconnected.
				cmd->SetSpeed(0, 0);
				printf("Client disconnected.\n");
				break;
			}
			else {
				// error receiving data from client. You may want to break from
				// while-loop here as well.
				cmd->SetSpeed(0, 0);
				printf("Error receiving data\n");
				break;
			}
		}
		else if (sel < 0) {
			// grave error occurred.
			cmd->SetSpeed(0, 0);
			printf("Error receiving data\n");
			break;
		}
	}
	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
	clientSocketfd.remove(clientSocket);
	return 0;
}

void RemoteControl::Speak(string sentence)
{
	string command;
	command += "espeak \"";
	command += sentence;
	command += "\" --stdout -vbrazil | aplay -D 'default' &";
	cmd->System(command);
}
