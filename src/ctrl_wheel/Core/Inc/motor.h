#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "main.h"

#define MOTOR1_CURRENT  ADC1->JDR1
#define MOTOR2_CURRENT  ADC1->JDR2
#define BATTERY         ADC1->JDR3
#define TEMPERATURE     ADC1->JDR4
#define BAT_mV          (BATTERY*19800)/4096

#define MOTOR_MAX_SPEED 180.0

extern volatile double motor1_speed, motor2_speed;
extern volatile double motor1_position, motor2_position;

void Motor1SetReference(double ref);
void Motor2SetReference(double ref);
void MotorControlSpeed(void);
void MotorControlVoltage(void);

#endif
