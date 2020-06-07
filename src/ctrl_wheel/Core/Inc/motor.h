#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "main.h"

extern volatile double motor1_speed, motor2_speed, motor1_position,
		motor2_position, motor1_integration, motor2_integration;

extern volatile double motor1_position, motor2_position;

void Motor1SetPWM(int pwm1);

void MotorSetReference(double ref1, double ref2);
void MotorSpeedControl(void);

#endif
