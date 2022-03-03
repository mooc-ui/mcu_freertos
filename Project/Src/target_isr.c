/**
  * @file    target_isr.c
  * @author  Application Team
  * @version V4.3.0
  * @date    2018-09-04
  * @brief   Main Interrupt Service Routines.
******************************************************************************/

#include "target_isr.h"
#include "user.h"
#include "main.h"
#include "motor_ctrl.h"

#include <stdbool.h>
#include "highSpeedMotor.h"
#include "hardwareTimer.h"

#define AUDIO_DATA_START_INDEX    44
#define UART_SEND_SIZE            4

volatile uint8_t pwm2_cnt = 0;
volatile uint8_t pwm0_cnt = 0;
volatile uint8_t pwm1_cnt = 0;
volatile uint8_t motor_pwm_cnt = 0;
volatile uint16_t pwm1_overtime = 0;
volatile uint16_t pwm2_overtime = 0;
volatile uint8_t wake_up_flag = 0;
uint8_t systick_cnt = 0;
volatile uint8_t brush_type = 0xff;
volatile uint8_t last_brush_type = 0;

extern uint16_t pwd_value;
extern uint32_t data_cnt;
extern volatile uint8_t enter_sleep_flag;
extern uint16_t  TaskTimer[TASK_NUM_MAX];
extern volatile uint8_t run_flag;
extern volatile uint8_t no_water_flag;
extern volatile uint8_t charge_flag;
extern volatile uint8_t low_bat_flag;
extern volatile uint8_t charge_insert_flag;
extern volatile uint8_t charging;
extern volatile uint8_t charge_over_flag;


static uint32_t highSpeedMotorPulseTotalTime = 0;
static uint32_t highSpeedMotorPulseCount = 0;

#define BUTTON_LONG_PRESS_TIME_INTERVAL 1000 //uint=1s


static uint32_t judgeButton1LongPressTimeInterval = BUTTON_LONG_PRESS_TIME_INTERVAL;
static uint32_t judgeButton2LongPressTimeInterval = BUTTON_LONG_PRESS_TIME_INTERVAL;
static bool judgeButton1IsLongPress = false;
static bool judgeButton2IsLongPress = false;

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
  while(1)
  {}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  while(1)
  {}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

//#define T4 *((volatile unsigned long *)((((uint32_t) IOC_DAT) & 0xF0000000)+0x2000000+((((uint32_t)IOC_DAT) &0xFFFFF)<<5)+(11<<2)))  //IOC11 T4

#include <stdbool.h>
extern void testGPIOHigh(void);
extern void testGPIOLow(void);

static uint16_t isOneSecondComing = 1000;
static uint32_t systemTimeSecond = 0;

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//		uint8_t index;

//		isOneSecondComing--;
//		if(0 == isOneSecondComing){
//				isOneSecondComing = 1000;//reload value
//				systemTimeSecond++;
//		}
//		
//    
//    for (index = 0; index < TASK_NUM_MAX; index++) {
//        if (TaskTimer[index] > 0) {
//            TaskTimer[index]--;
//        }
//    }
//}

uint32_t getSysTime(void)
{
		return systemTimeSecond;
}

volatile uint8_t pmu_cnt = 0;

/**
  * @brief  This function handles PMU interrupt request.
  * @param  None
  * @retval None
  */

extern void timer0Start(void);
extern void timer0Stop(void);

