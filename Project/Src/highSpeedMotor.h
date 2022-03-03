#ifndef _HIGH_SPEED_MOTOR_H_
#define _HIGH_SPEED_MOTOR_H_
#include "main.h"
#include "string.h"


typedef enum {
	MOTOR_STOP_WORK,
	MOTOR_IS_NORMAL_WORK,
	MOTOR_IS_MODIFIING_ROTARY_SPEED,
}motorStatus_t;

/*
*MOTOR_PERIOD_SETTING_VALUE = 0       level = 1
*MOTOR_PERIOD_SETTING_VALUE = 0xffff  level = 0
*/
#define MOTOR_PERIOD_SETTING_VALUE 1024  //pwm frequence=8khz

void highSpeedMotorInit(void);
void highSpeedMotorDutySet(uint32_t dutyVaule);
void highSpeedMotorOpen(void);
void highSpeedMotorClose(void);

motorStatus_t getHighSpeedMotorStatus(void);
void setHighSpeedMotorStatus(motorStatus_t newMotorStatus);
float measureMotorRotateSpeed(void);


void testHighSpeedMotor(void);

#endif