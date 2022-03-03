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

#define  ADC_CH_NUM      5

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

#endif /* __ADC_H__ */
/*******************************************************************************
 * EOF (not truncated)                                                        
 ******************************************************************************/
