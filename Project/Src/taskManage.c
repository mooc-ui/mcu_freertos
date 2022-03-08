/*******************************************************************************
 **Company       : xiaoqiao TECHNOLOGY LTD.
 **Project Name	 : C9
 **File:         : taskMannage.c
 **Created by  	 : tianwanjie
 **Created Date	 : 2022.03.08
 **Version:      : 
 **Target  		   : 
 **HardWare		   : 
 **SofeWare      : 
 ******************************************************************************/
 
#include "taskManage.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "SEGGER_RTT.h"

extern void testGPIOHigh(void);
extern void testGPIOLow(void);


/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle;
/* LED 任务句柄 */
static TaskHandle_t LED_Task_Handle;

static StackType_t AppTaskCreate_Stack[128];
/* LED 任务堆栈 */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate 任务控制块 */     //通常我们称这个任务控制块为任务的身份证
static StaticTask_t AppTaskCreate_TCB;
/* AppTaskCreate 任务控制块 */
static StaticTask_t LED_Task_TCB;

/* 空闲任务任务堆栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务堆栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
/* 空闲任务控制块 */
static StaticTask_t Idle_Task_TCB;
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;

static void AppTaskCreate(void);/* 用于创建任务 */
static void LED_Task(void* pvParameters);/* LED_Task 任务实现 */


/**
**********************************************************************
* @brief 获取空闲任务的任务堆栈和任务控制块内存
* ppxTimerTaskTCBBuffer : 任务控制块内存
* ppxTimerTaskStackBuffer : 任务堆栈内存
* pulTimerTaskStackSize : 任务堆栈大小
* @author fire
* @version V1.0
* @date 2018-xx-xx
**********************************************************************
*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t    *pulIdleTaskStackSize)
{
			*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* 任务控制块内存 */
			*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* 任务堆栈内存 */
			*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* 任务堆栈大小 */
}


/**
*********************************************************************
* @brief 获取定时器任务的任务堆栈和任务控制块内存
* ppxTimerTaskTCBBuffer : 任务控制块内存
* ppxTimerTaskStackBuffer : 任务堆栈内存
* pulTimerTaskStackSize : 任务堆栈大小
* @author fire
* @version V1.0
* @date 2018-xx-xx
**********************************************************************
*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
																		StackType_t **ppxTimerTaskStackBuffer,
																		uint32_t *pulTimerTaskStackSize)
{
		*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* 任务控制块内存 */
		*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* 任务堆栈内存 */
		*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* 任务堆栈大小 */
}

static void LED_Task(void* parameter)
{
	while (1) {
		testGPIOLow();
		vTaskDelay(500); /* 延时 500 个 tick */
		SEGGER_RTT_printf(0, "led1_task running,LED1_OFF\r\n");
		//printf("led1_task running,LED1_OFF\r\n");
		testGPIOHigh();
		vTaskDelay(500); /* 延时 500 个 tick */
		SEGGER_RTT_printf(0, "led1_task running,LED1_ON\r\n");
		//printf("led1_task running,LED1_ON\r\n");
			
		
		
		//printf("led1_task running,LED1_OFF\r\n");
 }
}

#if 1
static TaskHandle_t test_Task_Handle;
static StackType_t test_Task_Stack[128];
static StaticTask_t led_Task_TCB;
void test_Task(void *p)
{
	while(1){
			testGPIOLow();
			vTaskDelay(500); /* 延时 500 个 tick */
			SEGGER_RTT_printf(0, "led1_task running,LED1_OFF\r\n");
			//printf("led1_task running,LED1_OFF\r\n");
			testGPIOHigh();
			vTaskDelay(500); /* 延时 500 个 tick */
			SEGGER_RTT_printf(0, "led1_task running,LED1_ON\r\n");
	}

}
#endif

#if 1
static TaskHandle_t uartTaskHandle;
static StackType_t uartTaskStack[128];
static StaticTask_t uartTaskTCB;
void uartTask(void *p)
{
	while(1){
			vTaskDelay(1000);
			SEGGER_RTT_printf(0, "UART task running\r\n");
	}

}
#endif

#if 1
static TaskHandle_t sensorTaskHandle;
static StackType_t sensorTaskStack[128];
static StaticTask_t sensorTaskTCB;
void sensorTask(void *p)
{
	while(1){
			vTaskDelay(1000);
			SEGGER_RTT_printf(0, "sensor task running\r\n");
	}

}
#endif

#if 1

static TaskHandle_t messageTaskHandle;
static StackType_t messageTaskStack[128];
static StaticTask_t messageTaskTCB;

static QueueHandle_t messageQueueHandle = NULL;
static void messageQueueInit(void)
{
    messageQueueHandle = xQueueCreate(MESSAGE_QUEUE_LENGTH, sizeof(messageStruct));
}

void sendMessageToTaskQueue(uint32_t eventId, void* iParam, void* pParam)
{
		messageStruct message = {0};
    message.id = eventId;
    message.iParam = iParam;
    message.pParam = pParam;
    xQueueSend(messageQueueHandle, (void *)&message, 0);
}

