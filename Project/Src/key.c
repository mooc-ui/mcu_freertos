/*******************************************************************************
 **Company       : BURNON TECHNOLOGY LTD.
 **Project Name	 : HC32F030 + IMMC101T Control
 **File:         : key.c
 **Created by  	 : YangWQ
 **Created Date	 : August 1, 2020, 1:19 PM
 **Version:      : v0.1
 **Target  		   : HC32F030E8PA
 **HardWare		   : 
 **SofeWare      : keil uVition5.17.0.0
 ******************************************************************************/
 
#include "key.h"
#include "lib_gpio.h"
#include "motor_ctrl.h"
#include "user.h"
#include "string.h"

volatile uint8_t Key_New=0;
volatile uint8_t Key_Old=0;
volatile uint16_t Key_Cnt=0;
volatile uint8_t Key_Flay=0;
volatile uint8_t Key_LongFlay=0;
volatile uint8_t Key_TempValue=0;
volatile uint8_t enter_sleep_flag = 0;

extern uint8_t run_flag;

extern  void ResetTask0(void);
extern  void Delay_mS(unsigned int ms);

extern  void mode_pre_handle(void);
extern void PMU_init(void);

extern __IO uint32_t lcd_data_offset;

extern void roll_brush_pwm_init(void);

/*****************************************************************************
 * Function:      uint8_t GetKey(void)
 * Description:
 *
 * Caveats:  Distinguish between long press and short press
 *           short press: bit7 = 0 、 long press: bit7 = 1
 *****************************************************************************/
uint8_t GetKey(void)
{
		uint8_t key_value;

		key_value=0;
		Key_New=0;

		if(GPIOA_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0) { //IOA4 开关机键
				Key_New |=0x01;
		}
		
		if(GPIOA_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0) { //IOA5 模式键
				Key_New |=0x02;
		}	

		if(GPIOA_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0) { //IOA6 自动清洁消毒模式
				Key_New |=0x04;
		}	
		
		if(GPIOA_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0) { //IOA7 直立识别
				Key_New |=0x08;
		}				
		
		if(Key_New !=0) {
				if(Key_Flay==0) {
						if(Key_New==Key_Old) {
								Key_Cnt++;
								if(Key_Cnt >= SHORT_PRESS_TIME) {       // Short Press  10mS * 5 = 50mS
										Key_TempValue=Key_New;
								}
								
								if(Key_Cnt >= LONG_PRESS_TIME)  {       // Long Press   10mS * 80 = 0.8S
									  key_value=Key_New | 0x80;           // Set bit7
									  Key_TempValue = 0;
									  Key_Flay=1;
								}
						}
						else{
								Key_Cnt=0;
						}
				}
		}
		else{
			  if(Key_TempValue) {
					  key_value = Key_TempValue;
					  Key_TempValue=0;
				}
				Key_Flay=0;
				Key_Cnt=0;
		}
		Key_Old=Key_New;

		return key_value;
}

/*****************************************************************************
 * Function:      void key_proc(void)
 * Description:
 *
 * Caveats:
 *
 *****************************************************************************/
void key_proc(void)
{
		uint8_t  key;

		key = GetKey();
		
//		if(0x01 == key)                           //开关机键
//		{
//		}	
		
		if(0x02 == (key & 0x7f) && !sys_status.flags.upright)  //模式键,0,1分别为智能模式、地毯模式 非直立时才有效
		{		
			 if (3 == work_mode.mode)      //自清洁中，请稍后再试
		   {
					//voice_play(16);     // 
		   }
			 else
			 {
					memset(&work_mode, 0, sizeof(WORK_MODE_T));
					if (++work_mode.mode >= 2)          //0 智能模式;1 地毯模式循环切换
						work_mode.mode = 0;
			}
		}	
		
		if(0x04 == key && sys_status.flags.upright) //自清洁消毒开关键，只有直立时才有效              
		{		
				memset(&work_mode, 0, sizeof(WORK_MODE_T));
			  work_mode.mode = 3;   //自清洁消毒
		}			
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

