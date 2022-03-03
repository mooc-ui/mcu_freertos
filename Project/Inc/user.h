/**
  * @file    user.h
  * @author  xqiaotech
  * @version V0.0.1
  * @date    2021-05-12
  * @brief   Main program head.
******************************************************************************/

#ifndef __USER_H
#define __USER_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

/* Includes ------------------------------------------------------------------*/
void Task0_Process(void);
void Task1_Process(void);
void Task2_Process(void);
void Task3_Process(void);
void Delay_mS(unsigned int ms);

void high_temprature_led_light(void);
 
 /*****************************************************************************
 **\brief TASK  STATEMENT
 ******************************************************************************/
#define   TASK_NUM_MAX   4           // 4 TASKS

#define   TASK0_CYCLE_VALUE      10  // Base time 1mS * 10
#define   TASK1_CYCLE_VALUE      1   // Base time 1mS * 5 20210820 wwb modify 5->1
#define   TASK2_CYCLE_VALUE      100

#define   FALSE             0
#define   TRUE              1

typedef enum
{
	TURN_ON,
	TURN_OFF,
} VOICE_INDEX_ENUM;

typedef enum
{
	PIC_INTELLIGENT,     //智能模式
	PIC_CARPET,          //地毯模式	
	PIC_AUTO_CLEAN,      //自清洁模式
	PIC_ANIMATION = 16,  //动画起始索引，50张图片?
	
	PIC_BATTERY = 60,    //电量显示图片,共100张?
	
	PIC_DIRTY = 161,     //脏污程度显示，共100张?
} PIC_INDEX_ENUM;

typedef enum
{
	NO_DIRTY,
	DIRTY,
	VERY_DIRTY,
} DIRTY_ENUM;

typedef struct
{
	uint16_t motor_speed_int_cnt;
  uint16_t motor_speed_timer_cnt;
	uint16_t motor_speed;
  uint8_t check_cnt;
	uint8_t full_cnt;
} WSX_FULL_CHECK_T;
extern WSX_FULL_CHECK_T wsx_full_check;

typedef struct
{
	uint8_t mode;
	uint8_t step;
	uint16_t query_timeout_cnt;
	uint32_t time_cnt;
	
} WORK_MODE_T;
extern WORK_MODE_T work_mode;

typedef struct
{
	uint8_t tx_en;
	uint8_t dirty_value;
} IR_T;
extern IR_T ir;


void voice_play(VOICE_INDEX_ENUM index);
void picture_display(PIC_INDEX_ENUM index);
void user_data_init(void);


#ifdef __cplusplus
}
#endif     

#endif /* __USER_H */

/*********************************** END OF FILE ******************************/