bool sw1KeyFlag = 0;
bool sw2KeyFlag = 0;
void PMU_IRQHandler(void)
{
	
#if 1	//test GPIOA's KEY function
	
	if (PMU_GetIOAINTStatus(GPIO_Pin_5))
  {
    PMU_ClearIOAINTStatus(GPIO_Pin_5);
		sw2KeyFlag = 1;
		judgeButton1IsLongPress = true;
    printf("PMU, IOA5 EXIT\r\n");
  }	
	
	if (PMU_GetIOAINTStatus(GPIO_Pin_6))
  {
    PMU_ClearIOAINTStatus(GPIO_Pin_6);
		sw1KeyFlag = 1;
		judgeButton2IsLongPress = true;
    printf("PMU, IOA6 EXIT\r\n");
  }
		
	
	if (PMU_GetIOAINTStatus(GPIO_Pin_7))
  {
    PMU_ClearIOAINTStatus(GPIO_Pin_7);
    printf("machine is standing\r\n");
  }		
	
  if (PMU_GetIOAINTStatus(GPIO_Pin_3))  //计算电机的转速，恒功率情况下：进风口堵得越多，负载越小，电机转速越高   
  {
    PMU_ClearIOAINTStatus(GPIO_Pin_3);  //休眠时，按键中断唤醒mcu
		
		
		if(MOTOR_IS_MODIFIING_ROTARY_SPEED == getHighSpeedMotorStatus() || MOTOR_STOP_WORK == getHighSpeedMotorStatus()){//judge motor working status
				setHighSpeedMotorStatus(MOTOR_IS_NORMAL_WORK);
				highSpeedMotorPulseCount = 0;
				timer0Start();
				timer0Stop();
		}
		
		if(0 == highSpeedMotorPulseCount){
				highSpeedMotorPulseTotalTime = 0;//clear
				timer0Start();
		}
		highSpeedMotorPulseCount++;
		if(highSpeedMotorPulseCount>HIGH_SPEED_MOTOR_MEASURE_TOTAL_COUNT){
				highSpeedMotorPulseCount = 0;
				timer0Stop();//
		}
		//GPIOBToF_WriteBit(GPIOC, GPIO_Pin_9, !GPIOBToF_ReadOutputDataBit(GPIOC, GPIO_Pin_9));//test for measure calibration 
	}		
	
	
#endif
	
#if 0	
	
  if (PMU_GetIOAINTStatus(GPIO_Pin_4) || PMU_GetIOAINTStatus(GPIO_Pin_6))
  {
    PMU_ClearIOAINTStatus(GPIO_Pin_4 | GPIO_Pin_6);  //休眠时，按键中断唤醒mcu
		//PMU_INTConfig(PMU_INT_IOAEN, DISABLE);  //20211027 wwb mask
		
		wake_up_flag = 1;
		
		//GPIOBToF_SetBits(GPIOF, GPIO_Pin_0); //0916 for test

//		pmu_cnt++;
//		if (pmu_cnt % 2 != 0)
//			GPIOBToF_SetBits(GPIOF, GPIO_Pin_0);
//		else
//			GPIOBToF_ResetBits(GPIOF, GPIO_Pin_0);		
		
		if (enter_sleep_flag)
		{
			//CORTEX_NVIC_SystemReset();          //复位系统
		}			
  }
	
  if (PMU_GetIOAINTStatus(GPIO_Pin_8))  //充电唤醒
  {
    PMU_ClearIOAINTStatus(GPIO_Pin_8);  //休眠时，充电中断唤醒mcu
		
		//PMU_INTConfig(PMU_INT_IOAEN, DISABLE);
		
		if (!run_flag)            //20211028 wwb add 关机时才允许充电
		{
			charge_insert_flag = 1; //20211027 wwb mask
		  charge_over_flag = 0;   //重新插入底座时允许充电
		}
  }	
	
  if (PMU_GetIOAINTStatus(GPIO_Pin_3))  //计算电机的转速，恒功率情况下：进风口堵得越多，负载越小，电机转速越高   
  {
    PMU_ClearIOAINTStatus(GPIO_Pin_3);  //休眠时，按键中断唤醒mcu
		//计算500个下降沿
		wsx_full_check.motor_speed_int_cnt++;
		if (wsx_full_check.motor_speed_int_cnt > 500)     
		{
			wsx_full_check.motor_speed_int_cnt = 0;
			wsx_full_check.motor_speed = 1000 * 500 / wsx_full_check.motor_speed_timer_cnt;  //HZ
			wsx_full_check.motor_speed *= 60;  //换算为rpm
			wsx_full_check.motor_speed_timer_cnt = 0;
			
//			if ((3 == work_mode && motor_speed >= 0xCF00)   //不同的工作模式，转速不一样？50%占空比测试可以正确检测污水箱是否满
//			 || (3 != work_mode && motor_speed >= 0xD000))  //0xD800 污水箱满
			//if (motor_speed >= 0xD900)  //60%占空比 0xD900 污水箱满，测试数值可到0xD9BC; 0xDA70
			if (wsx_full_check.motor_speed >= 0xCD00)		//50%占空比 污水箱满		
			{
				wsx_full_check.full_cnt++;
			}
			
			if (++wsx_full_check.check_cnt >= 30)
			{			
        wsx_full_check.check_cnt = 0;				
				if (wsx_full_check.full_cnt >= 6)
					sys_faults.flags.wsx_full = TRUE;
				else
					sys_faults.flags.wsx_full = FALSE;
				
				wsx_full_check.full_cnt = 0;
			}
		}		
  }	
#endif	
	
}

