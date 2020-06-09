#include "main.h"
#include "motor.h"

#define TIME_INTERVAL 0.05
// 22 pulsos por volta
// engranagem 1:75
// 1650 pulsos por volta na roda
#define PPR 1650.0

volatile double motor1_speed, motor2_speed;
volatile double motor1_speed_old, motor2_speed_old;
volatile double motor1_position, motor2_position;
volatile double motor1_integration, motor2_integration;
volatile double motor1_ref, motor2_ref;
volatile double motor1_error, motor2_error;
volatile double motor1_voltage, motor2_voltage;
volatile double cnt1, cnt2;
volatile double P = 0 /*12*/, I = 3/*35*//*12 sem sobresinal*/, D = 0;
double Kv = 12.0 / 180.0; // 12V sao 180 RPM na roda

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

void Motor1SetVoltage(double voltage) {
	if (voltage > 12)
		voltage = 12;
	if (voltage < -12)
		voltage = -12;
	motor1_voltage = voltage;
}

void Motor2SetVoltage(double voltage) {
	if (voltage > 12)
		voltage = 12;
	if (voltage < -12)
		voltage = -12;
	motor2_voltage = voltage;
}

void Motor1SetReference(double ref) {
	if (ref > MOTOR_MAX_SPEED)
		ref = MOTOR_MAX_SPEED;
	if (ref < -MOTOR_MAX_SPEED)
		ref = -MOTOR_MAX_SPEED;
	motor1_ref = ref;
}

void Motor2SetReference(double ref) {
	if (ref > MOTOR_MAX_SPEED)
		ref = MOTOR_MAX_SPEED;
	if (ref < -MOTOR_MAX_SPEED)
		ref = -MOTOR_MAX_SPEED;
	motor2_ref = ref;
}

void MotorControlSpeed(void) {
	double voltage1, voltage2;
	double deriv1 = 0, deriv2 = 0;
	cnt1 = (int16_t) TIM3->CNT;
	cnt2 = (int16_t) TIM4->CNT;
	TIM3->CNT = 0;
	TIM4->CNT = 0;
	motor1_speed = cnt1 / TIME_INTERVAL / PPR * 60.0; // RPM roda
	motor2_speed = cnt2 / TIME_INTERVAL / PPR * 60.0; // RPM roda
	motor1_position += cnt1;
	motor2_position += cnt2;

	deriv1 = (motor1_speed - motor1_speed_old) / TIME_INTERVAL;
	deriv2 = (motor2_speed - motor2_speed_old) / TIME_INTERVAL;

	motor1_speed_old = motor1_speed;
	motor2_speed_old = motor2_speed;

	// controle PID Motor1
	motor1_error = motor1_ref - motor1_speed;
	motor1_integration += motor1_error * TIME_INTERVAL;
	voltage1 = ((motor1_integration * I) + (motor1_error * P) + (deriv1 * D))
			* Kv;

	// controle PID Motor2
	motor2_error = motor2_ref - motor2_speed;
	motor2_integration += motor2_error * TIME_INTERVAL;
	voltage2 = ((motor2_integration * I) + (motor2_error * P) + (deriv2 * D))
			* Kv;

	// Set Voltage
	if ((motor1_ref == 0) && (motor1_speed == 0)) {
		motor1_position = 0;
		motor1_integration = 0;
		Motor1SetVoltage(0);
	} else {
		Motor1SetVoltage(voltage1);
	}

	if ((motor2_ref == 0) && (motor2_speed == 0)) {
		motor2_position = 0;
		motor2_integration = 0;
		Motor2SetVoltage(0);
	} else {
		Motor2SetVoltage(voltage2);
	}

}

void MotorControlVoltage(void) {
	// Check batery voltage and apply to motors
	// 65535*4096/19,8 = 13557139,39
	double pwm, correction;
	if (BATERY_VOLTAGE) {
		correction = 13557139.39 / ((double) BATERY_VOLTAGE);
	} else {
		correction = 65535.0 / 16.8; // Tensao maxima da bateria eh 16.8V
	}
	pwm = motor1_voltage * correction;
	Motor1SetPWM((int) pwm);
	pwm = motor2_voltage * correction;
	Motor2SetPWM((int) pwm);
}

