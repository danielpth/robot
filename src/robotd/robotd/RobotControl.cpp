#include "RobotControl.h"

void RobotControl::SetCommand(Command* command)
{
	cmd = command;
}

int RobotControl::StartServer()
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
			/*
			int i;
			printf("\nRecebeu: ");
			for (i = 0; i < n; i++)
			{
				printf("0x%02x ", buffer[i]);
			}
			printf("\n");
			*/
			float speed1, speed2, diff;
			speed1 = speed2 = (((float)buffer[1]) - 127.0f) * 180.0f / 127.0f;
			diff = (((float)buffer[2]) - 127.0f) * 180.0f / 127.0f;
			speed1 += diff;
			speed2 -= diff;
			cmd->SetSpeed(speed1, speed2);
			//if (buffer[5]) {
				//system("espeak \"Sai, da, frente\" --stdout | aplay -D 'default'");
			//}
		}
		//printf("Here is the message: %s\n", buffer);
		//n = write(newsockfd, "I got your message", 18);
	}
	if (n < 0) printf("ERROR writing to socket\n");
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	return 0;
}