uint32_t getMotorRotateSpeedMeasureRawData(void)
{
		return highSpeedMotorPulseTotalTime;
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
}

/**
  * @brief  This function handles U32K0 interrupt request.
  * @param  None
  * @retval None
  */
void U32K0_IRQHandler(void)
{
}

/**
  * @brief  This function handles U32K1 interrupt request.
  * @param  None
  * @retval None
  */
void U32K1_IRQHandler(void)
{
}

/**
  * @brief  This function handles I2C interrupt request.
  * @param  None
  * @retval None
  */
void I2C_IRQHandler(void)
{
}

/**
  * @brief  This function handles ISO78160 interrupt request.
  * @param  None
  * @retval None
  */
void ISO78160_IRQHandler(void)
{
}

/**
  * @brief  This function handles ISO78161 interrupt request.
  * @param  None
  * @retval None
  */
void ISO78161_IRQHandler(void)
{
}

/**
  * @brief  This function handles TMR0 interrupt request.
  * @param  None
  * @retval None
  */
//uint8_t systick_cnt = 0;
volatile uint8_t audio_test_over = 0;
volatile uint16_t audio_ff_cnt = 0;
//void TMR0_IRQHandler(void)
//{
//	if (TMR_GetINTStatus(TMR0))
//	{
//		TMR_ClearINTStatus(TMR0);
//		
////		if (0 == systick_cnt++ % 2)
////				GPIOBToF_SetBits(GPIOB, GPIO_Pin_10);
////		else
////				GPIOBToF_ResetBits(GPIOB, GPIO_Pin_10);	

//		if (44 == data_cnt && audio_ff_cnt++ <= 256)
//		{
//				GPIOBToF_SetBits(GPIOE, GPIO_Pin_4);
//				PWM_SetData(PWM3, 0xff);
//		}
//		
//		if (0 == audio_test_over && audio_ff_cnt >= 256)
//		{
//			//修改语音pwm占空比
//			//if (data_cnt < 8420)
//			if (data_cnt < 16796)				
//			{
//				GPIOBToF_SetBits(GPIOE, GPIO_Pin_4);

//				PWM_SetData(PWM3, audio_data[data_cnt]);
//				//PWM_SetData(PWM3, 64);
//				
//				//PWM_ClearCounter(PWM3);
//				/* Enable PWM3 channel 0/1/2 output */
//				PWM_OutputCmd(PWM3, PWM_CHANNEL_1, ENABLE);
//				data_cnt++;
//			}
//			else 
//			{
//				audio_test_over = 1;
//				audio_ff_cnt = 0;
//				data_cnt = 44;
//				GPIOBToF_ResetBits(GPIOE, GPIO_Pin_4);
//				//PWM_ClearCounter(PWM3);
//				/* Disable PWM3 channel 0/1/2 output */
//				PWM_OutputCmd(PWM3, PWM_CHANNEL_1, DISABLE);
//			}
//	 }
//	}		
//}

