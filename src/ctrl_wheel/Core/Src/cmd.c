#include "cmd.h"
#include "main.h"
#include "motor.h"
#include "flash.h"
#include "adc.h"
#include "crc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct s_command_status command_status;

static uint8_t CommandReceive(uint8_t *cmd, uint8_t *buffer, uint8_t size_of_payload) {
  uint8_t data;
  uint8_t state = STATE_START_FRAME;
  uint8_t length, index = 0;
  HAL_StatusTypeDef status;
  uint8_t result = PROTOCOL_NONE;
  uint8_t crc = 0x55;

  do {
    status = HAL_UART_Receive(&huart1, &data, 1, 15);

    if (status == HAL_OK) {
      HAL_UART_Transmit(&huart1, &data, 1, 10);
      crc ^= data;
      switch (state) {
      case STATE_START_FRAME:
        if (data == COMMAND_SOF) {
          state = STATE_GET_CMD;
        }
        break;

      case STATE_GET_CMD:
        *cmd = data;
        state = STATE_GET_LENGHT;
        break;

      case STATE_GET_LENGHT:
        length = data;
        if (length > size_of_payload) {
          state = STATE_START_FRAME;
          status = HAL_ERROR;
          result = PROTOCOL_ERR_LENGTH;
        } else if (length == 0) {
          state = STATE_GET_CRC;
        } else {
          state = STATE_GET_PAYLOAD;
        }
        break;

      case STATE_GET_PAYLOAD:
        buffer[index] = data;
        index++;
        if (index == length)
          state = STATE_GET_CRC;
        break;

      case STATE_GET_CRC:
        if (crc == 0) {
          state = STATE_DONE;
        } else {
          result = PROTOCOL_ERR_CRC;
          status = HAL_ERROR;
        }
        break;

      }
    } else if (status == HAL_TIMEOUT) {
      result = PROTOCOL_ERR_TIMEOUT;
    } else if (status == HAL_BUSY) {
      result = PROTOCOL_ERR_BUSY;
    } else if (status == HAL_ERROR) {
      result = PROTOCOL_ERR_HAL;
    }
  } while ((status == HAL_OK) && (state != STATE_DONE));

  if (state == STATE_DONE) {
    result = PROTOCOL_OK;
  }

  return result;
}

static void CommandTransmit(uint8_t cmd, void *payload, uint8_t length) {
  uint8_t data = COMMAND_SOF, i;
  uint8_t *buffer = payload;
  uint8_t crc = 0x55;
  crc ^= data;
  crc ^= cmd;
  crc ^= length;
  i = 0;
  while (i < length) {
    crc ^= buffer[i];
    i++;
  }
  HAL_UART_Transmit(&huart1, &data, 1, 10);
  HAL_UART_Transmit(&huart1, &cmd, 1, 10);
  HAL_UART_Transmit(&huart1, &length, 1, 10);
  if (length)
    HAL_UART_Transmit(&huart1, buffer, length, length + 1);
  HAL_UART_Transmit(&huart1, &crc, 1, 10);
}

