/*******************************************************************************
 **Company       : xiaoqiao TECHNOLOGY LTD.
 **Project Name	 : C9
 **File:         : floorBrush.c
 **Created by  	 : tianwanjie
 **Created Date	 : 2022.02.28
 **Version:      : 
 **Target  		   : 
 **HardWare		   : 
 **SofeWare      : 
 ******************************************************************************/
 
#include "floorBrushPump.h"
#include <stdbool.h>

void floorBrushPumpInit(void)//15.5hz
{

  PWM_BaseInitType PWM_BaseInitStruct;
  PWM_OCInitType PWM_OCInitStruct;
  
  /* PWM0 base initialization 
       - Count mode   : CONTINUOUS mode
       - Clock source : PCLK 13M
       - Clock Divide : divide by 16     */
  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_16;
  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;//250HZ  和pwm3使用的是一个时钟源，但是分频出来的时钟却不一样，原因未知
  PWM_BaseInitStruct.Mode = PWM_MODE_UPCOUNT; //PWM_MODE_UPCOUNT; //PWM_MODE_CONTINUOUS
  PWM_BaseInit(PWM1, &PWM_BaseInitStruct);


  /** PWM0 channel 1 Initialize:
       - Mode         : compare mode
       - Out Mode     : TOGGLE_RESET
       - Out line     : PWM1 output (IOB6)
       Output: 12.5Hz 
       High/Low = (0x8000-0x2000)/(0xFFFF-0x6000) = 3/5
    */
  PWM_OLineConfig(PWM1_OUT1, PWM_OLINE_1);
  PWM_OCInitStruct.OutMode = PWM_OUTMOD_RESET_SET; //PWM_OUTMOD_TOGGLE_RESET
  //PWM_OCInitStruct.Period = 256;
  PWM_OC1Init(PWM1, &PWM_OCInitStruct);

	PWM1->CCR0 = 0xFFFF;
	PWM1->CCR1 = 0xFFFF*1/4-1;   //20%占空比
  
  PWM_ClearCounter(PWM1);
  /* Enable PWM0 channel 0/1/2 output */
  //PWM_OutputCmd(PWM1, PWM_CHANNEL_0, ENABLE);  //pwm1无波形输出
  PWM_OutputCmd(PWM1, PWM_CHANNEL_1, ENABLE);
//  PWM_OutputCmd(PWM1, PWM_CHANNEL_2, ENABLE);	
	
}


void startfloorBrushPump(void)
{
		PWM_OutputCmd(PWM1, PWM_CHANNEL_1, ENABLE);
}

void stopfloorBrushPump(void)
{
		PWM_OutputCmd(PWM1, PWM_CHANNEL_1, DISABLE);
}

void setfloorBrushPumpDuty(uint32_t dutyValue)
{
	PWM1->CCR0 = 0xFFFF;
	PWM1->CCR1 = dutyValue;//0xFFFF*1/4-1;   //20%占空比
  
  PWM_ClearCounter(PWM1);
  /* Enable PWM0 channel 0/1/2 output */

  PWM_OutputCmd(PWM1, PWM_CHANNEL_1, ENABLE);
}

void testfloorBrushPump(void)
{
	extern bool sw1KeyFlag;
	extern bool sw2KeyFlag;	
	
	static bool isOpenHighMotor = false;
	static bool isModifyHighMotorDuty = false;
	static uint32_t highMotorDutyInitValue = 256;	
	
	floorBrushPumpInit();
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
							setfloorBrushPumpDuty(highMotorDutyInitValue);
					//}

					sw1KeyFlag = 0;
			}
			if(sw2KeyFlag){
					printf("sw2 \n");
					isOpenHighMotor = 1 - isOpenHighMotor;
					if(isOpenHighMotor){
							printf("motor open \n");
							startfloorBrushPump();
					}else{
							printf("motor close \n");
							stopfloorBrushPump();
					}
					sw2KeyFlag = 0;
			}
	}
}

