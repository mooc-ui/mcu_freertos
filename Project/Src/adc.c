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
#include "lib_adc.h"
#include "adc.h"
#include "motor_ctrl.h"
#include "user.h"
#include "target_isr.h"
#include "main.h"
#include <stdbool.h>
#include "delay.h"

uint16_t adc_result = 0;

volatile uint8_t no_water_flag = 0;
volatile uint8_t last_no_water_flag = 0;
volatile uint8_t charge_flag = 0;
volatile uint8_t charge_over_flag = 0;
volatile uint8_t low_bat_flag = 0;
volatile uint8_t no_water_display = 1;
volatile uint8_t charge_insert_flag = 0;

uint8_t  AD_Cnt = 0;
uint16_t AD_AverageValue = 0;
uint16_t AD_Buf[7][4]={0xF80,0xF81,0xF82,0xF83};
uint16_t ADSortBuf(uint16_t buf[]);

ADC_T adc[ADC_CH_NUM];

uint32_t  adc_channel = ADC_CHANNEL6;

extern volatile uint8_t run_flag; 
extern volatile uint8_t brush_type;

extern sys_status_t sys_status;

bool adcConvertDone = false;
sampleAdcSelect qAdcResultValue = {0};

void adc_init(uint32_t Channel)
{
  ADCInitType ADC_InitStruct;
	GPIO_InitType GPIO_InitStruct;
	
	//GPIO_InitType GPIO_InitStruct;
  
//  /* Forbidden ADC channel3's pin */
//  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_FORBIDDEN;
//  GPIO_InitStruct.GPIO_Pin = ADC_CHANNEL3_Pin;
//  GPIOA_Init(ADC_CHANNEL3_GPIO, &GPIO_InitStruct);
	
//	//ADC3
//  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_FORBIDDEN;
//  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
//  GPIOA_Init(GPIOA, &GPIO_InitStruct);	
//	
//	//ADC7 ADC8
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//  GPIOBToF_Init(GPIOE, &GPIO_InitStruct);	
	
  /* ADC initialization */
  ADC_DeInit();
  ADC_InitStruct.TrigMode = ADC_TRIGMODE_MANUAL;
  ADC_InitStruct.ConvMode = ADC_CONVMODE_SINGLECHANNEL;
	ADC_InitStruct.Channel = Channel;
  ADC_InitStruct.ClockDivider = ADC_CLKDIV_1;   //ADC_CLKDIV_1
  ADC_InitStruct.ClockSource = ADC_CLKSRC_RCH; //ԭֵADC_CLKSRC_RCH
  ADC_Init(&ADC_InitStruct);
	
	
  ADC_CAPDivisionCmd(ENABLE);

	ADC_INTConfig(ADC_INT_MANUALDONE,ENABLE);
	CORTEX_NVIC_ClearPendingIRQ(ANA_IRQn);
	CORTEX_NVIC_EnableIRQ(ANA_IRQn);//adc interrupt config
  
  /* Enable ADC */
  ADC_Cmd(ENABLE);
  /* Starts a manual ADC conversion */
  ADC_StartManual();
}


void ANA_IRQHandler(void)
{
		if(1 == ADC_GetManualDoneFlag()){
				ADC_ClearManualDoneFlag();
				qAdcResultValue.getChannelAdcValue = ADC_GetADCConversionValue(qAdcResultValue.channel);
				//printf("adc manual convert done\n");
		}
	
//		if(1 == ADC_GetAutoDoneFlag()){
//				ADC_ClearAutoDoneFlag();
//				qAdcResultValue.getChannelAdcValue = ADC_GetADCConversionValue(qAdcResultValue.channel);
//				//printf("adc auto convert done\n");	
//		}
		
		adcConvertDone = true;
}


/*
*adcChannelSelect param select :
*ADC_CHANNEL3
*ADC_CHANNEL4
*ADC_CHANNEL7
*ADC_CHANNEL8
*ADC_CHANNEL9
*ADC_CHANNEL11
*/

void startADCSample(uint32_t adcChannelSelect)
{
		adc_init(adcChannelSelect);
		ADC_StartManual();
}

void getAdcSampleValueResult(void)
{
		//
}

void setAdcSampleValueResult(void)
{
		//
}

/*
* adc test function
*/

