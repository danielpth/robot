#ifndef __COMMAND__
#define __COMMAND__

#include <stdint.h>

enum {
  CMD_JUMP_BOOTLOADER = 1,
  CMD_TURNON_SPEED_CONTROL,
  CMD_TURNOFF_SPEED_CONTROL,
  CMD_SET_VOLTAGE,
  CMD_SET_SPEED,
  CMD_SET_POSITION,
  CMD_SET_P,
  CMD_SET_I,
  CMD_SET_D,
  CMD_CLEAR_POSITION,
  CMD_GET_SPEED,
  CMD_GET_POSITION,
  CMD_GET_BATTERY,
  CMD_MAX
};

enum {
  STATE_START_FRAME = 0,
  STATE_GET_CMD,
  STATE_GET_LENGHT,
  STATE_GET_PAYLOAD,
  STATE_GET_CRC,
  STATE_DONE,
};

enum {
	PROTOCOL_OK = 0,
	PROTOCOL_NONE,
	PROTOCOL_ERR_TIMEOUT,
	PROTOCOL_ERR_CRC,
	PROTOCOL_ERR_LENGTH,
	PROTOCOL_ERR_BUSY,
	PROTOCOL_ERR_HAL,
	PROTOCOL_ERR_INVALID_CMD,
	PROTOCOL_ERR_INVALID_BYTE,
};

#define COMMAND_SOF 2
#define COMMAND_EOF 3

#define ACK 0x55
#define NACK 0x80

struct __attribute__((__packed__)) s_command_status {
  uint8_t cmd;
  unsigned long timestamp;
  float position1;
  float position2;
  float speed1;
  float speed2;
  uint16_t bat;
  uint8_t last;
  uint8_t result;
};

struct __attribute__((__packed__)) s_speed {
  unsigned long timestamp;
  float speed1;
  float speed2;
};

struct __attribute__((__packed__)) s_position {
  unsigned long timestamp;
  float position1;
  float position2;
};

struct __attribute__((__packed__)) s_battery {
  unsigned long timestamp;
  uint16_t battery;
};

struct __attribute__((__packed__)) s_voltage {
  unsigned long timestamp;
  float voltage1;
  float voltage2;
};

void CommandControl(void);

#endif