void TMR0_IRQHandler(void)
{
	
  if (TMR_GetINTStatus(TMR0))
  {
		highSpeedMotorPulseTotalTime++;
    TMR_ClearINTStatus(TMR0);
    /* Toggle IOB7 */
    //GPIOBToF_WriteBit(GPIOC, GPIO_Pin_9, !GPIOBToF_ReadOutputDataBit(GPIOC, GPIO_Pin_9));//test pin togger
  }	
	
	
//	if (TMR_GetINTStatus(TMR0))
//	{
//		TMR_ClearINTStatus(TMR0);
//		
////		if (0 == systick_cnt++ % 2)
////				GPIOBToF_SetBits(GPIOB, GPIO_Pin_10);
////		else
////				GPIOBToF_ResetBits(GPIOB, GPIO_Pin_10);	
//		
//		if (0 == audio_test_over)
//		{
//			//修改语音pwm占空比
//			//if (data_cnt < 8420)
//			//if (data_cnt < 16796)
//			if (data_cnt < 33548)
//			{
//				if (44 == data_cnt)
//					GPIOBToF_SetBits(GPIOE, GPIO_Pin_4);

//				PWM_SetData(PWM3, (audio_data[data_cnt + 1]<<8) | audio_data[data_cnt]);
//				//PWM_SetData(PWM3, 64);
//				
//				//PWM_ClearCounter(PWM3);
//				/* Enable PWM3 channel 0/1/2 output */
//				if (44 == data_cnt)
//						PWM_OutputCmd(PWM3, PWM_CHANNEL_1, ENABLE);
//				data_cnt += 2;
//			}
//			else 
//			{
//				audio_test_over = 1;
//				audio_ff_cnt = 0;
//				data_cnt = 44;
//				GPIOBToF_ResetBits(GPIOE, GPIO_Pin_4);
//				//PWM_ClearCounter(PWM3);
//				/* Disable PWM3 channel 0/1/2 output */
//				PWM_OutputCmd(PWM3, PWM_CHANNEL_1, DISABLE);
//			}
//	 }
//	}		
}

/**
  * @brief  This function handles TMR1 interrupt request.
  * @param  None
  * @retval None
  */
void TMR1_IRQHandler(void)
{
  if (TMR_GetINTStatus(TMR1))
  {
    TMR_ClearINTStatus(TMR1);
    
		//button1 longPress/shortPress
		if(judgeButton1IsLongPress){
				judgeButton1LongPressTimeInterval--;
			
				if(0 == judgeButton1LongPressTimeInterval){
						if(0 == GPIOA_ReadInputDataBit(GPIOA,GPIO_Pin_5)){
								//key is long press
								//GPIOBToF_WriteBit(GPIOC, GPIO_Pin_9, !GPIOBToF_ReadOutputDataBit(GPIOC, GPIO_Pin_9));//test pin togger
						}else{
								//short press
						}
						judgeButton1IsLongPress = false;
						judgeButton1LongPressTimeInterval = BUTTON_LONG_PRESS_TIME_INTERVAL;					
				}			
		}//end	
		
		//button2 longPress/shortPress
		if(judgeButton2IsLongPress){
				judgeButton2LongPressTimeInterval--;
			
				if(0 == judgeButton2LongPressTimeInterval){
						if(0 == GPIOA_ReadInputDataBit(GPIOA,GPIO_Pin_6)){
								//key is long press
								GPIOBToF_WriteBit(GPIOC, GPIO_Pin_9, !GPIOBToF_ReadOutputDataBit(GPIOC, GPIO_Pin_9));//test pin togger
						}else{
								//short press
						}
						judgeButton2IsLongPress = false;
						judgeButton2LongPressTimeInterval = BUTTON_LONG_PRESS_TIME_INTERVAL;					
				}			
		}//end
		
  }			
}