void adcTestFunction(void)
{
	static float voltageConvertResultValue;
	static uint8_t adcChannelTotal[ADC_SAMPLE_CHANNEL] = {ADC_CHANNEL3,ADC_CHANNEL4,ADC_CHANNEL6,ADC_CHANNEL7,ADC_CHANNEL8,ADC_CHANNEL9,ADC_CHANNEL11};//���ı�ͨ��������ʱ��ǵ��޸ĺ��С
	static uint8_t adcChannelIndex = 0;	
	
	if(adcConvertDone){
			adcConvertDone = false;
			//if(ADC_CalculateVoltage(ADC_3V_EXTERNAL_NODIV, adcValue, &voltageConvertResultValue))
			if (ADC_CalculateVoltage(ADC_3V_EXTERNAL_CAPDIV, qAdcResultValue.getChannelAdcValue, &voltageConvertResultValue))
			{
				printf("NVR checksum error!\r\n");
			}else{
				printf("channel %d adcValue = %d Voltage is %.3fV\r\n", qAdcResultValue.channel, qAdcResultValue.getChannelAdcValue, voltageConvertResultValue);	
			}						
	}

	startADCSample(adcChannelTotal[adcChannelIndex%ADC_SAMPLE_CHANNEL]);
	qAdcResultValue.channel = adcChannelTotal[adcChannelIndex%ADC_SAMPLE_CHANNEL];
	adcChannelIndex++;
}

uint8_t adc_is_done(void)
{
	return (ANA->ADCCTRL & ANA_ADCCTRL_MTRIG) ? 0 : 1;
}

/*****************************************************************************
 * Function:      uint16_t ADSortBuf(uint16_t buf[])
 * Description:   Data sorting 
 *                Four data, remove the maximum and minimum value, 
 *                the middle two data values average
 * Caveats:       Execution time about 9.22uS @SystemClock 48MHZ
 *
 *****************************************************************************/
uint16_t ADSortBuf(uint16_t buf[])
{
		uint16_t temp,i,j;
    uint16_t value[4];
	
	  value[0] = buf[0];
	  value[1] = buf[1];
	  value[2] = buf[2];
	  value[3] = buf[3];

		for(i=0;i<3;i++) {
				for(j=0;j<(3-i);j++) {
						if(value[j] > value[j+1]) {
								temp = value[j];
								value[j] = value[j+1];
								value[j+1] = temp;
						}
				}
		}
		temp = value[1] + value[2];

		return ( temp/2 );
}

/*****************************************************************************
 * Function:    void Temperature_Sampling(void)
 * Description:   
 *                
 * Caveats:     Time from start of sampling to completion of conversion
 *              about: 3.14uS (Conversion Rate 1Msps)
 *
 *****************************************************************************/
uint16_t adc_value_test[100] = {0};
//void voltage_sample(void)
//{
//		uint8_t arr_index = 0;
//	
//    if(adc_is_done())   
//		{
//				ADC_ClearManualDoneFlag();
//				adc_result = (uint16_t)ADC_GetADCConversionValue(adc_channel); //ʵ�ʵ�ѹֵvol_ch3 = (adc_data - 49.588) / 7088.91575;
//			
//				if (ADC_CHANNEL6 == adc_channel)  //��ˢ,������� I=36.7*V-55,
//				{
//					if (adc_result >= 11252)  //11445��4A���� 11252��3A����, 10866��1A����,11059(����ֵ):2A //20210929 ԭֵ10000�������󱣻�,�޸� 10400�ᱣ�� ���蹦��С����תʱ�䳤���׻�
//					{
//						brush_oc_cnt++;
//					}
//					
//					adc_value_test[brush_adc_cnt] = adc_result;
//					brush_adc_cnt++;
//					if (brush_adc_cnt >= 100)
//					{
//						if (brush_oc_cnt >= 15) //100�μ�� ��15�ι���
//						{
//							sys_faults.flags.brush_motor_oc = TRUE;  //�ù�ˢ���������־
//						}
//						else
//						{
//							sys_faults.flags.brush_motor_oc = FALSE;
//							adc_channel = ADC_CHANNEL11;  //��һ�μ��ˮ��
//							adc_init(adc_channel);
//						}
//						
//						brush_adc_cnt = 0;
//						brush_oc_cnt = 0;
//						AD_Cnt = 0;   //20210908 add
//						
//						return;
//						
//					}
//					else
//						ADC_StartManual();
//				}
//				else if (ADC_CHANNEL11 == adc_channel)  //ˮ��,������� I=36.7*V-55,
//				{
//					if (adc_result >= 11252)  //11445��4A���� 11252��3A����  10866��1A����
//						pump_oc_cnt++;
//					
//					adc_value_test[pump_adc_cnt] = adc_result;
//					pump_adc_cnt++;
//					if (pump_adc_cnt >= 100)
//					{
//						if (pump_oc_cnt >= 15) //100�μ�� ��15�ι���
//						{
//							//�õ�ˢ���������־
//							sys_faults.flags.pump_motor_oc = TRUE;
//						}
//						else
//						{
//							sys_faults.flags.pump_motor_oc = FALSE;
//							adc_channel = ADC_CHANNEL3;  
//						}
//						
//						pump_adc_cnt = 0;
//						pump_oc_cnt = 0;
//						adc_init(adc_channel);
//						AD_Cnt = 0;  //20210908 add  
//						
//						return;
//					}
//					else
//						ADC_StartManual();
//				}
//			
//				//����adcͨ����������
//				switch(adc_channel)
//				{
//					case ADC_CHANNEL3:       //�����
//						arr_index = 0;							
//						break;
//					
//					case ADC_CHANNEL4:       //��ˢ���  20211027 wwb add
//						arr_index = 1;							
//						break;					
//					
//					case ADC_CHANNEL7:       //VBUS��Դ��ѹ����
//						
//						arr_index = 2;
//						break;

