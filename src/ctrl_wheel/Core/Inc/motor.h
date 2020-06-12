#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "main.h"

#define MOTOR1_CURRENT  ADC1->JDR1
#define MOTOR2_CURRENT  ADC1->JDR2
#define BATTERY         ADC1->JDR3
#define TEMPERATURE     ADC1->JDR4
#define BAT_mV          (BATTERY*19800)/4096 // 19.8 V = 19800mV

#define MOTOR_MAX_SPEED 180.0
#define MOTOR_MAX_VOLTAGE 12

// 22 pulses per revolution on motor
// gear 1:75
// 1650 pulses per revolution on wheel
// max distance for encoder 4990.9867272727272727272727272727 mm
#define MOTOR_TIME_INTERVAL 50 // ms
#define MOTOR_PULSES 22.0
#define MOTOR_RATIO 75.0
#define WHEEL_RADIUS 20.0 // mm
#define WHEEL_PULSES (MOTOR_PULSES * MOTOR_RATIO) // 1650.0

extern volatile float motor1_speed, motor2_speed;
extern volatile float motor1_position, motor2_position;

void Motor1SetReference(float ref);
void Motor2SetReference(float ref);
void MotorCalculate (void);
void MotorControlSpeed(void);
void MotorControlVoltage(void);
void MotorTurnOnControlSpeed(void);
void MotorTurnOffControlSpeed(void);

void Motor1SetVoltage(float voltage);
void Motor2SetVoltage(float voltage);
void MotorClearPosition(void);

#endif