void sendMessageToTaskQueueFromIsr(uint32_t eventId, void* iParam, void* pParam)
{
		messageStruct message = {0};
    message.id = eventId;
    message.iParam = iParam;
    message.pParam = pParam;
    xQueueSendFromISR(messageQueueHandle, (void *)&message, 0);
}

void messageProcessTask(void *arg)
{
		messageStruct message = {0};
	
		messageQueueInit();
		while(1){
			vTaskDelay(1000);
			SEGGER_RTT_printf(0, "message task running\r\n");			
			
//				memset((void *)&message, 0, sizeof(message));
//				if(xQueueReceive(messageQueueHandle, (void *)&message,portMAX_DELAY)){
//						switch(message.id){
//							case QSPORT_SCRUBBER_KEY_EVENT:
//								break;
//							case 2:
//								break;
//							default:
//								break;
//						}
//				}
		}
}

#endif



void vApplicationIdleHook( void )
{
		//todo
		__WFI();
}


static void AppTaskCreate(void)
{
#if 0
		taskENTER_CRITICAL(); //进入临界区
		/* 创建 LED_Task 任务 */
		xTaskCreateStatic((TaskFunction_t )LED_Task, (const char*)"LED_Task", 
																				(uint32_t)128, //
																				(void* )NULL, 
																				(UBaseType_t)1, 
																				(StackType_t*),//
																				(StaticTask_t*)&);//
#endif
																				
#if 1		
		test_Task_Handle = xTaskCreateStatic((TaskFunction_t )test_Task,
																				(const char*)"test_Task",//任务名称
																				(uint32_t)128, //
																				(void* )NULL, 
																				(UBaseType_t)1, 
																				(StackType_t*)test_Task_Stack,//
																				(StaticTask_t*)&led_Task_TCB);//
		if (NULL == test_Task_Handle){
		    printf("test_Task_Handle create falied\n");
		}		
#endif
		
		uartTaskHandle = xTaskCreateStatic((TaskFunction_t )uartTask, //任务函数		
																				(const char*)"test_Task",//任务名称
																				(uint32_t)128, //任务堆栈大小
																				(void* )NULL, //传递给任务函数的参数
																				(UBaseType_t)4, //任务优先级
																				(StackType_t*)uartTaskStack,//任务堆栈
																				(StaticTask_t*)&uartTaskTCB);//任务控制块
		if (NULL == uartTaskHandle){
		    printf("test_Task create falied\n");
		}		

		sensorTaskHandle = xTaskCreateStatic((TaskFunction_t )sensorTask, //任务函数		
																				(const char*)"test_Task",//任务名称
																				(uint32_t)128, //任务堆栈大小
																				(void* )NULL, //传递给任务函数的参数
																				(UBaseType_t)2, //任务优先级
																				(StackType_t*)sensorTaskStack,//任务堆栈
																				(StaticTask_t*)&sensorTaskTCB);//任务控制块
		if (NULL == sensorTaskHandle){
		    printf("test_Task create falied\n");
		}		
		
		messageTaskHandle = xTaskCreateStatic((TaskFunction_t )messageProcessTask, //任务函数		
																				(const char*)"messageTask",//任务名称
																				(uint32_t)128, //任务堆栈大小
																				(void* )NULL, //传递给任务函数的参数
																				(UBaseType_t)3, //任务优先级
																				(StackType_t*)messageTaskStack,//任务堆栈
																				(StaticTask_t*)&messageTaskTCB);//任务控制块
	
		
		vTaskDelete(AppTaskCreate_Handle); //删除 AppTaskCreate 任务
		taskEXIT_CRITICAL(); //退出临界区
}

//static void AppTaskCreate(void)
//{
//		//taskENTER_CRITICAL(); //进入临界区
//		/* 创建 LED_Task 任务 */
//		xTaskCreate(LED_Task, "LED_Task", (uint32_t)128, 0, 5, 0);
//																					
//		xTaskCreate(test_Task, "test_Task", (uint32_t)128, 0, 5, 0);
//		
//		xTaskCreate(uartTask, "test_Task",(uint32_t)128, 0, 5, 0);


//		xTaskCreate(sensorTask, "test_Task",(uint32_t)128, 0, 5, 0);
//																				
//		
//		//xTaskCreate((TaskFunction_t )messageProcessTask, (const char*)"messageTask",(uint32_t)128, 0, 5, 0);
//		
//		//taskEXIT_CRITICAL(); //退出临界区
//	
//		vTaskStartScheduler(); 
//}


void testFreeRTOS(void)
{
		/* 创建 AppTaskCreate 任务 */
		AppTaskCreate_Handle = xTaskCreateStatic( (TaskFunction_t )AppTaskCreate,
																							(const char* )"AppTaskCreate",//任务名称
																							(uint32_t )128, //任务堆栈大小
																							(void* )NULL,//传递给任务函数的参数
																							(UBaseType_t )3, //任务优先级
																							(StackType_t* )AppTaskCreate_Stack,
																							(StaticTask_t* )&AppTaskCreate_TCB);
		if (NULL != AppTaskCreate_Handle) /* 创建成功 */
			
		vTaskStartScheduler(); /* 启动任务，开启调度 */
}

//void testFreeRTOS(void)
//{
//		AppTaskCreate();
//}