////					case ADC_CHANNEL8:       //���ˮ����
////						
////						arr_index = 2;
////						break;
//												
//					case ADC_CHANNEL9:       //IR_R �������  ���۳̶ȼ��
//						
//						arr_index = 3;
//						break;
//					
//					default:
//						break;
//				}						
//			
//				AD_Buf[arr_index][AD_Cnt++] = adc_result;
//				if(AD_Cnt >= 4)    {
//						AD_Cnt = 0;
//						AD_AverageValue = ADSortBuf(AD_Buf[arr_index]);
//					
//					  //adcͨ����������
//					  switch(adc_channel)
//						{
//							case ADC_CHANNEL3:       //�����
////								if (AD_AverageValue >= 15000 && AD_AverageValue < 0x7000 && !run_flag && 3 != work_mode.mode && !charge_over_flag)  //����ѹ����20V����ʼ���,���ϳ���ͷΪͨ��ʱ��ѹ0x7f00
////								{
////									charge_insert_flag = 1;  //20211027 wwb mask
////									GPIOBToF_SetBits(GPIOF, GPIO_Pin_0);
////									charge_flag = 1;
////								}
////								else                           //�رճ��
////								{
////									charge_flag = 0;
////									charge_insert_flag = 0;
////									charging = 0;
////									GPIOBToF_ResetBits(GPIOF, GPIO_Pin_0);
////								}																		
//							
//								adc_channel = ADC_CHANNEL4;
//								break;
//								
//							case ADC_CHANNEL4:       //��ˢ��� 20211027 wwb add
//								//װ��ˢ0x13C4 δװ��ˢ0x21B3   װ0x26EA
////								if (AD_AverageValue > 0x2000)
////									brush_type = 0;      //0 δװ��ˢ; 1 ����ģʽ��ˢ; 2 ��̺ģʽ��ˢ
////								else if (AD_AverageValue > 0x1000)
////									brush_type = 1;
//							  //����ˢ�ˣ�װ0x26EA δװ0x20F9
//								if (AD_AverageValue < 0x2300)
//								{
//									sys_faults.flags.brush_type_error = TRUE;
//								}
//								else
//								{
//									if (INTELLIGENT_MODE == work_mode.mode)
//										sys_faults.flags.brush_type_error = FALSE;
//									else if (CARPET_MODE == work_mode.mode)
//										sys_faults.flags.brush_type_error = FALSE;									
//								}									
//						
//								adc_channel = ADC_CHANNEL7;
//								break;								
//						
//							case ADC_CHANNEL7:       //VBUS��Դ��ѹ����
//								//����ص�ѹ  21V 0x23D2,22V 0x2584, 23V 0x2728,24V 0x28FB,25V 0x2ABB
//								if(AD_AverageValue <= 8260)  //8260 19V ��ѹ���ͣ������ر��������ܿ�������ѹ����֮ǰ����һ��ʱ��ĵ͵�����ʾ
//								//if(AD_AverageValue <= 0x2580)   //for test
//								{
//										low_bat_flag = 1;
//								}
//								else
//								{

