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
 
#include "delay.h"
#include "lib_gpio.h"

__IO uint32_t global_ms, global_us;


void Delay_nAPBClock(__IO uint32_t n)
{
  uint32_t tmp;
  
  TMR3->INT = TMR_INT_INT;
  TMR3->RELOAD = n;
  TMR3->VALUE  = n;
  TMR3->CTRL = TMR_CTRL_EN;
  
  do
  {
    tmp = TMR3->INT;
  } while (!(tmp & TMR_INT_INT));
  
  TMR3->INT = TMR_INT_INT;
  TMR3->CTRL = 0;
}

void delay_init(void)
{
  uint32_t PCLK_Freq;
  PCLK_Freq = CLK_GetPCLKFreq();
  global_ms = PCLK_Freq/1000;
  global_us = global_ms/1000;
}
void delay_us(uint32_t us)
{
  __IO uint32_t n;
  n = global_us*us;
  Delay_nAPBClock(n);
}
void delay_ms(uint32_t ms)
{
  __IO uint32_t n;
  n = global_ms*ms;
  Delay_nAPBClock(n);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

