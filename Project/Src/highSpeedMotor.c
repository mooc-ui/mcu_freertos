/*******************************************************************************
 **Company       : xiaoqiao TECHNOLOGY LTD.
 **Project Name	 : C9
 **File:         : highSpeedMotor.c
 **Created by  	 : tianwanjie
 **Created Date	 : 2022.02.28
 **Version:      : 
 **Target  		   : 
 **HardWare		   : 
 **SofeWare      : 
 ******************************************************************************/

#include "highSpeedMotor.h"
#include <stdbool.h>
#include "target_isr.h"

static motorStatus_t m_motorSatus = MOTOR_STOP_WORK;

void highSpeedMotorInit(void)
{
 //测试高速电机的pwm波 测R19电阻
  PWM_BaseInitType PWM_BaseInitStruct;
  PWM_OCInitType PWM_OCInitStruct;
  
  /* PWM0 base initialization 
       - Count mode   : CONTINUOUS mode
       - Clock source : PCLK 13M   理论值主频32.768MHz，                                    实际只能跑26MHz以下
       - Clock Divide : divide by 16     */
  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_2;
  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;//PWM_CTL_TESL_APBDIV128;//PWM_CLKSRC_APB;//  频率是16khz
  PWM_BaseInitStruct.Mode = PWM_MODE_UPCOUNT; //参考手册208关于计数方式的描述
  PWM_BaseInit(PWM3, &PWM_BaseInitStruct);

  /** PWM0 channel 1 Initialize:
       - Mode         : compare mode
       - Out Mode     : TOGGLE_RESET
       - Out line     : PWM1 output (IOB6)
       Output: 12.5Hz 
       High/Low = (0x8000-0x2000)/(0xFFFF-0x6000) = 3/5
    */
  PWM_OLineConfig(PWM3_OUT1, PWM_OLINE_1);	
  PWM_OCInitStruct.OutMode = PWM_OUTMOD_RESET_SET;//参考手册page 208 picture20-5
  //PWM_OCInitStruct.Period = 256;     //config CCR0
  PWM_OC1Init(PWM3, &PWM_OCInitStruct);

	PWM3->CCR0 = MOTOR_PERIOD_SETTING_VALUE;//512;//1024;                 //由ClockSource和ClockDivision一起决定   频率8K
	PWM3->CCR1 = 128;//256;//256;                  //duty=256/1024
  
  PWM_ClearCounter(PWM3);
  /* Enable PWM0 channel 0/1/2 output */
  //PWM_OutputCmd(PWM3, PWM_CHANNEL_0, ENABLE);  //pwm3无波形输出
  PWM_OutputCmd(PWM3, PWM_CHANNEL_1, ENABLE);
//  PWM_OutputCmd(PWM3, PWM_CHANNEL_2, ENABLE);

}


/*
*占空比的值必须比128大，否则电机不会转
*/
void highSpeedMotorDutySet(uint32_t dutyVaule)
{
		if(dutyVaule > MOTOR_PERIOD_SETTING_VALUE){
				dutyVaule = 1024;
				return;
		}
		PWM3->CCR1 = dutyVaule; //duty=256/1024
		
		PWM_ClearCounter(PWM3);
		PWM_OutputCmd(PWM3, PWM_CHANNEL_1, ENABLE);
		
		m_motorSatus = MOTOR_IS_MODIFIING_ROTARY_SPEED;
}

void highSpeedMotorOpen(void)
{
		PWM_OutputCmd(PWM3, PWM_CHANNEL_1, ENABLE);
		m_motorSatus = MOTOR_IS_NORMAL_WORK;
}

void highSpeedMotorClose(void)
{
		PWM_OutputCmd(PWM3, PWM_CHANNEL_1, DISABLE);
		m_motorSatus = MOTOR_STOP_WORK;
}

/*
三种状态：
1.电机停止转动
2.电机占空比修改
3.电机稳定转动
*/
motorStatus_t getHighSpeedMotorStatus(void)
{
		return m_motorSatus;
}

void setHighSpeedMotorStatus(motorStatus_t newMotorStatus)
{
		m_motorSatus = newMotorStatus;
}

float measureMotorRotateSpeed(void)
{
	float motorRotateCompute = 0.0f;
	
	if(0 == motorRotateCompute){
			return 0;
	}else{
			motorRotateCompute = 1000.0f/(getMotorRotateSpeedMeasureRawData()*1.0f)*60.0f;
			return motorRotateCompute;
	}
}


void testHighSpeedMotor(void)
{
	extern bool sw1KeyFlag;
	extern bool sw2KeyFlag;	
	
	static bool isOpenHighMotor = false;
	static bool isModifyHighMotorDuty = false;
	static uint32_t highMotorDutyInitValue = 256;	
	
	highSpeedMotorInit();
	while(1){
			if(sw1KeyFlag){
					printf("sw1 \n");
					//isModifyHighMotorDuty = 1;
					//if(isModifyHighMotorDuty){
							printf("start modify high motor duty\n");
							if(highMotorDutyInitValue>=768){
									highMotorDutyInitValue = 256;
							}
							highMotorDutyInitValue += 256;
							isModifyHighMotorDuty = 0;
							highSpeedMotorDutySet(highMotorDutyInitValue);
					//}

					sw1KeyFlag = 0;
			}
			if(sw2KeyFlag){
					printf("sw2 \n");
					isOpenHighMotor = 1 - isOpenHighMotor;
					if(isOpenHighMotor){
							printf("open \n");
							highSpeedMotorOpen();
					}else{
							printf("close \n");
							highSpeedMotorClose();
					}
					sw2KeyFlag = 0;
			}
	}
}
