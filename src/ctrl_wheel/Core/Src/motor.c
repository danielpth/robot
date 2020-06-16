#include "motor.h"

#include <stdbool.h>
volatile float motor1_speed, motor2_speed; // RPM wheel
volatile float motor1_error_old, motor2_error_old;
volatile float motor1_position, motor2_position; // mm
volatile float motor1_integration, motor2_integration;
volatile float motor1_ref, motor2_ref;
volatile float motor1_error, motor2_error;
volatile float motor1_voltage, motor2_voltage;
volatile float cnt1, cnt2;
volatile float P = 3.6, I = 6.0, D = 0.011;
float Kv = 12.0 / 180.0; // 12V sao 180 RPM na roda
bool motor_control_speed = false, motor_control_position = false;

void Motor1SetPWM(int pwm1) {
  if (pwm1 > 0) {
    HAL_GPIO_WritePin(Motor1_INA_GPIO_Port, Motor1_INA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor1_INB_GPIO_Port, Motor1_INB_Pin, GPIO_PIN_RESET);
    TIM2->CCR1 = pwm1;

  } else if (pwm1 < 0) {
    HAL_GPIO_WritePin(Motor1_INA_GPIO_Port, Motor1_INA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor1_INB_GPIO_Port, Motor1_INB_Pin, GPIO_PIN_SET);
    TIM2->CCR1 = -pwm1;

  } else {
    HAL_GPIO_WritePin(Motor1_INA_GPIO_Port, Motor1_INA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor1_INB_GPIO_Port, Motor1_INB_Pin, GPIO_PIN_RESET);
    TIM2->CCR1 = 0;
  }
}

void Motor2SetPWM(int pwm2) {
  if (pwm2 > 0) {
    HAL_GPIO_WritePin(Motor2_INA_GPIO_Port, Motor2_INA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor2_INB_GPIO_Port, Motor2_INB_Pin, GPIO_PIN_RESET);
    TIM2->CCR2 = pwm2;

  } else if (pwm2 < 0) {
    HAL_GPIO_WritePin(Motor2_INA_GPIO_Port, Motor2_INA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor2_INB_GPIO_Port, Motor2_INB_Pin, GPIO_PIN_SET);
    TIM2->CCR2 = -pwm2;

  } else {
    HAL_GPIO_WritePin(Motor2_INA_GPIO_Port, Motor2_INA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor2_INB_GPIO_Port, Motor2_INB_Pin, GPIO_PIN_RESET);
    TIM2->CCR2 = 0;

  }
}

void Motor1SetVoltage(float voltage) {
  if (voltage > MOTOR_MAX_VOLTAGE)
    voltage = MOTOR_MAX_VOLTAGE;
  if (voltage < -MOTOR_MAX_VOLTAGE)
    voltage = -MOTOR_MAX_VOLTAGE;
  motor1_voltage = voltage;
}

void Motor2SetVoltage(float voltage) {
  if (voltage > MOTOR_MAX_VOLTAGE)
    voltage = MOTOR_MAX_VOLTAGE;
  if (voltage < -MOTOR_MAX_VOLTAGE)
    voltage = -MOTOR_MAX_VOLTAGE;
  motor2_voltage = voltage;
}

void Motor1SetReference(float ref) {
  if (ref > MOTOR_MAX_SPEED)
    ref = MOTOR_MAX_SPEED;
  if (ref < -MOTOR_MAX_SPEED)
    ref = -MOTOR_MAX_SPEED;
  motor1_ref = ref;
}

void Motor2SetReference(float ref) {
  if (ref > MOTOR_MAX_SPEED)
    ref = MOTOR_MAX_SPEED;
  if (ref < -MOTOR_MAX_SPEED)
    ref = -MOTOR_MAX_SPEED;
  motor2_ref = ref;
}

void MotorCalculate(void) {
  cnt1 = (int16_t) TIM3->CNT;
  TIM3->CNT = 0;
  cnt2 = (int16_t) TIM4->CNT;
  TIM4->CNT = 0;
  motor1_position += cnt1 * WHEEL_RADIUS * 2.0 * 3.1415 / MOTOR_PULSES / MOTOR_RATIO; // linear mm
  motor2_position += cnt2 * WHEEL_RADIUS * 2.0 * 3.1415 / MOTOR_PULSES / MOTOR_RATIO; // linear mm
  motor1_speed = cnt1 / (MOTOR_TIME_INTERVAL / 1000.0) / WHEEL_PULSES * 60.0; // RPM wheel
  motor2_speed = cnt2 / (MOTOR_TIME_INTERVAL / 1000.0) / WHEEL_PULSES * 60.0; // RPM wheel
}

void MotorControlSpeed(void) {
  if (motor_control_speed) {
    float voltage1, voltage2;
    float deriv1 = 0, deriv2 = 0;

    // controle PID Motor1
    motor1_error = motor1_ref - motor1_speed;
    motor1_integration += motor1_error * (MOTOR_TIME_INTERVAL / 1000.0);
    deriv1 = (motor1_error - motor1_error_old) / (MOTOR_TIME_INTERVAL / 1000.0);
    motor1_error_old = motor1_error;
    voltage1 = ((motor1_integration * I) + (motor1_error * P) + (deriv1 * D)) * Kv;

    // controle PID Motor2
    motor2_error = motor2_ref - motor2_speed;
    motor2_integration += motor2_error * (MOTOR_TIME_INTERVAL / 1000.0);
    deriv2 = (motor2_error - motor2_error_old) / (MOTOR_TIME_INTERVAL / 1000.0);
    motor2_error_old = motor2_error;
    voltage2 = ((motor2_integration * I) + (motor2_error * P) + (deriv2 * D)) * Kv;

    // Set Voltage
    if ((motor1_ref == 0) && (motor1_speed == 0)) {
      motor1_integration = 0;
      Motor1SetVoltage(0);
    } else {
      Motor1SetVoltage(voltage1);
    }

    if ((motor2_ref == 0) && (motor2_speed == 0)) {
      motor2_integration = 0;
      Motor2SetVoltage(0);
    } else {
      Motor2SetVoltage(voltage2);
    }
  }
}

void MotorControlVoltage(void) {
  // Check batery voltage and apply to motors
  // 65535*4096/19,8 = 13557139,39
  float pwm, correction;
  if (BATTERY) {
    correction = 13557139.39 / ((float) BATTERY);
  } else {
    correction = 65535.0 / 16.8; // Tensao maxima da bateria eh 16.8V
  }
  pwm = motor1_voltage * correction;
  Motor1SetPWM((int) pwm);
  pwm = motor2_voltage * correction;
  Motor2SetPWM((int) pwm);
}

void MotorTurnOnControlSpeed(void) {
  motor_control_speed = true;
}

void MotorTurnOffControlSpeed(void) {
  motor_control_speed = false;
}

void MotorClearPosition(void) {
  motor1_position = 0;
  motor1_position = 0;
}
