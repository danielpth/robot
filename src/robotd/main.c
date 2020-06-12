#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include "main.h"

#include "../ctrl_wheel/Core/Inc/command.h"


#define SERIAL_PORT "/dev/ttyUSB0"

int fd;

int open_serial ()
{
	struct termios tty;

	fd = open(SERIAL_PORT, O_SYNC, O_RDWR);
	
	if (fd == -1) {
		printf ("Serial port can not be opened\n");
		printf ("error %d from open\n", errno);
		return -1;
	}
	
	printf ("Serial port opened\n");
	
	
	if (tcgetattr (fd, &tty) != 0)
	{
		printf ("error %d from tcgetattr\n", errno);
		return -1;
	}

	cfsetospeed (&tty, B115200);
	cfsetispeed (&tty, B115200);

	cfmakeraw(&tty);
/*
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
									// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
									// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	//tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB; // set 1 stop bit
	tty.c_cflag &= ~CRTSCTS;
*/
	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		printf ("error %d from tcsetattr\n", errno);
		return -1;
	}
	return fd;
}
	
void CommandTransmit(int fd, void *payload, uint8_t lenght) {
  uint8_t data = COMMAND_SOF;
  write (fd, &data, 1);
  write (fd, &lenght, 1);
  write (fd, payload, lenght);
}


uint8_t CommandReceive(void *payload, uint8_t size_of_payload) {
  uint8_t data;
  uint8_t state = STATE_START_FRAME;
  uint8_t length, index = 0;
  uint8_t *buffer = payload;
  HAL_StatusTypeDef status;
  uint8_t result = HAL_ERROR;


fd_set rfds;
struct timeval tv;
           int retval;

           /* Watch stdin (fd 0) to see when it has input. */

           FD_ZERO(&rfds);
           FD_SET(fd, &rfds);

           /* Wait up to five seconds. */

           tv.tv_sec = 1;
           tv.tv_usec = 0;
#if 0
           retval = select(fd+1, &rfds, NULL, NULL, &tv);
           /* Don't rely on the value of tv now! */

           if (retval == -1)
               perror("select()");
           else if (retval)
               printf("Data is available now.\n");
               /* FD_ISSET(0, &rfds) will be true. */
           else
               printf("No data within five seconds.\n");
#endif




  do {
	retval = select(fd+1, &rfds, NULL, NULL, &tv);
           if (retval == -1)
		   {
               printf("error on serial port\n");
			   return -1;
		   } else if (retval == 0) return HAL_TIMEOUT;
		   
    status = read(fd, &data, 1);

    if (status > 0) {
      switch (state) {
      case STATE_START_FRAME:
        if (data == COMMAND_SOF)
          state = STATE_GET_LENGHT;
		  //printf ("Recebeu SOF\n");
        break;

      case STATE_GET_LENGHT:
        length = data;
        if (length > size_of_payload) {
          state = STATE_START_FRAME;
          status = -1;
        } else {
          state = STATE_GET_PAYLOAD;
		  //printf ("Recebeu length %d\n", length);
        }
        break;

      case STATE_GET_PAYLOAD:
        buffer[index] = data;
        index++;
        if (index == length) {
          state = STATE_DONE;
		  //printf ("Recebeu payload\n");
		}
        break;
      }
    } else {
      result = HAL_ERROR;
    }
  } while ((status > 0) && (state != STATE_DONE));

  if (state == STATE_DONE) {
    //CommandEchoBack(COMMAND_SOF);
    result = HAL_OK;
  }

  return result;
}

void JumpToBootloader()
{
	char payload = COMMAND_JUMP_BOOTLOADER;
	CommandTransmit(fd, &payload, 1);
}

void MotorTurnOnControlSpeed()
{
	char payload = COMMAND_TURNON_SPEED_CONTROL;
	CommandTransmit(fd, &payload, 1);
}

void RobotSetSpeed(double speed1, double speed2)
{
	char payload[9];
	double *speed;
	payload[0] = COMMAND_SET_SPEED;
	speed = (double*) &payload[1];
	*speed = speed1;
	speed = (double*) &payload[1+sizeof(double)];
	*speed = speed2;
	
	CommandTransmit(fd, &payload, sizeof(payload));
}

int main()
{
	int c, s;
	//char buf[50];
	struct s_command_status status;
	
	fd = open_serial ();
	
	if (fd == -1) {
		printf ("Erro\n");
		return -1;
	} else {
		printf ("Serial ready!\n");
	}
	
	do {
		c = CommandReceive(&status, sizeof(status));
		if (c == 0) {
			printf ("Battery: %02.2fV Speed1: %.2f Speed2: %.2f \n", status.bat/4096.0*19.8, status.speed1, status.speed2);
			//printf ("B\n");
		}
	} while (1);
	
	
	do {
	c = getchar();
	switch (c) {
		case 'q':
		break;

		case 'u':
		break;

		case 'w':
		printf ("RobotSetSpeed\n");
		RobotSetSpeed(100, 100);
		break;

		case 's':
		RobotSetSpeed(0, 0);
		break;

		case 'a':
		RobotSetSpeed(-50, 50);
		break;

		case 'd':
		RobotSetSpeed(50, -50);
		break;

		case 'x':
		RobotSetSpeed(-50, -50);
		break;
	}

		s = CommandReceive(&status, sizeof(status));
		if (c == s) {
			printf ("Battery: %02.2fV Speed1: %.2f Speed2: %.2f \n", status.bat/4096.0*19.8, status.speed1, status.speed2);
		}

	} while (c != 'q');
	
	
	
	
	
	
	close (fd);

	return 0;
}