//								}

//								adc_channel = ADC_CHANNEL9;
//								break;

////							case ADC_CHANNEL8:       //WM ˮλ��⣬��ˮͰ�Ƿ�ȱˮ
////								if (AD_AverageValue >= 10866)
////								{
////									//ȱˮʱ��������ʾ�����е����������������ֱ������һ��
////									//no_water_flag = 1;
////									if (1 == run_flag && 1 == no_water_display)
////									{
////										//lcd_display_from_spiflash(3);
////										no_water_display = 0;
////									}										
////								}
////								else
////									no_water_flag = 0;
////								
////								adc_channel = ADC_CHANNEL9;
////								break;
//														
//							case ADC_CHANNEL9:       //IR_R ������գ����۳̶ȼ��
//								if (ir.tx_en)
//								{
//									ir.adc_cnt++;      
//                  //���ڵ�               0x575D 0x53A0 	
//									//��ɫ��ĭֽ���ֵ�ס   0x507F 20607;0x5162
//									//��ɫ��ĭֽ������ڵ� 0x34E3 13539
//									//��ɫ��ĭֽ 0x12BA
//									if (AD_AverageValue < 0x3500)       //ֵԽС��Խ��;����  
//										ir.very_dirty_cnt++;
//									else if (AD_AverageValue < 0x5100)  //ֵԽС��Խ��;����
//										ir.dirty_cnt++;
//									if (ir.adc_cnt >= 10)
//									{
//										if (ir.very_dirty_cnt >= 3)
//										{
//												 ir.dirty_value = VERY_DIRTY; //��ʾ����
//										}											
//										else if (ir.dirty_cnt >= 6)
//										{
//										     ir.dirty_value = DIRTY;      //��ʾ����
//										}
//										else  
//										{
//												 ir.dirty_value = NO_DIRTY;   //����
//										}
//										ir.adc_cnt = 0;
//										ir.dirty_cnt = 0;
//										ir.very_dirty_cnt = 0;
//									}									
//								}								
//								adc_channel = ADC_CHANNEL6;
//								break;
//			
//							default:
//								break;
//						}					
//				}	
//				adc_init(adc_channel);
//        //Adc_ClrSglIrqState();
//        //ADC_StartManual();
//	  }
//}

#define T4 *((volatile unsigned long *)((((uint32_t) IOC_DAT) & 0xF0000000)+0x2000000+((((uint32_t)IOC_DAT) &0xFFFFF)<<5)+(11<<2)))  //IOC11 T4
uint8_t f_test = 0;

