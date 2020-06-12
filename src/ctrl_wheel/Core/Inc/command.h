#ifndef __COMMAND__
#define __COMMAND__

#include <stdint.h>

#define COMMAND_JUMP_BOOTLOADER 1
#define COMMAND_TURNON_SPEED_CONTROL 2
#define COMMAND_TURNOFF_SPEED_CONTROL 3
#define COMMAND_SET_VOLTAGE 4
#define COMMAND_SET_SPEED 5
#define COMMAND_SET_POSITION 6
#define COMMAND_SET_P 7
#define COMMAND_SET_I 8
#define COMMAND_SET_D 9
#define COMMAND_CLEAR_POSITION 10
#define COMMAND_SET_RESULT 11


#define STATE_START_FRAME 0
#define STATE_GET_LENGHT 1
#define STATE_GET_PAYLOAD 2
#define STATE_GET_CRC 3
#define STATE_DONE 4


#define COMMAND_SOF 2
#define COMMAND_EOF 3

struct __attribute__((__packed__)) s_command_status {
  uint8_t cmd;
  uint32_t timestamp;
  float position1;
  float position2;
  float speed1;
  float speed2;
  uint16_t bat;
  uint8_t last;
  uint8_t result;
};

void CommandControl(void);

#endif
