/**
  * @file    v_stdio.h
  * @author  Application Team
  * @version V4.3.0
  * @date    2018-09-04
  * @brief   standard printf.
******************************************************************************/

#include "user.h"
#include "uart.h"
#include "string.h"


uint16_t  TaskTimer[TASK_NUM_MAX];
uint8_t audio_play_buf[10] = {0x5A,0xA5,0x07,0x82,0x00,0xA0,0x10,0x01,0x40,0x00};
WSX_FULL_CHECK_T wsx_full_check;

/*****************************************************************************
 * Function:      void ResetTask0(void)
 * Description:
 *
 * Caveats:
 *
 *****************************************************************************/
void ResetTask0(void)
{
	  TaskTimer[0] = 10;   
}

/*****************************************************************************
 * Function:    void Init_Task_Time(void)
 * Description: 
 *
 * Caveats:     Allocate execution time for each task
 *
 *****************************************************************************/
void Task_InitTime(void)
{
	  TaskTimer[0] = 10;                  
	  TaskTimer[1] = 1;                   
		TaskTimer[2] = 10; 
}

//”Ô“Ù≤•∑≈
void voice_play(VOICE_INDEX_ENUM index)
{
	  if (!uart[0].flags.b.comm_start)
		{
			audio_play_buf[6] = index;
			memcpy(uart[0].tx_buf, audio_play_buf, sizeof(audio_play_buf));
			uart[0].flags.b.comm_start = TRUE;
		} 
}

//Õº∆¨œ‘ æ
void picture_display(PIC_INDEX_ENUM index)
{
	  if (!uart[0].flags.b.comm_start)
		{
			audio_play_buf[6] = index;
			memcpy(uart[0].tx_buf, audio_play_buf, sizeof(audio_play_buf));
			uart[0].flags.b.comm_start = TRUE;
		} 
}

void user_data_init(void)
{
	memset(&wsx_full_check, 0, sizeof(WSX_FULL_CHECK_T));
	memset(&work_mode, 0, sizeof(WORK_MODE_T));
}

/*********************************** END OF FILE ******************************/
