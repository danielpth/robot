#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "main.h"

extern volatile double motor1_speed, motor2_speed, motor1_position,
		motor2_position, motor1_integration, motor2_integration;

extern volatile double motor1_position, motor2_position;

void MotorSetReference(double ref_left, double ref_right);
void MotorSpeedControl(void);

#endif
