/**
  * @file    main.c
  * @author  Application Team
  * @version V4.3.0
  * @date    2018-09-04
  * @brief   Main program body.
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "user.h"
#include "adc.h"
#include "key.h"
#include "delay.h"
#include "string.h"
#include "motor_ctrl.h"
#include "uart.h"

#include "v_stdio.h"

/* Private functions ---------------------------------------------------------*/

volatile uint8_t run_flag = 0;                  //Ĭ��Ϊ�ػ�
uint8_t mode0_step = 0;
uint8_t mode1_step = 0;

uint16_t  dirty_full_cnt = 0;
volatile uint8_t sleep_cnt = 0; 
extern uint32_t  adc_channel;

extern uint16_t  Task0_Cycle;
extern uint16_t  TaskTimer[TASK_NUM_MAX];

extern volatile uint16_t pwm1_overtime;
extern volatile uint16_t pwm2_overtime;
extern volatile uint8_t wake_up_flag;
extern volatile uint8_t brush_oc_flag;
extern volatile uint8_t pump_oc_flag;
extern volatile uint8_t brush_type;
extern volatile uint8_t upright_flag;
extern volatile uint8_t last_upright_flag;
extern volatile uint8_t no_water_flag;
extern volatile uint8_t last_no_water_flag;
extern volatile uint8_t charge_flag;
extern volatile uint8_t low_bat_flag;
extern volatile uint8_t wake_up_flag;
extern volatile uint8_t no_water_display;
extern volatile uint8_t charge_insert_flag;
extern volatile uint8_t invalid_key_flag;
extern volatile uint8_t rx_cnt;
extern volatile uint8_t rx_done_flag;
extern volatile uint8_t uninstall_flag;
extern volatile uint8_t last_uninstall_flag;
extern volatile uint8_t last_brush_type;

extern volatile uint8_t vol_display_100;
extern volatile uint8_t vol_display_80;
extern volatile uint8_t vol_display_60;
extern volatile uint8_t vol_display_40;
extern volatile uint8_t vol_display_20;

extern void ResetTask0(void);
extern void voltage_sample(void);
extern void motor_read_status_tx(void);
extern void adc_init(uint32_t Channel);

extern void delay_init(void);

extern void motor_speed_set(uint16_t duty);
extern void Task_InitTime(void);

sys_status_t sys_status;
sys_faults_t sys_faults;
sys_time_t sys_time;
motor_ctrl_t motor_ctrl[MOTOR_CTRL_NUM];
WORK_MODE_T work_mode;
IR_T ir;

/* Private functions ---------------------------------------------------------*/

///**
//  * @brief  Clock_Init:
//              - PLLL input clock    : External 32K crystal
//              - PLLL frequency      : 26M
//              - AHB Clock source    : PLLL
//              - AHB Clock frequency : 26M (PLLL divided by 1)
//              - APB Clock frequency : 13M (AHB Clock divided by 2)
//  * @param  None
//  * @retval None
//  */
//void Clock_Init(void)
//{
//  CLK_InitTypeDef CLK_Struct;

//  CLK_Struct.ClockType = CLK_TYPE_AHBSRC \
//                        |CLK_TYPE_PLLL   \
//                        |CLK_TYPE_HCLK   \
//                        |CLK_TYPE_PCLK;
//  CLK_Struct.AHBSource      = CLK_AHBSEL_LSPLL;
//  CLK_Struct.PLLL.Frequency = CLK_PLLL_26_2144MHz;
//  CLK_Struct.PLLL.Source    = CLK_PLLLSRC_XTALL;
//  CLK_Struct.PLLL.State     = CLK_PLLL_ON;
//  CLK_Struct.HCLK.Divider   = 1;
//  CLK_Struct.PCLK.Divider   = 1;  //ԭֵΪ2 
//  CLK_ClockConfig(&CLK_Struct);
//}

//20210514 wwb modify
/**
  * @brief  Clock_Init:
              - PLLL input clock    : External 32K crystal
              - PLLL frequency      : 26M
              - AHB Clock source    : PLLL
              - AHB Clock frequency : 26M (PLLL divided by 1)
              - APB Clock frequency : 13M (AHB Clock divided by 2)
  * @param  None
  * @retval None
  */
void Clock_Init(void)
{
  CLK_InitTypeDef CLK_Struct;

  CLK_Struct.ClockType = CLK_TYPE_AHBSRC \
                        |CLK_TYPE_PLLH   \
                        |CLK_TYPE_HCLK   \
                        |CLK_TYPE_PCLK;
  CLK_Struct.AHBSource      = CLK_AHBSEL_HSPLL;
  CLK_Struct.PLLH.Frequency = CLK_PLLH_32_768MHz;
  CLK_Struct.PLLH.Source    = CLK_PLLHSRC_XTALH;
  CLK_Struct.PLLH.State     = CLK_PLLH_ON;
  CLK_Struct.HCLK.Divider   = 1;    //FLASH���г������֧��26MHz������/2  //20210630 modify for test
  CLK_Struct.PCLK.Divider   = 2;    //ԭֵΪ1 20220104 wwb modify 1 -> 2  
  CLK_ClockConfig(&CLK_Struct);
}