static void CommandJumpBootloader(void) {
  HAL_TIM_Base_Stop_IT(&htim1);
  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Stop(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Stop(&htim3, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Stop(&htim4, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Stop(&htim4, TIM_CHANNEL_2);
  HAL_ADC_Stop(&hadc1);
  HAL_Delay(100);
  flash_jump_to_app();
}
/*
 static void CommandSendStatus(uint8_t last, uint8_t r) {

 // Get status atomic as possible to send to host
 command_status.cmd = 'S';
 command_status.timestamp = HAL_GetTick();
 command_status.bat = BATTERY;
 command_status.position1 = motor1_position;
 command_status.position2 = motor2_position;
 command_status.speed1 = motor1_speed;
 command_status.speed2 = motor2_speed;
 command_status.last = last;
 command_status.result = r;
 CommandTransmit(&command_status, sizeof(command_status));
 }
 */
static void CommandSendResult(uint8_t status) {
  uint8_t rc;
  if (status == PROTOCOL_OK) {
    rc = ACK;
  } else {
    rc = NACK | status;
  }
  CommandTransmit(rc, NULL, 0);
}

static void CommandSetVoltage(uint8_t *buff) {
  struct s_voltage *voltage = (struct s_voltage*) buff;
  Motor1SetVoltage(voltage->voltage1);
  Motor2SetVoltage(voltage->voltage2);
}

static void CommandSetSpeed(uint8_t *buff) {
  struct s_speed *speed = (struct s_speed*) buff;
  Motor1SetReference(speed->speed1);
  Motor2SetReference(speed->speed2);
}

static void CommandSetPosition(uint8_t *buff) {
  //struct s_position *position = (struct s_position*) buff;
  //Motor1SetPosition(position->position1);
  //Motor2SetPosition(position->position2);
}

static void CommandSetP(uint8_t *buff) {
  float *f = (float*) buff;
  P = *f;
}

static void CommandSetI(uint8_t *buff) {
  float *f = (float*) buff;
  I = *f;
}

static void CommandSetD(uint8_t *buff) {
  float *f = (float*) buff;
  D = *f;
}

void CommandGetSpeed(void) {
  struct s_speed speed;
  speed.timestamp = HAL_GetTick();
  speed.speed1 = motor1_speed;
  speed.speed1 = motor2_speed;
  CommandTransmit(CMD_GET_SPEED, &speed, sizeof(speed));
}

void CommandGetPosition(void) {
  struct s_position pos;
  pos.timestamp = HAL_GetTick();
  pos.position1 = motor1_position;
  pos.position2 = motor2_position;
  CommandTransmit(CMD_GET_POSITION, &pos, sizeof(pos));
}

void CommandGetBattery(void) {
  struct s_battery bat;
  bat.timestamp = HAL_GetTick();
  bat.battery = BATTERY;
  CommandTransmit(CMD_GET_BATTERY, &bat, sizeof(bat));
}

uint8_t buf[200];

void CommandControl(void) {
  uint8_t status = 0;
  uint8_t cmd = 0;

  // Get command
  status = CommandReceive(&cmd, buf, sizeof(buf));
  if (status == HAL_OK) {
    switch (cmd) {

    case CMD_JUMP_BOOTLOADER:
      CommandJumpBootloader();
      //CommandSendResult(status);
      break;

    case CMD_TURNON_SPEED_CONTROL:
      MotorTurnOnControlSpeed();
      //CommandSendResult(status);
      break;

    case CMD_TURNOFF_SPEED_CONTROL:
      MotorTurnOffControlSpeed();
      //CommandSendResult(status);
      break;

    case CMD_SET_VOLTAGE:
      CommandSetVoltage(&buf[0]);
      //CommandSendResult(status);
      break;

    case CMD_SET_SPEED:
      CommandSetSpeed(&buf[0]);
      //CommandSendResult(status);
      break;

    case CMD_SET_POSITION:
      CommandSetPosition(&buf[0]);
      //CommandSendResult(status);
      break;

    case CMD_SET_P:
      CommandSetP(&buf[0]);
      //CommandSendResult(status);
      break;

    case CMD_SET_I:
      CommandSetI(&buf[0]);
      //CommandSendResult(status);
      break;

    case CMD_SET_D:
      CommandSetD(&buf[0]);
      //CommandSendResult(status);
      break;

    case CMD_CLEAR_POSITION:
      MotorClearPosition();
      //CommandSendResult(status);
      break;

    case CMD_GET_SPEED:
      //CommandSendResult(status);
      CommandGetSpeed();
      break;

    case CMD_GET_POSITION:
      //CommandSendResult(status);
      CommandGetPosition();
      break;

    case CMD_GET_BATTERY:
      //CommandSendResult(status);
      CommandGetBattery();
      break;

    default:
      //status = PROTOCOL_ERR_INVALID_CMD;
      //CommandSendResult(status);
      break;

    }
  } else if (status != PROTOCOL_ERR_TIMEOUT) {
    //CommandSendResult(status);
  }
}
