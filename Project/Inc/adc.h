/******************************************************************************/
/** adc.h
 **
 ** Device define
 **
 **   - 2020-07-23
 **
 *****************************************************************************/
#ifndef  __ADC_H__
#define  __ADC_H__

#include "target.h"
#include <stdint.h>
#include <stdbool.h>

#define ADC_CH_NUM 5

#define  ADC_SAMPLE_CHANNEL      7


typedef struct sampleAdcSelect_t{
		uint32_t channel;
		int16_t  getChannelAdcValue;
		int16_t  reserve;
}sampleAdcSelect;

//typedef struct sampleAdcValue{
//		int16_t vbusAdcValue;//ADC_CHANNEL7
//		int16_t electrolyticWaterAdcValue;//ADC_CHANNEL8
//		int16_t infraredAdcValue;//ADC_CHANNEL9
//		int16_t waterPumpdAdcValue;//ADC_CHANNEL11
//		int16_t chargeAdcValue;//ADC_CHANNEL3
//		int16_t floorBrushAdcValue;//ADC_CHANNEL4
//}sampleAdcValue_t;
extern sampleAdcSelect qAdcResultValue;


typedef enum
{
	ADC_CH6, 
	ADC_CH11,
	ADC_CH4,  
	ADC_CH8, 
	ADC_CH9,
} ADC_CH_ENUM;

typedef struct
{
	uint8_t adc_cnt;
	uint16_t adc_value[4];
  uint8_t comp_cnt;
  uint8_t comp2_cnt;
} ADC_T;
extern ADC_T adc[ADC_CH_NUM];


void adc_init(uint32_t Channel);

void startADCSample(uint32_t adcChannelSelect);
void adcTestFunction(void);

#endif /* __ADC_H__ */
/*******************************************************************************
 * EOF (not truncated)                                                        
 ******************************************************************************/