void uarts_init(void)
{
	UART_InitType UART_InitStruct;
	GPIO_InitType GPIO_InitStruct;
	
  /* UART0 RX pin(IOA12), input mode ��ʾ��ͨ�Ŵ���*/
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INPUT; //GPIO_Mode_INPUT
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
  GPIOA_Init(GPIOA, &GPIO_InitStruct);
  
  /* UART0 initialization */
  UART_DeInit(UART0);
  UART_StructInit(&UART_InitStruct);
  UART_InitStruct.Baudrate = 115200;    
  UART_InitStruct.Mode = UART_MODE_RX | UART_MODE_TX;
  UART_Init(UART0, &UART_InitStruct);
  
  /* Enable UART transmit doen/receive interrupt */
  UART_INTConfig(UART0, UART_INT_RX, ENABLE);
  UART_INTConfig(UART0, UART_INT_TXDONE, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(UART0_IRQn, 0);	
	
  /* UART1 RX pin(IOA13), input mode ������ͨ�Ŵ���*/
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INPUT; //GPIO_Mode_INPUT
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
  GPIOA_Init(GPIOA, &GPIO_InitStruct);
  
  /* UART1 initialization */
  UART_DeInit(UART1);
  UART_StructInit(&UART_InitStruct);
  UART_InitStruct.Baudrate = 4800;    
  UART_InitStruct.Mode = UART_MODE_RX | UART_MODE_TX;
  UART_Init(UART1, &UART_InitStruct);
  
  /* Enable UART transmit doen/receive interrupt */
  UART_INTConfig(UART1, UART_INT_RX, ENABLE);
  UART_INTConfig(UART1, UART_INT_TXDONE, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(UART1_IRQn, 0);	
	
  /* UART2 initialization ���ͨ�Ŵ���*/
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INPUT; //GPIO_Mode_INPUT
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
  GPIOA_Init(GPIOA, &GPIO_InitStruct);	
	
  UART_DeInit(UART2);
  UART_StructInit(&UART_InitStruct);
  UART_InitStruct.Baudrate = 9600;
  UART_InitStruct.Mode = UART_MODE_RX | UART_MODE_TX;
  UART_Init(UART2, &UART_InitStruct);
  
  /* Enable UART transmit doen/receive interrupt */
  UART_INTConfig(UART2, UART_INT_RX, ENABLE);
  UART_INTConfig(UART2, UART_INT_TXDONE, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(UART2_IRQn, 0);		
	
	//UART5 RX pin(IOB1)wifiͨ�Ŵ���
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INPUT; //GPIO_Mode_INPUT
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIOBToF_Init(GPIOB, &GPIO_InitStruct);	
	
  UART_DeInit(UART5);
  UART_StructInit(&UART_InitStruct);
  UART_InitStruct.Baudrate = 115200;
  UART_InitStruct.Mode = UART_MODE_RX | UART_MODE_TX;
  UART_Init(UART5, &UART_InitStruct);
  
  /* Enable UART transmit doen/receive interrupt */
  UART_INTConfig(UART5, UART_INT_RX, ENABLE);
  UART_INTConfig(UART5, UART_INT_TXDONE, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(UART5_IRQn, 0);

	uarts_data_init();
}

//����pwm0
void pwm0_init(void)
{
  PWM_BaseInitType PWM_BaseInitStruct;
  PWM_OCInitType PWM_OCInitStruct;
  
  /* PWM0 base initialization 
       - Count mode   : CONTINUOUS mode
       - Clock source : PCLK 13M
       - Clock Divide : divide by 16     */
  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_4;
  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;
  PWM_BaseInitStruct.Mode = PWM_MODE_CONTINUOUS; //PWM_MODE_UPCOUNT; //PWM_MODE_CONTINUOUS
  PWM_BaseInit(PWM0, &PWM_BaseInitStruct);


  /** PWM0 channel 1 Initialize:
       - Mode         : compare mode
       - Out Mode     : TOGGLE_RESET
       - Out line     : PWM1 output (IOB6)
       Output: 12.5Hz 
       High/Low = (0x8000-0x2000)/(0xFFFF-0x6000) = 3/5
    */
  PWM_OLineConfig(PWM0_OUT0, PWM_OLINE_0);
  PWM_OCInitStruct.OutMode = PWM_OUTMOD_RESET_SET; //PWM_OUTMOD_TOGGLE_RESET
  PWM_OCInitStruct.Period = 256;
  PWM_OC0Init(PWM0, &PWM_OCInitStruct);
	PWM0->CCR0 = 0x8000;
	
	PWM0->CCR1 = 0x4000;
  
  PWM_ClearCounter(PWM0);
  /* Enable PWM0 channel 0/1/2 output */
  PWM_OutputCmd(PWM0, PWM_CHANNEL_0, ENABLE);  //pwm3�޲������
  //PWM_OutputCmd(PWM0, PWM_CHANNEL_1, ENABLE);
//  PWM_OutputCmd(PWM0, PWM_CHANNEL_2, ENABLE);
  
  while (1)
  {
    WDT_Clear();
  }
}

//���ٵ������ pwm3
void main_motor_pwm_init(void)
{
  PWM_BaseInitType PWM_BaseInitStruct;
  PWM_OCInitType PWM_OCInitStruct;
  
  /* PWM0 base initialization 
       - Count mode   : CONTINUOUS mode
       - Clock source : PCLK 13M   ����ֵ��Ƶ32.768MHz��ʵ��ֻ����26MHz����
       - Clock Divide : divide by 16     */
  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_2;  //PWM_CLKDIV_4 20220104 modify
  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;
  PWM_BaseInitStruct.Mode = PWM_MODE_UPCOUNT; //PWM_MODE_CONTINUOUS
  PWM_BaseInit(PWM3, &PWM_BaseInitStruct);

  /** PWM0 channel 1 Initialize:
       - Mode         : compare mode
       - Out Mode     : TOGGLE_RESET
       - Out line     : PWM1 output (IOB6)
       Output: 12.5Hz 
       High/Low = (0x8000-0x2000)/(0xFFFF-0x6000) = 3/5
    */
  //PWM_OLineConfig(PWM3_OUT0, PWM_OLINE_0); //ԭ��д����0��20220204 wwb modify
  PWM_OLineConfig(PWM3_OUT1, PWM_OLINE_1);	
  PWM_OCInitStruct.OutMode = PWM_OUTMOD_RESET_SET; //PWM_OUTMOD_TOGGLE_RESET
  //PWM_OCInitStruct.Period = 256;     //Ƶ��8K	
  PWM_OC1Init(PWM3, &PWM_OCInitStruct);

	PWM3->CCR0 = 1024;                 //Ƶ��8K
	PWM3->CCR1 = 512;                  //20211105 wwb mask 512->614
  
  PWM_ClearCounter(PWM3);
  /* Enable PWM0 channel 0/1/2 output */
  //PWM_OutputCmd(PWM3, PWM_CHANNEL_0, ENABLE);  //pwm3�޲������
  PWM_OutputCmd(PWM3, PWM_CHANNEL_1, DISABLE);
//  PWM_OutputCmd(PWM3, PWM_CHANNEL_2, ENABLE);
}

//��ˢ�������pwm2,2ms��,����8ms
void brush_motor_pwm_init(void)
{
	//GPIO_InitType GPIO_InitStruct;
	
  PWM_BaseInitType PWM_BaseInitStruct;
  PWM_OCInitType PWM_OCInitStruct;

  /* PWM0 base initialization 
       - Count mode   : CONTINUOUS mode
       - Clock source : PCLK 13M
       - Clock Divide : divide by 16     */
  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_2;   //125Hz. PWM_CLKDIV_4 20220104 modify
  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;
  PWM_BaseInitStruct.Mode = PWM_MODE_CONTINUOUS;
  PWM_BaseInit(PWM0, &PWM_BaseInitStruct);

//  /** PWM0 channel 0 Initialize:
//       - Mode         : compare mode
//       - Out Mode     : TOGGLE
//       - Out line     : PWM0 output (IOB0)
//       Output: 6.25Hz 
//       High/Low = 1/1
////    */   
//  PWM_OLineConfig(PWM0_OUT0, PWM_OLINE_0);
//  PWM_OCInitStruct.OutMode = PWM_OUTMOD_TOGGLE_SET;
//  PWM_OCInitStruct.Period = 0xC000;
//  PWM_OC0Init(PWM0, &PWM_OCInitStruct);
//	
//	////High/Low = = (0x8000-0x4000)/(0xFFFF-0x4000) = 1/3
//	PWM_OLineConfig(PWM0_OUT1, PWM_OLINE_1);
//  PWM_OCInitStruct.OutMode = PWM_OUTMOD_TOGGLE_RESET;  //PWM_OUTMOD_TOGGLE_RESET
//  PWM_OCInitStruct.Period = 0x1000;   //0x6400;
//  PWM_OC1Init(PWM0, &PWM_OCInitStruct);   //����������PWM_OC1Init(PWM0, &PWM_OCInitStruct);->�ر�ע�ⲻҪд���ˣ���Ҫд��:PWM_OC0Init
//	
	
	PWM0->CCR0 = 0;   //Ĭ��ֵ��0
	
	PWM_OLineConfig(PWM0_OUT2, PWM_OLINE_2);
  PWM_OCInitStruct.OutMode = PWM_OUTMOD_TOGGLE_SET;  //PWM_OUTMOD_TOGGLE_RESET
  PWM_OCInitStruct.Period = 0x4000;   //0x4000;
  PWM_OC2Init(PWM0, &PWM_OCInitStruct);   //����������PWM_OC1Init(PWM0, &PWM_OCInitStruct);->�ر�ע�ⲻҪд���ˣ���Ҫд��:PWM_OC0Init
	
  
  PWM_ClearCounter(PWM0);
  /* Enable PWM0 channel 0/1/2 output */
  //PWM_OutputCmd(PWM0, PWM_CHANNEL_0, DISABLE);
	//PWM_OutputCmd(PWM0, PWM_CHANNEL_1, DISABLE);
  PWM_OutputCmd(PWM0, PWM_CHANNEL_2, DISABLE);	
}

//32.768MHz������Ƶ,PCLK����Ƶ����Сֻ��֧�ֵ�30Hz��pwm��
//void pump_motor_pwm_init(void)
//{
//  PWM_BaseInitType PWM_BaseInitStruct;
//  PWM_OCInitType PWM_OCInitStruct;
//  
//  /* PWM0 base initialization 
//       - Count mode   : CONTINUOUS mode
//       - Clock source : PCLK 13M
//       - Clock Divide : divide by 16     */
//  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_16;
//  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;
//  PWM_BaseInitStruct.Mode = PWM_MODE_UPCOUNT; //PWM_MODE_UPCOUNT; //PWM_MODE_CONTINUOUS
//  PWM_BaseInit(PWM1, &PWM_BaseInitStruct);


//  /** PWM0 channel 1 Initialize:
//       - Mode         : compare mode
//       - Out Mode     : TOGGLE_RESET
//       - Out line     : PWM1 output (IOB6)
//       Output: 12.5Hz 
//       High/Low = (0x8000-0x2000)/(0xFFFF-0x6000) = 3/5
//    */
//  PWM_OLineConfig(PWM1_OUT1, PWM_OLINE_1);
//  PWM_OCInitStruct.OutMode = PWM_OUTMOD_RESET_SET; //PWM_OUTMOD_TOGGLE_RESET
//  //PWM_OCInitStruct.Period = 256;
//  PWM_OC1Init(PWM1, &PWM_OCInitStruct);

//	PWM1->CCR0 = 0xFFFF;
//	PWM1->CCR1 = 0x2000;
//  
//  PWM_ClearCounter(PWM1);
//  /* Enable PWM0 channel 0/1/2 output */
//  //PWM_OutputCmd(PWM1, PWM_CHANNEL_0, ENABLE);  //pwm1�޲������
//  PWM_OutputCmd(PWM1, PWM_CHANNEL_1, ENABLE);
////  PWM_OutputCmd(PWM1, PWM_CHANNEL_2, ENABLE);
//}

//PCLK����Ƶ 15Hz
void pump_motor_pwm_init(void)
{
  PWM_BaseInitType PWM_BaseInitStruct;
  PWM_OCInitType PWM_OCInitStruct;
  
  /* PWM0 base initialization 
       - Count mode   : CONTINUOUS mode
       - Clock source : PCLK 13M
       - Clock Divide : divide by 16     */
  PWM_BaseInitStruct.ClockDivision = PWM_CLKDIV_16;
  PWM_BaseInitStruct.ClockSource = PWM_CLKSRC_APB;
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
	PWM1->CCR1 = 0x3333;   //20%ռ�ձ�
  
  PWM_ClearCounter(PWM1);
  /* Enable PWM0 channel 0/1/2 output */
  //PWM_OutputCmd(PWM1, PWM_CHANNEL_0, ENABLE);  //pwm1�޲������
  PWM_OutputCmd(PWM1, PWM_CHANNEL_1, DISABLE);
//  PWM_OutputCmd(PWM1, PWM_CHANNEL_2, ENABLE);	
}

void motors_pwm_init(void)
{
	main_motor_pwm_init();
	brush_motor_pwm_init();
	pump_motor_pwm_init();
}

void PMU_init(void)
{
  GPIO_InitType GPIO_InitStruct;
  PMU_LowPWRTypeDef LowPower_InitStruct;	
	
	/* Disable WDT */
  WDT_Disable();
	
  /* Can enter deep-sleep mode, when VDD5/VDCIN is not drop */
  //PMU_PDNDSleepConfig(PMU_VDCINPDNS_1, PMU_VDDPDNS_1);     //Ҫȥ��
  
  /*------------------ Forbidden all GPIOs ------------------*/
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_FORBIDDEN;
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All & (~(GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14));
  GPIOA_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All & (~(GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15));
  GPIOBToF_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All & (~(GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8));
  GPIOBToF_Init(GPIOC, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All;
  GPIOBToF_Init(GPIOD, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All & (~GPIO_Pin_5);
  GPIOBToF_Init(GPIOE, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All & (~(GPIO_Pin_0 | GPIO_Pin_1));
  GPIOBToF_Init(GPIOF, &GPIO_InitStruct);

  /*------------------ Low power configuration ------------------*/
  LowPower_InitStruct.AHBPeriphralDisable = PMU_AHB_ALL & (~MISC2_HCLKEN_GPIO);
  //LowPower_InitStruct.APBPeriphralDisable = PMU_APB_ALL & (~(MISC2_PCLKEN_SPI1 | MISC2_PCLKEN_SPI2));  //20210916 wwb modify
  LowPower_InitStruct.APBPeriphralDisable = PMU_APB_ALL & (~(MISC2_PCLKEN_PMU | MISC2_PCLKEN_MISC | MISC2_PCLKEN_MISC2 | MISC2_PCLKEN_TIMER | MISC2_PCLKEN_SPI1 | MISC2_PCLKEN_SPI2 | MISC2_PCLKEN_RTC | MISC2_PCLKEN_ANA));  //20210801 wwb modify	
  LowPower_InitStruct.BGPPower            = PMU_BGPPWR_OFF;
  LowPower_InitStruct.COMP1Power          = PMU_COMP1PWR_OFF;
  LowPower_InitStruct.COMP2Power          = PMU_COMP2PWR_OFF;
  LowPower_InitStruct.LCDPower            = PMU_LCDPWER_OFF;
  LowPower_InitStruct.AVCCPower           = PMU_AVCCPWR_ON;
  LowPower_InitStruct.TADCPower           = PMU_TADCPWR_OFF;
  LowPower_InitStruct.VDCINDetector       = PMU_VDCINDET_DISABLE;
	LowPower_InitStruct.VDDDetector         = PMU_VDDDET_DISABLE;   //������
  //PMU_EnterDSleep_LowPower(&LowPower_InitStruct);
  PMU_EnterSleep_LowPower(&LowPower_InitStruct);	
}

void adc_auto_trig_init(void)
{
  ADCInitType ADC_InitStruct;
  TMR_InitType TMR_InitStruct; 
 
   /* ADC initialization */
  ADC_DeInit();
  ADC_InitStruct.TrigMode = ADC_TRIGMODE_AUTO;
  ADC_InitStruct.ConvMode = ADC_CONVMODE_MULTICHANNEL;
  ADC_InitStruct.ClockDivider = ADC_CLKDIV_4;
  ADC_InitStruct.ClockSource = ADC_CLKSRC_RCH;
  ADC_Init(&ADC_InitStruct);
  /* Cap division x1/4 */
  ADC_CAPDivisionCmd(ENABLE);
   
  /* Enable ADC */
  ADC_Cmd(ENABLE);

  /* Configure Timer0 as ADC trigger source */
  ADC_TrigSourceConfig(ADC_TRIGSOURCE_TIM0);
  
  /* Timer0 configuration */
  TMR_DeInit(TMR0);
  TMR_InitStruct.ClockSource = TMR_CLKSRC_INTERNAL;
  TMR_InitStruct.EXTGT = TMR_EXTGT_DISABLE;
  TMR_InitStruct.Period = 13107 - 1;  //ԭֵ13107200 - 1;  1ms����һ��adc����
  TMR_Init(TMR0, &TMR_InitStruct);
  
  /* Enable Timer0 */
  TMR_Cmd(TMR0, ENABLE);
}

void timer0_init(void)
{
  TMR_InitType TMR_InitStruct;
  
  /* Timer0 initialization:
       - Clock source: internal clock(APB clock 13107200Hz) 
       - Overflow interval: 10ms                             */
  TMR_DeInit(TMR0);
  TMR_InitStruct.ClockSource = TMR_CLKSRC_INTERNAL;
  TMR_InitStruct.EXTGT = TMR_EXTGT_DISABLE;
  TMR_InitStruct.Period = 13107200/100 - 1;
  TMR_Init(TMR0, &TMR_InitStruct);
  
  /* Enable Timer0 interrupt */
  TMR_INTConfig(TMR0, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(TMR0_IRQn, 0);
  
  /* Enable Timer0 */
  TMR_Cmd(TMR0, ENABLE);
}

void GPIO_init(void)
{
	GPIO_InitType GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUTPUT_CMOS;
	
	UART_DeInit(UART0);	   //IOB2����Ϊio��	
//	//IOB6 ˮ�ÿ��ƣ�IOB12 ���ʹ�� IOB15 ���ⷢ�ͣ�ˮ�ø�Ϊpwm1����
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_12 | GPIO_Pin_15;
//  GPIOBToF_Init(GPIOB, &GPIO_InitStruct);
//	GPIOBToF_ResetBits(GPIOB, GPIO_InitStruct.GPIO_Pin);
	//IOB12 ���ʹ�� IOB15 ���ⷢ�ͣ�
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15;
  GPIOBToF_Init(GPIOB, &GPIO_InitStruct);
	GPIOBToF_ResetBits(GPIOB, GPIO_InitStruct.GPIO_Pin);	

	//IOD12 ���ˮ12V����;IOD14 wifiģ�����ATָ��ģʽ(��ʹ��ʱ����);IOD15 12V_EN ��ʱ��12V_EN��ʹ����ʾ����3.3V
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIOBToF_Init(GPIOD, &GPIO_InitStruct);
	GPIOBToF_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_14); //20211220
	GPIOBToF_ResetBits(GPIOD, GPIO_Pin_15);
	//GPIOBToF_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_14);
	
		//IOC7 5V_EN,IOC9 RST_WIFI
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11;
  GPIOBToF_Init(GPIOC, &GPIO_InitStruct);
	GPIOBToF_SetBits(GPIOC, GPIO_Pin_7 | GPIO_Pin_9);  //20211220
	//GPIOBToF_ResetBits(GPIOC, GPIO_Pin_7 | GPIO_Pin_9);
	
	
	//IOF0 ��ˮͰ��װ���
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INPUT;	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIOBToF_Init(GPIOF, &GPIO_InitStruct);
	
	//IOA3���ת�ټ��(���ڵ��ת�ټ��) IOA5ģʽ��,IOA6������,IOA7ֱ���ж� //IOA9 ��ˢ���
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INPUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
  GPIOA_Init(GPIOA, &GPIO_InitStruct);
	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INOUT_CMOS;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_9;
//  GPIOA_Init(GPIOA, &GPIO_InitStruct);	
}

void brush_err_notify(void)
{
		if (sys_faults.flags.brush_type_error)
		{
				if (0 == sys_time.brush_err_notify_cnt) //��ˢ���ʹ��󣬼��5s�����
				{}                              
					
				if (++sys_time.brush_err_notify_cnt >= 500) 
					sys_time.brush_err_notify_cnt = 0;
		}
		else
				sys_time.brush_err_notify_cnt = 0;
}

void bottom_unconnect_notify(void)
{
		if (!sys_status.flags.bottom_connected)
		{
				if (0 == sys_time.bottom_unconnect_notify_cnt) //��ˢ���ʹ��󣬼��5s�����
				{}                              
					
				if (++sys_time.bottom_unconnect_notify_cnt >= 500) 
					sys_time.bottom_unconnect_notify_cnt = 0;
		}
		else
				sys_time.bottom_unconnect_notify_cnt = 0;
}

void wsx_full_notify(void)
{
		if (sys_faults.flags.wsx_full)
		{
				if (0 == sys_time.wsx_full_notify_cnt) //��ˮ���������5s�����
				{}                              
					
				if (++sys_time.wsx_full_notify_cnt >= 500) 
					sys_time.wsx_full_notify_cnt = 0;
		}
		else
				sys_time.wsx_full_notify_cnt = 0;
}

//ֱ�����
void upright_check(void)  
{
		if(0 == GPIOA_ReadInputDataBit(GPIOA, GPIO_Pin_7))  //ֱ��ʶ�𣬵͵�ƽ��Ч
		{
			if (++sys_time.upright_check_cnt >= 3)  //��ʱȥ��
			{
					sys_time.upright_check_cnt = 0;
					sys_status.flags.upright = TRUE;
			}
		}
		else     //ֱ����ʧ
		{
			sys_time.upright_check_cnt = 0;
			sys_status.flags.upright = FALSE;
		}
}

//��ˮ��δ��װ���
void wsx_install_check(void)
{
		if(1 == GPIOBToF_ReadInputDataBit(GPIOF, GPIO_Pin_0))  //��װ���
		{
			if (++sys_time.wsx_uninstall_check_cnt >= 3)  //��ʱȥ��
			{
					sys_time.wsx_uninstall_check_cnt = 0;
					sys_faults.flags.wsx_uninstall = TRUE;
			}
		
			if (!sys_faults.flags.wsx_uninstall)
			{			
					if (0 == sys_time.wsx_uninstall_notify_cnt)
					{} 			 //�����
						
					if (++sys_time.wsx_uninstall_notify_cnt >= 500)
						sys_time.wsx_uninstall_notify_cnt = 0;							
			}                              	
		}
		else
		{
			sys_time.wsx_uninstall_check_cnt = 0;
			sys_time.wsx_uninstall_notify_cnt = 0;
			sys_faults.flags.wsx_uninstall = FALSE;
		}	
}

void ir_tx_enable(void)
{
	ir.tx_en = TRUE;
	GPIOBToF_SetBits(GPIOB, GPIO_Pin_15);
}

void ir_tx_disable(void)
{
	ir.tx_en = FALSE;	
	GPIOBToF_ResetBits(GPIOB, GPIO_Pin_15);
}

void work_mode_proc(void)
{
		uint8_t i = 0;

		switch(work_mode.mode)
		{
			case INTELLIGENT_MODE:        //����ģʽ
				//1����ʾ����ģʽͼƬ
			  //2��������ʾ
			  //3����Ӧ�������ʹ��
				switch(work_mode.step)
				{
					case 0:   //��ʾͼƬ
						picture_display(PIC_INTELLIGENT);
						ir_tx_enable();
						work_mode.step++;

						break;
					
					case 1:   //��ʱ�䲥������
						voice_play(TURN_ON);
						work_mode.step++;
					
						break;
					
					default:
						break;				
				}
				
				for (i = 0; i < MOTOR_CTRL_NUM; i++)
				{
					if (sys_status.val)            //ֱ�������ӵ�����ͣ���е��
						motor_ctrl[i].en = DISABLE;
					else                           //׼�������е��
						motor_ctrl[i].en = ENABLE;
				}

				brush_err_notify();
				
				break;
			
			case CARPET_MODE:        //��̺ģʽ
				switch(work_mode.step)
				{
					case 0:   //��ʾͼƬ
						picture_display(PIC_CARPET);
						ir_tx_enable();
						work_mode.step++;
					  
						break;
					
					case 1:   //��ʱ�䲥������
						voice_play(TURN_OFF);
						work_mode.step++;
					
						break;
					
					default:
						break;				
				}
			
				motor_ctrl[PUMP_MOTOR].en = DISABLE;
				if (sys_status.val)            //ֱ�������ӵ�����ͣ���е��
				{
					motor_ctrl[MAIN_MOTOR].en = DISABLE;
					motor_ctrl[BRUSH_MOTOR].en = DISABLE;
				}
				else                             //׼����������͹�ˢ���
				{
					motor_ctrl[MAIN_MOTOR].en = ENABLE;
					motor_ctrl[BRUSH_MOTOR].en = ENABLE;
				}

				brush_err_notify();

				break;
			
			case AUTO_CLEAN_MODE:        //�����ģʽ
				switch(work_mode.step)
				{
					case 0:   //��ʾͼƬ
						picture_display(PIC_AUTO_CLEAN);
						ir_tx_disable();
						work_mode.step++;
					  
						break;
					
					case 1:   //��ʱ�䲥������
						voice_play(TURN_OFF);
						work_mode.step++;
					
						break;
					
					default:
						break;				
				}
				
				if (!uart[1].flags.b.comm_start && 0 == work_mode.query_timeout_cnt)
					uart1_send_cmd(CONNECT_QUERY);
				if (++work_mode.query_timeout_cnt >= 100)  //1s��ѯһ��	
					work_mode.query_timeout_cnt = 0;
		
			  bottom_unconnect_notify();
				if (!sys_status.flags.bottom_connected)    //����δ���ӣ��˳�
					break;
			
				switch(work_mode.step)
				{
					case 2:              //����ϴ,���е������,�������������ת
						for (i = 0; i < MOTOR_CTRL_NUM; i++)
						{
							if (sys_status.val)            //ֱ�������ӵ����������е��
								motor_ctrl[i].en = ENABLE;
							else                           //�����е��
								motor_ctrl[i].en = DISABLE;
						}
						if (work_mode.time_cnt >= 1200)   //2������ϴ����
						{
								work_mode.time_cnt = 0;
								work_mode.step = 1;
								motor_ctrl[MAIN_MOTOR].en = ENABLE;   //�������
							  motor_ctrl[PUMP_MOTOR].en = DISABLE;  //��ˮ��
							
								uart1_send_cmd(DRY_START);            //1�����������Ϳ�ʼ��ɿ�ʼָ��
						}
						
						break;
					
					case 3:              //���
						if (sys_status.val)                    //ֱ����������ʱ����ˢһֱ��������
							motor_ctrl[BRUSH_MOTOR].en = ENABLE;
						else                          
							motor_ctrl[BRUSH_MOTOR].en = DISABLE;
						
						if (work_mode.time_cnt >= 180000)     //���30����
						{
								motor_ctrl[BRUSH_MOTOR].en = DISABLE;
								work_mode.step = 0;
							
								//���ͺ�ɽ���ָ��
								uart1_send_cmd(DRY_STOP);
							
							  //׼���ػ�
							  work_mode.mode = 0xff;  //??????
						}
						break;
					
					default:
						break;
				}	

				work_mode.time_cnt++;
				break;
			
			default:				
				break;		
		}
	
		wsx_full_notify();
}

//���۳̶���ʾ
void dirty_display(void)
{
	if (!ir.tx_en)
		return;
	
	switch(ir.dirty_value)
	{
		case NO_DIRTY:
			picture_display(PIC_DIRTY);
			break;
		case DIRTY:
			picture_display(PIC_DIRTY + 1);
			break;
		
		case VERY_DIRTY:
			picture_display(PIC_DIRTY + 2);
			break;
		
		default:
			break;
	}
}

/*****************************************************************************
 * Function:    void Task0_Process(void)
 * Description:
 *
 * Caveats:
 *
 *****************************************************************************/
void Task0_Process(void)
{
		key_proc();
		upright_check();  
		wsx_install_check();
		work_mode_proc();	
		motor_proc();
	  dirty_display();

		WDT_Clear();              // Feed  WDT	
}

/*****************************************************************************
 * Function:    void Task1_Process(void)
 * Description:
 *
 * Caveats:
 *
 *****************************************************************************/
void Task1_Process(void)
{
	  //voltage_sample();
	
	  //���ת�ټ����ʱ��ͳ��
		__disable_irq();
	  if (wsx_full_check.motor_speed_int_cnt > 0)
			wsx_full_check.motor_speed_timer_cnt++;
		__enable_irq();
}

/*****************************************************************************
 * Function:    void Task2_Process(void)
 * Description: ����ͨ�Ŵ���
 *
 * Caveats:
 *
 *****************************************************************************/
void Task2_Process(void)
{   
	if (++uart[2].tx_interval >= 5)  //500ms ��ѯһ�ε����Ϣ
	{
		uart[2].tx_interval = 0;
		uart2_send_cmd(READ_INFO);
	}
	uarts_comm_proc();
}

#ifndef  ASSERT_NDEBUG
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_errhandler error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_errhandler error line source number
  * @retval None
  */
void assert_errhandler(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

//uint8_t query_vesion[7] = {0x5A,0xA5,0x04,0x83,0x00,0x0F,0x01};

//#define T4 *((volatile unsigned long *)((((uint32_t) IOC_DAT) & 0xF0000000)+0x2000000+((((uint32_t)IOC_DAT) &0xFFFFF)<<5)+(11<<2)))  //IOC11 T4
//uint8_t f_test = 0;

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	WDT_Disable();
  Clock_Init();
	
	GPIO_init();
	
  PMU_WakeUpPinConfig(GPIO_Pin_3, IOA_FALLING);
	PMU_ClearIOAINTStatus(GPIO_Pin_3);
	PMU_WakeUpPinConfig(GPIO_Pin_6, IOA_FALLING);
	PMU_ClearIOAINTStatus(GPIO_Pin_6);	
  PMU_INTConfig(PMU_INT_IOAEN, ENABLE);
  CORTEX_SetPriority_ClearPending_EnableIRQ(PMU_IRQn, 0);	
		
	delay_init();
	
	delay_ms(10);
	GPIOBToF_SetBits(GPIOD, GPIO_Pin_15);	
	
	adc_init(adc_channel);  

	motors_pwm_init();
	motor_ctrl_init();
	
	uarts_init();      
	user_data_init();
	Task_InitTime();
	  
	/* Enable SysTick timer and its interrupt to generate interrupt(1ms) */
	CORTEX_SystemTick_Config(16384000/500);   //HCLK 1ms 

//PMU_init();   //�͹��Ĳ���

  WDT_Enable();  
	
	Stdio_Init();//log init
	printf(" c9 project start \n");
//	while(1){
//			//Delay_ms(1000);
//			delay_ms(1000);
//			printf("test log \n");
//	}
	
//	__disable_irq();
//	__enable_irq();	

  while (1)
  {
			//WDT_Clear();                         // Feed  WDT
		
			if (TaskTimer[0] == 0) {             // Task2:   10MS --Read Key,Check Uart Break and Feed WDT
					Task0_Process();
					TaskTimer[0] = TASK0_CYCLE_VALUE;									
			}

			if (TaskTimer[1] == 0) {             // Task1:    1MS --AD Samping				  
					Task1_Process();				  
					TaskTimer[1] = TASK1_CYCLE_VALUE;
			}	

			if (TaskTimer[2] == 0) {             // Task2:    100MS ����ͨ�Ŵ���
					Task2_Process();				  
					TaskTimer[2] = TASK2_CYCLE_VALUE;
			}
			
			voltage_sample();     //����ʱЧ��Ҫ��ϸߣ����г��� 
//			f_test = 1 - f_test;//����������ƽ��2.8-6.4usѭ��һ��,���Լ48us
//  		T4 = f_test;
  }
}

/*********************************** END OF FILE ******************************/
