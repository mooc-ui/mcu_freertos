/*******************************************************************************
 **Company       : xiaoqiao TECHNOLOGY LTD.
 **Project Name	 : C9
 **File:         : hardwareTimer.c
 **Created by  	 : tianwanjie
 **Created Date	 : 2022.03.02
 **Version:      : 
 **Target  		   : 
 **HardWare		   : 
 **SofeWare      : 
 ******************************************************************************/

#include "hardwareTimer.h"


void timer0Init(void)
{
	TMR_InitType TMR_InitStruct;
	
  TMR_DeInit(TMR0);
  TMR_InitStruct.ClockSource = TMR_CLKSRC_INTERNAL;
  TMR_InitStruct.EXTGT = TMR_EXTGT_DISABLE;
	
	/*这个13107200是是通过外部32.768k晶振通过倍频到26.2144Mhz 然后HCLK进行1分频  
	* PCLK进行二分频，其中PCLK是为TIMER外设提供时钟的,所以timer外设的时钟频率为13107200hz
	*10ms = 1/(1/13107200hz * 13107200/100)  ====> RELOAD = Period * APBCLK – 1 其中period是我们要配置的定时器周期
	*/
	//3279
  TMR_InitStruct.Period = (13117200/1000 +3100 -1)/2;//period = 1ms
	//TMR_InitStruct.Period = 1;//0xffffffff-1;//13107200/100 - 1;//2^17-1  maxValue = 0XFFFF FFFF = 2^32 period=16ms duty=50%
  TMR_Init(TMR0, &TMR_InitStruct);
  
  /* Enable Timer0 interrupt */
  TMR_INTConfig(TMR0, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(TMR0_IRQn, 0);
	
	//TMR_Cmd(TMR0, ENABLE);//start timer0
}

void timer0Start(void)
{
		TMR_Cmd(TMR0, ENABLE);
}

void timer0Stop(void)
{
		TMR_Cmd(TMR0, DISABLE);
}

void timer1Init(void)
{
  TMR_InitType TMR_InitStruct;
  
  TMR_DeInit(TMR1);
  TMR_InitStruct.ClockSource = TMR_CLKSRC_INTERNAL;
  TMR_InitStruct.EXTGT       = TMR_EXTGT_DISABLE;
  TMR_InitStruct.Period      = (13117200/1000 +3100 -1);//1ms //13107200/100 - 1;//10ms
  TMR_Init(TMR1, &TMR_InitStruct);
	
  TMR_INTConfig(TMR1, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(TMR1_IRQn, 0);	
  
  TMR_Cmd(TMR1, ENABLE);	
}

void timer1Start(void)
{
		TMR_Cmd(TMR1, ENABLE);
}

void timer1Stop(void)
{
		TMR_Cmd(TMR1, DISABLE);
}


/*some question*/
//void timer2Init(void)
//{
//  TMR_InitType TMR_InitStruct;
//  
//  TMR_DeInit(TMR2);
//  TMR_InitStruct.ClockSource = TMR_CLKSRC_INTERNAL;
//  TMR_InitStruct.EXTGT       = TMR_EXTGT_DISABLE;
//  TMR_InitStruct.Period      = 13107200/100 - 1;//10ms
//  TMR_Init(TMR2, &TMR_InitStruct);
//	
//  TMR_INTConfig(TMR2, ENABLE);
//  CORTEX_SetPriority_ClearPending_EnableIRQ(TMR2_IRQn, 0);	
//  
//  TMR_Cmd(TMR2, ENABLE);	
//}

//void timer2Start(void)
//{
//		TMR_Cmd(TMR2, ENABLE);
//}

//void timer2Stop(void)
//{
//		TMR_Cmd(TMR2, DISABLE);
//}


//void timer3Init(void)
//{
//  TMR_InitType TMR_InitStruct;
//  
//  TMR_DeInit(TMR3);
//  TMR_InitStruct.ClockSource = TMR_CLKSRC_INTERNAL;
//  TMR_InitStruct.EXTGT       = TMR_EXTGT_DISABLE;
//  TMR_InitStruct.Period      = 13107200/100 - 1;//10ms
//  TMR_Init(TMR3, &TMR_InitStruct);
//	
//  TMR_INTConfig(TMR3, ENABLE);
//  CORTEX_SetPriority_ClearPending_EnableIRQ(TMR3_IRQn, 0);	
//  
//  TMR_Cmd(TMR3, ENABLE);	
//}

//void timer3Start(void)
//{
//		TMR_Cmd(TMR3, ENABLE);
//}

//void timer3Stop(void)
//{
//		TMR_Cmd(TMR3, DISABLE);
//}


