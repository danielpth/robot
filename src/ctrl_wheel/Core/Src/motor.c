#include "motor.h"

volatile double motor1_speed, motor2_speed, motor1_speed_old, motor2_speed_old;
volatile double motor1_position, motor2_position;
volatile double motor1_integration, motor2_integration;
volatile double motor1_ref, motor2_ref;
volatile double motor1_error, motor2_error;
volatile double cnt1, cnt2;
volatile double P = 0.5, I = 0, D = 0;
volatile double Kv = 50;

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

void MotorSetReference(double ref1, double ref2) {
	motor1_ref = ref1;
	motor2_ref = ref2;
}

#define TIME_INTERVAL 0.05
void MotorSpeedControl(void) {
	int pwm1, pwm2, deriv1 = 0, deriv2 = 0;
	cnt1 = (int16_t)TIM3->CNT;
	cnt2 = (int16_t)TIM4->CNT;
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	//cnt1 *= motor1_dir;
	//cnt2 *= motor2_dir;
	motor1_speed = cnt1 / TIME_INTERVAL;
	motor2_speed = cnt2 / TIME_INTERVAL;
	motor1_position += cnt1;
	motor2_position += cnt2;

	deriv1 = (motor1_speed - motor1_speed_old) / TIME_INTERVAL;
	deriv2 = (motor2_speed - motor2_speed_old) / TIME_INTERVAL;

	motor1_speed_old = motor1_speed;
	motor2_speed_old = motor2_speed;

	// controle PID Motor1
	motor1_error = motor1_ref - motor1_speed;
	motor1_integration += motor1_error * TIME_INTERVAL;
	pwm1 = ((motor1_integration * I) + (motor1_error * P) + (deriv1 * D)) * Kv;

	motor1_error = motor1_ref - motor1_speed;
	motor1_integration += motor1_error * TIME_INTERVAL;
	pwm1 = ((motor1_integration * I) + (motor1_error * P) + (deriv1 * D)) * Kv;

	// controle PID Motor2
	motor2_error = motor2_ref - motor2_speed;
	motor2_integration += motor2_error;
	pwm2 = ((motor2_integration * I) + (motor2_error * P) + (deriv2 * D)) * Kv;

	motor2_error = motor2_ref - motor2_speed;
	motor2_integration += motor2_error;
	pwm2 = ((motor2_integration * I) + (motor2_error * P) + (deriv2 * D)) * Kv;

	// Set PWM
	if ((motor1_ref == 0) && (motor1_speed == 0)) {
		motor1_position = 0;
		motor1_integration = 0;
		Motor1SetPWM(0);
	} else {
		Motor1SetPWM(pwm1);
	}

	if ((motor2_ref == 0) && (motor2_speed == 0)) {
		motor2_position = 0;
		motor2_integration = 0;
		Motor2SetPWM(0);
	} else {
		Motor2SetPWM(pwm2);
	}

}