void voltage_sample(void)
{
		//uint8_t arr_index = 0;	
	
    if(adc_is_done()) 
		{
//				f_test = 1 - f_test;//������������������150us
//  		  T4 = f_test;
				ADC_ClearManualDoneFlag();
				adc_result = (uint16_t)ADC_GetADCConversionValue(adc_channel); //ʵ�ʵ�ѹֵvol_ch3 = (adc_data - 49.588) / 7088.91575;
					
				//adcͨ����������
				switch(adc_channel)
				{
					case ADC_CHANNEL6:       //��ˢ�������
						if (adc_result >= 11252)  //11445��4A���� 11252��3A����, 10866��1A����,11059(����ֵ):2A //20210929 ԭֵ10000�������󱣻�,�޸� 10400�ᱣ�� ���蹦��С����תʱ�䳤���׻�
							adc[ADC_CH6].comp_cnt++;

						adc[ADC_CH6].adc_cnt++;
						if (adc[ADC_CH6].adc_cnt >= 100)
						{
							if (adc[ADC_CH6].comp_cnt >= 15) //100�μ�� ��15�ι���
								sys_faults.flags.brush_motor_oc = TRUE;  //�ù�ˢ���������־
							else
								sys_faults.flags.brush_motor_oc = FALSE;
							
							adc[ADC_CH6].adc_cnt = 0;
							adc[ADC_CH6].comp_cnt = 0;
						}																
					
						adc_channel = ADC_CHANNEL11;
						break;
					
					case ADC_CHANNEL11:       //ˮ�ù������
						if (adc_result >= 11252)  //11445��4A���� 11252��3A����  10866��1A����
							adc[ADC_CH11].comp_cnt++;

						adc[ADC_CH11].adc_cnt++;
						if (adc[ADC_CH11].adc_cnt >= 100)
						{
							if (adc[ADC_CH11].comp_cnt >= 15) //100�μ�� ��15�ι���
								sys_faults.flags.pump_motor_oc = TRUE;  //�õ�ˢ���������־
							else
								sys_faults.flags.pump_motor_oc = FALSE;
							
							adc[ADC_CH11].adc_cnt = 0;
							adc[ADC_CH11].comp_cnt = 0;
						}
						
						adc_channel = ADC_CHANNEL4;
						break;								
						
					case ADC_CHANNEL4:       //��ˢ��� 20211027 wwb add
						//װ��ˢ0x13C4 δװ��ˢ0x21B3   װ0x26EA
//								if (AD_AverageValue > 0x2000)
//									brush_type = 0;      //0 δװ��ˢ; 1 ����ģʽ��ˢ; 2 ��̺ģʽ��ˢ
//								else if (AD_AverageValue > 0x1000)
//									brush_type = 1;
						//����ˢ�ˣ�װ0x26EA δװ0x20F9
					  adc[ADC_CH4].adc_value[adc[ADC_CH4].adc_cnt] = adc_result;
					  adc[ADC_CH4].adc_cnt++;
					  if (adc[ADC_CH4].adc_cnt >= 4)
						{
							AD_AverageValue = ADSortBuf(adc[ADC_CH4].adc_value);
							adc[ADC_CH4].adc_cnt = 0;
						}

						if (AD_AverageValue < 0x2300)
						{
							sys_faults.flags.brush_type_error = TRUE;
						}
						else
						{
							if (INTELLIGENT_MODE == work_mode.mode)
								sys_faults.flags.brush_type_error = FALSE;
							else if (CARPET_MODE == work_mode.mode)
								sys_faults.flags.brush_type_error = FALSE;									
						}									
				
						adc_channel = ADC_CHANNEL8;
						break;								
				
					case ADC_CHANNEL8:       //���ˮ����
					  adc[ADC_CH8].adc_value[adc[ADC_CH8].adc_cnt] = adc_result;
					  adc[ADC_CH8].adc_cnt++;
					  if (adc[ADC_CH8].adc_cnt >= 4)
						{
							AD_AverageValue = ADSortBuf(adc[ADC_CH8].adc_value);
							adc[ADC_CH8].adc_cnt = 0;
						}

						adc_channel = ADC_CHANNEL9;
						break;
												
					case ADC_CHANNEL9:       //IR_R ������գ����۳̶ȼ��
						if (ir.tx_en)
						{
              adc[ADC_CH9].adc_cnt++;							
							//���ڵ�               0x575D 0x53A0 	
							//��ɫ��ĭֽ���ֵ�ס   0x507F 20607;0x5162
							//��ɫ��ĭֽ������ڵ� 0x34E3 13539
							//��ɫ��ĭֽ 0x12BA
							if (adc_result < 0x3500)       //ֵԽС��Խ��;����  
								adc[ADC_CH9].comp_cnt++;
							else if (AD_AverageValue < 0x5100)  //ֵԽС��Խ��;����
								adc[ADC_CH9].comp2_cnt++;
							if (adc[ADC_CH9].adc_cnt >= 20)
							{
								if (adc[ADC_CH9].comp_cnt >= 3)
								{
										 ir.dirty_value = VERY_DIRTY; //��ʾ����
								}											
								else if (adc[ADC_CH9].comp2_cnt >= 6)
								{
										 ir.dirty_value = DIRTY;      //��ʾ����
								}
								else  
								{
										 ir.dirty_value = NO_DIRTY;   //����
								}
								adc[ADC_CH9].adc_cnt = 0;
								adc[ADC_CH9].comp_cnt = 0;
								adc[ADC_CH9].comp2_cnt = 0;
							}									
						}								
						adc_channel = ADC_CHANNEL6;
						break;
	
					default:
						break;
				}					
	
				adc_init(adc_channel);
        //Adc_ClrSglIrqState();
        //ADC_StartManual();
	  }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

