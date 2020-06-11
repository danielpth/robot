#ifndef __COMMAND__
#define __COMMAND__

#include "main.h"

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

void CommandControl(void);

#endif
