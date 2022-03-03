/******************************************************************************/
/** motor_ctrl.h
 **
 ** Device define
 **
 **   - 2021-12-20
 **
 *****************************************************************************/

#ifndef  __MOTOR_CTRL_H__
#define  __MOTOR_CTRL_H__

#include "target.h"
#include <stdint.h>


#define   MOTOR_CTRL_NUM    3

typedef enum
{
	MAIN_MOTOR,
	PUMP_MOTOR,
	BRUSH_MOTOR,
} motor_type_enum;

typedef enum
{
	INTELLIGENT_MODE,
	CARPET_MODE,
	AUTO_CLEAN_MODE = 3,
} mode_enum;

typedef struct 
{
	uint8_t en;
	uint8_t running;
	void (*pwm_set_duty)(uint16_t duty);
	void (*start)(void);
	void (*stop)(void);
	uint16_t stop_delay; 
} motor_ctrl_t;
extern motor_ctrl_t motor_ctrl[MOTOR_CTRL_NUM];

typedef union
{
		uint8_t val;
	  struct
		{
			uint8_t upright:1;           //直立
			uint8_t bottom_connected:1;  //底座连接通信状态
		}flags;
} sys_status_t;
extern sys_status_t sys_status;

typedef union
{
		uint16_t val;
		struct
		{
			uint16_t wsx_uninstall:1;     //污水箱未安装标志
			uint16_t wsx_full:1;          //污水箱满标志
			uint16_t brush_type_error:1;  //滚刷类型错误
			uint16_t no_water:1;          //缺水标志
			uint16_t pump_motor_oc:1;     //水泵电机过流
			uint16_t brush_motor_oc:1;    //滚刷电机过流
		}flags;
} sys_faults_t;
extern sys_faults_t sys_faults;

typedef struct
{
	uint16_t brush_err_notify_cnt;
	uint16_t upright_check_cnt;
	uint16_t wsx_uninstall_check_cnt;
	uint16_t wsx_uninstall_notify_cnt;
	uint16_t wsx_full_notify_cnt;	
	uint16_t bottom_unconnect_notify_cnt;	
} sys_time_t;
extern sys_time_t sys_time;

void motor_ctrl_init(void);
void motor_proc(void);


#endif /* __MOTOR_CTRL_H__ */

/*******************************************************************************
 * EOF (not truncated)                                                        
 ******************************************************************************/
