#include "command.h"
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

struct s_command_status {
  uint32_t timestamp;
  int32_t position1;
  int32_t position2;
  double speed1;
  double speed2;
  uint16_t bat;
} command_status;

void CommandJumpBootloader(void) {
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

void CommandEchoBack(uint8_t echo) {
  // Send echo back indicating that command was received
  HAL_UART_Transmit(&huart1, &echo, 1, 100);
}

void SendStatus(void) {
  //sprintf((char*)buffer, "R: %08x %04x %08x %08x %05x %05x\n\r", command_status.timestamp, command_status.bat, command_status.position1,
      //command_status.position2, (int) (command_status.speed1 * 1000.0), (int) (command_status.speed1 * 1000.0));

}

void CommandControl(void) {
  HAL_StatusTypeDef status;
  uint8_t command;
  // Get status atomic as possible to send to host
  command_status.timestamp = HAL_GetTick();
  command_status.bat = BATTERY;
  command_status.position1 = motor1_position;
  command_status.position2 = motor2_position;
  command_status.speed1 = motor1_speed;
  command_status.speed2 = motor2_speed;
  HAL_UART_Transmit(&huart1, (uint8_t*) &command_status, sizeof(command_status), 100);

  // Get command by serial
  status = HAL_UART_Receive(&huart1, &command, 1, 10);

  if (status == HAL_OK) {
    switch (command) {

    case COMMAND_JUMP_BOOTLOADER:
      CommandEchoBack(COMMAND_JUMP_BOOTLOADER);
      CommandJumpBootloader();
      break;

    case COMMAND_TURNON_SPEED_CONTROL:
      CommandEchoBack(COMMAND_TURNON_SPEED_CONTROL);
      MotorTurnOnControlSpeed();
      break;

    case COMMAND_TURNOFF_SPEED_CONTROL:
      CommandEchoBack(COMMAND_TURNOFF_SPEED_CONTROL);
      MotorTurnOffControlSpeed();
      break;

    case COMMAND_SET_VOLTAGE:
      CommandEchoBack(COMMAND_SET_VOLTAGE);
      Motor1SetVoltage(0);
      Motor2SetVoltage(0);
      break;

    case COMMAND_SET_SPEED:
      CommandEchoBack(COMMAND_SET_SPEED);
      Motor1SetReference(0);
      Motor2SetReference(0);
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
      CommandEchoBack(COMMAND_CLEAR_POSITION);
      MotorClearPosition();
      break;

    }
  }
}
