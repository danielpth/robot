#include "command.h"
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

static uint8_t CommandReceive(void *payload, uint8_t size_of_payload) {
  uint8_t data;
  uint8_t state = STATE_START_FRAME;
  uint8_t length, index = 0;
  uint8_t *buffer = payload;
  HAL_StatusTypeDef status;
  uint8_t result = HAL_ERROR;

  do {
    status = HAL_UART_Receive(&huart1, &data, 1, 1);

    if (status == HAL_OK) {
      switch (state) {
      case STATE_START_FRAME:
        if (data == COMMAND_SOF)
          state = STATE_GET_LENGHT;
        break;

      case STATE_GET_LENGHT:
        length = data;
        if (length > size_of_payload) {
          state = STATE_START_FRAME;
          status = HAL_ERROR;
        } else {
          state = STATE_GET_PAYLOAD;
        }
        break;

      case STATE_GET_PAYLOAD:
        buffer[index] = data;
        index++;
        if (index == length)
          state = STATE_DONE;
        break;
      }
    } else {
      result = status;
    }
  } while ((status == HAL_OK) && (state != STATE_DONE));

  if (state == STATE_DONE) {
    //CommandEchoBack(COMMAND_SOF);
    result = HAL_OK;
  }

  return result;
}

static void CommandTransmit(void *payload, uint8_t lenght) {
  uint8_t data = COMMAND_SOF;
  uint8_t *buffer = payload;
  HAL_UART_Transmit(&huart1, &data, 1, 10);
  HAL_UART_Transmit(&huart1, &lenght, 1, 10);
  HAL_UART_Transmit(&huart1, buffer, lenght, lenght + 1);
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

static void CommandSendStatus(uint8_t r) {

  // Get status atomic as possible to send to host
  command_status.cmd = 'S';
  command_status.timestamp = HAL_GetTick();
  command_status.bat = BATTERY;
  command_status.position1 = motor1_position;
  command_status.position2 = motor2_position;
  command_status.speed1 = motor1_speed;
  command_status.speed2 = motor2_speed;
  command_status.result = r;
  CommandTransmit(&command_status, sizeof(command_status));
}

static void CommandSetVoltage(uint8_t *buff) {
  double *voltage1, *voltage2;
  voltage1 = (double *)&buff[0];
  voltage2 = (double *)&buff[sizeof(double)];
  Motor1SetVoltage(*voltage1);
  Motor2SetVoltage(*voltage2);
}

static void CommandSetSpeed(uint8_t *buff) {
  double *speed1, *speed2;
  speed1 = (double *)&buff[0];
  speed2 = (double *)&buff[sizeof(double)];
  Motor1SetReference(*speed1);
  Motor2SetReference(*speed2);
}

void CommandControl(void) {
  uint8_t status = 0, result = 0;
  uint8_t buf[14];

  // Get command
  status = CommandReceive(buf, sizeof(buf));

  if (status == HAL_OK) {
    result = 1;
    switch (buf[0]) {

    case COMMAND_JUMP_BOOTLOADER:
      CommandJumpBootloader();
      break;

    case COMMAND_TURNON_SPEED_CONTROL:
      MotorTurnOnControlSpeed();
      break;

    case COMMAND_TURNOFF_SPEED_CONTROL:
      MotorTurnOffControlSpeed();
      break;

    case COMMAND_SET_VOLTAGE:
      CommandSetVoltage(&buf[1]);
      break;

    case COMMAND_SET_SPEED:
      CommandSetSpeed(&buf[1]);
      break;

    case COMMAND_SET_POSITION:
      break;

    case COMMAND_SET_P:
      break;

    case COMMAND_SET_I:
      break;

    case COMMAND_SET_D:
      break;

    case COMMAND_CLEAR_POSITION:
      MotorClearPosition();
      break;

    default:
      result = 2;
      break;

    }
  } else if (status == HAL_TIMEOUT) {
    result = 0;
  }
  CommandSendStatus(result);
}
