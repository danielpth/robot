#include "RemoteControl.h"

void RemoteControl::batteryMonitor()
{
	unsigned long ts;
	float voltage;
	printf("batteryMonitor\n");

	while (runBatteryMonitor) {
		if (cmd) {
			cmd->GetBattery(&ts, &voltage);
			printf("Battery: %.2f\n", voltage);

			if (voltage < 12.0f) {
				Speak("Battery very low!");
				Speak("Power off");
				cmd->System("sudo poweroff");
			}
			else if (voltage < 12.8f) {
				Speak("Battery low!");
			}
		}
		sleep(1);
	}
}

RemoteControl::RemoteControl(Command* command)
{
	if (command == NULL) {
		printf("Invalid command\n");
		exit(EXIT_FAILURE);
	}
	cmd = command;
	runBatteryMonitor = true;
	threadBatteryMonitor = new thread(&RemoteControl::batteryMonitor, this);
	cmd->SetSpeed(0, 0);
	cmd->TurnOnSpeedControl();
}

RemoteControl::~RemoteControl()
{
	runBatteryMonitor = false;
	if (threadBatteryMonitor)
		threadBatteryMonitor->join();
	cmd->TurnOffSpeedControl();
	cmd->SetVoltage(0, 0);
	delete threadBatteryMonitor;
}

int RemoteControl::StartServer()
{
	int sockfd, newsockfd/*, portno*/, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	printf("Starting Remote control\n");
	cmd->SetSpeed(0, 0);
	cmd->TurnOnSpeedControl();

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("ERROR opening socket\n");
	memset(&serv_addr, 0, sizeof(serv_addr));
	//portno = 7632;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(7632);
	if (bind(sockfd, (struct sockaddr*)&serv_addr,
		sizeof(serv_addr)) < 0)
		printf("ERROR on binding\n");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, (socklen_t*)&clilen);
	if (newsockfd < 0)
		printf("ERROR on accept\n");
	else
		printf("Client connected\n");
	while (1) {
		bzero(buffer, 256);
		//n = read(newsockfd, buffer, 255);
		n = read(newsockfd, buffer, 6);
		if (n < 0) {
			printf("ERROR reading from socket\n");
		}
		else if (n == 0) {
			cmd->SetSpeed(0, 0);
		}
		else {
			float speed1, speed2, diff;
			speed1 = speed2 = (((float)buffer[1]) - 127.0f) * 180.0f / 127.0f;
			diff = (((float)buffer[2]) - 127.0f) * 180.0f / 127.0f;
			speed1 += diff;
			speed2 -= diff;
			cmd->SetSpeed(speed1, speed2);
			if (buffer[5]) {
				Speak("Sai, da, frente");
			}
		}
		//printf("Here is the message: %s\n", buffer);
		//n = write(newsockfd, "I got your message", 18);
	}
	if (n < 0) printf("ERROR writing to socket\n");
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	return 0;
}

void RemoteControl::Speak(string sentence)
{
	string command;
	command += "espeak \"";
	command += sentence;
	command += "\" --stdout | aplay -D 'default'";
	cmd->System(command);
}
