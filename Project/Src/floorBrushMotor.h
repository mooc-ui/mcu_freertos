#ifndef _FLOOR_BRUSH_MOTOR_H_
#define _FLOOR_BRUSH_MOTOR_H_
#include "main.h"
#include "string.h"


void floorBrushMotorInit(void);
void setfloorBrushMotorDuty(uint32_t dutyValue);
void stopfloorBrushMotor(void);
void startfloorBrushMotor(void);


void testfloorBrushMotor(void);

#endif