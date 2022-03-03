#include "motor_ctrl.h"
#include "user.h"
#include "string.h"

void main_motor_set_duty(uint16_t duty)
{
	//PWM3->CCR0 = 1024;                 //频率8K	
	PWM3->CCR1 = duty;
  
  PWM_ClearCounter(PWM3);
}

void main_motor_start(void)
{
	PWM_OutputCmd(PWM3, PWM_CHANNEL_1, ENABLE);
}

void main_motor_stop(void)
{
	PWM_OutputCmd(PWM3, PWM_CHANNEL_1, DISABLE);
}

void pump_motor_start(void)
{
	PWM_OutputCmd(PWM1, PWM_CHANNEL_1, ENABLE);
}

void pump_motor_stop(void)
{
	PWM_OutputCmd(PWM1, PWM_CHANNEL_1, DISABLE);
}

void brush_motor_start(void)
{
	PWM_OutputCmd(PWM0, PWM_CHANNEL_2, ENABLE);
}

void brush_motor_stop(void)
{
	PWM_OutputCmd(PWM0, PWM_CHANNEL_2, DISABLE);
}

void motor_ctrl_init(void)
{
	memset(motor_ctrl, 0, sizeof(motor_ctrl));
	motor_ctrl[MAIN_MOTOR].start = main_motor_start;
	motor_ctrl[MAIN_MOTOR].stop = main_motor_stop;
	motor_ctrl[PUMP_MOTOR].start = pump_motor_start;
	motor_ctrl[PUMP_MOTOR].stop = pump_motor_stop;
	motor_ctrl[BRUSH_MOTOR].start = brush_motor_start;
	motor_ctrl[BRUSH_MOTOR].stop = brush_motor_stop;
}

void motor_proc(void)
{
	uint8_t i = 0;

	for (i = 0; i < MOTOR_CTRL_NUM; i++)
	{		
		if (sys_faults.val)
		{
			if (motor_ctrl[i].en && motor_ctrl[i].running)
			{
				motor_ctrl[i].stop();
				motor_ctrl[i].en = DISABLE;
				motor_ctrl[i].running = FALSE;			
			}
		}
		else 
		{
			if (motor_ctrl[i].en && !motor_ctrl[i].running)
			{
				motor_ctrl[i].start();
				motor_ctrl[i].en = ENABLE;
				motor_ctrl[i].running = TRUE;			
			}		
		}
		
		//延时停电机控制
		if (!motor_ctrl[i].en && motor_ctrl[i].running)
		{
			if (motor_ctrl[i].stop_delay)
			{
				motor_ctrl[i].stop_delay--;
				if (0 == motor_ctrl[i].stop_delay)
				{
					motor_ctrl[i].stop();
					motor_ctrl[i].running = FALSE;
				}
			}
		}
	}	
}