/**
  * @brief  This function handles TMR2 interrupt request.
  * @param  None
  * @retval None
  */
void TMR2_IRQHandler(void)
{
	
  if (TMR_GetINTStatus(TMR1))
  {
    TMR_ClearINTStatus(TMR1);
  }		
	
}

/**
  * @brief  This function handles TMR3 interrupt request.
  * @param  None
  * @retval None
  */
void TMR3_IRQHandler(void)
{
	
  if (TMR_GetINTStatus(TMR3))
  {
    TMR_ClearINTStatus(TMR3);
  }	
	
}

/**
  * @brief  This function handles PWM0 interrupt request.
  * @param  None
  * @retval None
  */
void PWM0_IRQHandler(void)
{
}

/**
  * @brief  This function handles PWM1 interrupt request.
  * @param  None
  * @retval None
  */
void PWM1_IRQHandler(void)
{
}

/**
  * @brief  This function handles PWM2 interrupt request.
  * @param  None
  * @retval None
  */
void PWM2_IRQHandler(void)
{
}

/**
  * @brief  This function handles PWM3 interrupt request.
  * @param  None
  * @retval None
  */
//uint8_t systick_cnt = 0;
void PWM3_IRQHandler(void)
{		
//		if (0 == systick_cnt++ % 2)
//				GPIOBToF_SetBits(GPIOB, GPIO_Pin_10);
//		else
//				GPIOBToF_ResetBits(GPIOB, GPIO_Pin_10);	
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void DMA_IRQHandler(void)
{
}

/**
  * @brief  This function handles FLASH interrupt request.
  * @param  None
  * @retval None
  */
void FLASH_IRQHandler(void)
{
}

/**
  * @brief  This function handles ANA interrupt request.
  * @param  None
  * @retval None
  */
//void ANA_IRQHandler(void)
//{
//}

#define	SPI1_CS_1 		GPIOBToF_SetBits(GPIOB, GPIO_Pin_9)  		
#define	SPI1_CS_0 		GPIOBToF_ResetBits(GPIOB, GPIO_Pin_9)       //CS
__IO uint8_t lcd_send_cnt = 0;
__IO uint8_t lcd_send_data[2] = {0};
volatile uint8_t spi1_tx_dis = 1;
/**
  * @brief  This function handles SPI1 interrupt request.
  * @param  None
  * @retval None
  */
volatile uint8_t photo_data_read_step = 2;
void SPI1_IRQHandler(void)  //lcd屏显示使用
{
//  if (SPI_GetStatus(SPI1, SPI_STS_TXIF))
//  {
//    //if (spi1_send_cnt < spi1_send_buf_size && spi1_send_cnt < spi_flash_read_len)
//		if (spi1_send_cnt < spi1_send_buf_size)
//    {
//			if (0 == lcd_send_cnt)
//			{				
//				uint16_t temp_data = 0;
//				uint8_t disp_data = 0;
//				//W25QXX_Read(&disp_data, lcd_data_offset + spi1_send_cnt, 1);
//				if (0 == spi1_send_cnt % 4096)
//				{
//					W25QXX_Read(temp_disp_data, lcd_data_offset + spi1_send_cnt, 4096);
//				}
//				
//				if (0 == lcd_data_offset)
//					disp_data = temp_disp_data[spi1_send_cnt%4096];
//				else
//					disp_data = Photo_Data[spi1_send_cnt];
//////				//disp_data = spi_flash_read_buf[spi1_send_cnt];
////				disp_data = temp_disp_data[spi1_send_4096_cnt];

//				//readRingbuffer(&disp_data, 1);
//				//spi1_send_4096_cnt++;

//				spi1_send_cnt++;
//				temp_data = disp_data;
//				temp_data = temp_data<<7;
//				temp_data |= 0x8000; 
//				lcd_send_data[0] = (temp_data&0xff00)>>8;
//				lcd_send_data[1] = (temp_data&0x00ff);		
//				//delay_us(1);
//				SPI1_CS_0;
//				
//			}						
//      SPI_SendData(SPI1, lcd_send_data[lcd_send_cnt]);
//			if (lcd_send_cnt > 0)
//			{
//				lcd_send_cnt = 0;
//				SPI1_CS_1;
//			}
//			else
//			{
//				lcd_send_cnt++;
//								
////				if (spi1_send_4096_cnt >= 4096)
////				{
////					photo_data_read_step = 2;
////					spi1_send_4096_cnt = 0;
////				}
//			}
//    }
//		
//		if (spi1_send_cnt == spi1_send_buf_size)
//    {
//      //SPI_INTConfig(SPI1, SPI_INT_TX, DISABLE);
//			spi1_tx_dis = 1;
//			spi1_send_cnt = 0;
//			lcd_send_cnt = 0;
//    }		
//			
//    SPI_ClearStatus(SPI1, SPI_STS_TXIF);
//  }
	
	//20210714 wwb mask
//  if (SPI_GetStatus(SPI1, SPI_STS_RXIF))
//  {
//    if (spi1_recv_cnt < spi1_recv_buf_size)
//    {
//      spi1_recv_buf[spi1_recv_cnt++] = SPI_ReceiveData(SPI1);
//    }
//    if (spi1_recv_cnt == spi1_recv_buf_size)
//    {
//      SPI_INTConfig(SPI1, SPI_INT_RX, DISABLE);
//    }
//    SPI_ClearStatus(SPI1, SPI_STS_RXIF);
//  }
}

/**
  * @brief  This function handles SPI2 interrupt request.
  * @param  None
  * @retval None
  */
void SPI2_IRQHandler(void)    //读取lcd显示图片
{
//  if (SPI_GetStatus(SPI2, SPI_STS_TXIF))
//  {
//    if (spi1_send_cnt < spi2_send_buf_size)
//    {
//			if (0 == lcd_send_cnt)
//			{				
//				uint16_t temp_data = 0;
//				uint8_t disp_data = 0;

//				W25QXX_Read(&disp_data, lcd_data_offset + spi1_send_cnt, 1);

//				//readRingbuffer(&disp_data, 1);
//				spi1_send_cnt++;
//				temp_data = disp_data;
//				temp_data = temp_data<<7;
//				temp_data |= 0x8000; 
//				lcd_send_data[0] = (temp_data&0xff00)>>8;
//				lcd_send_data[1] = (temp_data&0x00ff);		
//				//delay_us(1);
//				SPI1_CS_0;
//			}						
//      SPI_SendData(SPI1, lcd_send_data[lcd_send_cnt]);
//			if (lcd_send_cnt > 0)
//			{
//				lcd_send_cnt = 0;
//				SPI1_CS_1;
//			}
//			else
//				lcd_send_cnt++;
//    }
//    if (spi1_send_cnt == spi1_send_buf_size)
//    {
//      SPI_INTConfig(SPI1, SPI_INT_TX, DISABLE);
//    }
//    SPI_ClearStatus(SPI1, SPI_STS_TXIF);
//  }
//  if (SPI_GetStatus(SPI1, SPI_STS_RXIF))
//  {
//    if (spi1_recv_cnt < spi1_recv_buf_size)
//    {
//      spi1_recv_buf[spi1_recv_cnt++] = SPI_ReceiveData(SPI1);
//    }
//    if (spi1_recv_cnt == spi1_recv_buf_size)
//    {
//      SPI_INTConfig(SPI1, SPI_INT_RX, DISABLE);
//    }
//    SPI_ClearStatus(SPI1, SPI_STS_RXIF);
//  }	
}

/*********************************** END OF FILE ******************************/
