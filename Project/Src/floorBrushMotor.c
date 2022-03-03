/*******************************************************************************
 **Company       : xiaoqiao TECHNOLOGY LTD.
 **Project Name	 : C9
 **File:         : floorBrushmotor.c
 **Created by  	 : tianwanjie
 **Created Date	 : 2022.02.28
 **Version:      : 
 **Target  		   : 
 **HardWare		   : 
 **SofeWare      : 
 ******************************************************************************/

#include "floorBrushMotor.h"
#include <stdbool.h>

void floorBrushMotorInit(void)
{
	
  PWM_BaseInitType PWM_BaseInitStruct;
  PWM_OCInitType PWM_OCInitStruct;

  /* PWM0 base initialization 
       - Count mode   : CONTINUOUS mode
       - Clock source : PCLK 13M
       - Clock Divide : divide by 16     */
  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_2;   //125Hz. PWM_CLKDIV_4 20220104 modify
  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;//250HZ  和pwm3使用的是一个时钟源，但是分频出来的时钟却不一样，原因未知
  PWM_BaseInitStruct.Mode = PWM_MODE_CONTINUOUS;//PWM mode
  PWM_BaseInit(PWM0, &PWM_BaseInitStruct);
	
	PWM0->CCR0 = 0;   //默认值是0
	
	PWM_OLineConfig(PWM0_OUT2, PWM_OLINE_2);
  PWM_OCInitStruct.OutMode = PWM_OUTMOD_TOGGLE_SET;  //PWM_OUTMOD_TOGGLE_RESET
  PWM_OCInitStruct.Period = 0xffff/8-1;   //0x4000;
  PWM_OC2Init(PWM0, &PWM_OCInitStruct);   
	
  
  PWM_ClearCounter(PWM0);
  /* Enable PWM0 channel 0/1/2 output */
  //PWM_OutputCmd(PWM0, PWM_CHANNEL_0, DISABLE);
	//PWM_OutputCmd(PWM0, PWM_CHANNEL_1, DISABLE);
  PWM_OutputCmd(PWM0, PWM_CHANNEL_2, ENABLE);	
}

void startfloorBrushMotor(void)
{
		PWM_OutputCmd(PWM0, PWM_CHANNEL_2, ENABLE);	
}

void stopfloorBrushMotor(void)
{
		PWM_OutputCmd(PWM0, PWM_CHANNEL_2, DISABLE);
}

void setfloorBrushMotorDuty(uint32_t dutyValue)
{
	PWM_OCInitType PWM_OCInitStruct;
	
	PWM0->CCR0 = 0;   //默认值是0
	
	PWM_OLineConfig(PWM0_OUT2, PWM_OLINE_2);
  PWM_OCInitStruct.OutMode = PWM_OUTMOD_TOGGLE_SET;  //PWM_OUTMOD_TOGGLE_RESET
  PWM_OCInitStruct.Period = dutyValue;//0xffff/8-1;   //0x4000;
  PWM_OC2Init(PWM0, &PWM_OCInitStruct);   
}

void testfloorBrushMotor(void)
{
	extern bool sw1KeyFlag;
	extern bool sw2KeyFlag;	
	
	static bool isOpenHighMotor = false;
	static bool isModifyHighMotorDuty = false;
	static uint32_t highMotorDutyInitValue = 256;	
	
	floorBrushMotorInit();
	while(1){
			if(sw1KeyFlag){
					printf("sw1 \n");
					//isModifyHighMotorDuty = 1;
					//if(isModifyHighMotorDuty){
							printf("start modify motor duty\n");
							if(highMotorDutyInitValue>=0xffff/8*7){
									highMotorDutyInitValue = 0xffff/8;
							}
							highMotorDutyInitValue += 0xffff/8;
							isModifyHighMotorDuty = 0;
							setfloorBrushMotorDuty(highMotorDutyInitValue);
					//}

					sw1KeyFlag = 0;
			}
			if(sw2KeyFlag){
					printf("sw2 \n");
					isOpenHighMotor = 1 - isOpenHighMotor;
					if(isOpenHighMotor){
							printf("motor open \n");
							startfloorBrushMotor();
					}else{
							printf("motor close \n");
							stopfloorBrushMotor();
					}
					sw2KeyFlag = 0;
			}
	}
}
