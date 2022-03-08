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


/* ���������� */
static TaskHandle_t AppTaskCreate_Handle;
/* LED ������ */
static TaskHandle_t LED_Task_Handle;

static StackType_t AppTaskCreate_Stack[128];
/* LED �����ջ */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate ������ƿ� */     //ͨ�����ǳ����������ƿ�Ϊ��������֤
static StaticTask_t AppTaskCreate_TCB;
/* AppTaskCreate ������ƿ� */
static StaticTask_t LED_Task_TCB;

/* �������������ջ */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* ��ʱ�������ջ */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
/* ����������ƿ� */
static StaticTask_t Idle_Task_TCB;
/* ��ʱ��������ƿ� */
static StaticTask_t Timer_Task_TCB;

static void AppTaskCreate(void);/* ���ڴ������� */
static void LED_Task(void* pvParameters);/* LED_Task ����ʵ�� */


/**
**********************************************************************
* @brief ��ȡ��������������ջ��������ƿ��ڴ�
* ppxTimerTaskTCBBuffer : ������ƿ��ڴ�
* ppxTimerTaskStackBuffer : �����ջ�ڴ�
* pulTimerTaskStackSize : �����ջ��С
* @author fire
* @version V1.0
* @date 2018-xx-xx
**********************************************************************
*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t    *pulIdleTaskStackSize)
{
			*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* ������ƿ��ڴ� */
			*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* �����ջ�ڴ� */
			*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* �����ջ��С */
}


/**
*********************************************************************
* @brief ��ȡ��ʱ������������ջ��������ƿ��ڴ�
* ppxTimerTaskTCBBuffer : ������ƿ��ڴ�
* ppxTimerTaskStackBuffer : �����ջ�ڴ�
* pulTimerTaskStackSize : �����ջ��С
* @author fire
* @version V1.0
* @date 2018-xx-xx
**********************************************************************
*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
																		StackType_t **ppxTimerTaskStackBuffer,
																		uint32_t *pulTimerTaskStackSize)
{
		*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* ������ƿ��ڴ� */
		*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* �����ջ�ڴ� */
		*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* �����ջ��С */
}

static void LED_Task(void* parameter)
{
	while (1) {
		testGPIOLow();
		vTaskDelay(500); /* ��ʱ 500 �� tick */
		SEGGER_RTT_printf(0, "led1_task running,LED1_OFF\r\n");
		//printf("led1_task running,LED1_OFF\r\n");
		testGPIOHigh();
		vTaskDelay(500); /* ��ʱ 500 �� tick */
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
			vTaskDelay(500); /* ��ʱ 500 �� tick */
			SEGGER_RTT_printf(0, "led1_task running,LED1_OFF\r\n");
			//printf("led1_task running,LED1_OFF\r\n");
			testGPIOHigh();
			vTaskDelay(500); /* ��ʱ 500 �� tick */
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
		taskENTER_CRITICAL(); //�����ٽ���
		/* ���� LED_Task ���� */
		xTaskCreateStatic((TaskFunction_t )LED_Task, (const char*)"LED_Task", 
																				(uint32_t)128, //
																				(void* )NULL, 
																				(UBaseType_t)1, 
																				(StackType_t*),//
																				(StaticTask_t*)&);//
#endif
																				
#if 1		
		test_Task_Handle = xTaskCreateStatic((TaskFunction_t )test_Task,
																				(const char*)"test_Task",//��������
																				(uint32_t)128, //
																				(void* )NULL, 
																				(UBaseType_t)1, 
																				(StackType_t*)test_Task_Stack,//
																				(StaticTask_t*)&led_Task_TCB);//
		if (NULL == test_Task_Handle){
		    printf("test_Task_Handle create falied\n");
		}		
#endif
		
		uartTaskHandle = xTaskCreateStatic((TaskFunction_t )uartTask, //������		
																				(const char*)"test_Task",//��������
																				(uint32_t)128, //�����ջ��С
																				(void* )NULL, //���ݸ��������Ĳ���
																				(UBaseType_t)4, //�������ȼ�
																				(StackType_t*)uartTaskStack,//�����ջ
																				(StaticTask_t*)&uartTaskTCB);//������ƿ�
		if (NULL == uartTaskHandle){
		    printf("test_Task create falied\n");
		}		

		sensorTaskHandle = xTaskCreateStatic((TaskFunction_t )sensorTask, //������		
																				(const char*)"test_Task",//��������
																				(uint32_t)128, //�����ջ��С
																				(void* )NULL, //���ݸ��������Ĳ���
																				(UBaseType_t)2, //�������ȼ�
																				(StackType_t*)sensorTaskStack,//�����ջ
																				(StaticTask_t*)&sensorTaskTCB);//������ƿ�
		if (NULL == sensorTaskHandle){
		    printf("test_Task create falied\n");
		}		
		
		messageTaskHandle = xTaskCreateStatic((TaskFunction_t )messageProcessTask, //������		
																				(const char*)"messageTask",//��������
																				(uint32_t)128, //�����ջ��С
																				(void* )NULL, //���ݸ��������Ĳ���
																				(UBaseType_t)3, //�������ȼ�
																				(StackType_t*)messageTaskStack,//�����ջ
																				(StaticTask_t*)&messageTaskTCB);//������ƿ�
	
		
		vTaskDelete(AppTaskCreate_Handle); //ɾ�� AppTaskCreate ����
		taskEXIT_CRITICAL(); //�˳��ٽ���
}

//static void AppTaskCreate(void)
//{
//		//taskENTER_CRITICAL(); //�����ٽ���
//		/* ���� LED_Task ���� */
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
//		//taskEXIT_CRITICAL(); //�˳��ٽ���
//	
//		vTaskStartScheduler(); 
//}


void testFreeRTOS(void)
{
		/* ���� AppTaskCreate ���� */
		AppTaskCreate_Handle = xTaskCreateStatic( (TaskFunction_t )AppTaskCreate,
																							(const char* )"AppTaskCreate",//��������
																							(uint32_t )128, //�����ջ��С
																							(void* )NULL,//���ݸ��������Ĳ���
																							(UBaseType_t )3, //�������ȼ�
																							(StackType_t* )AppTaskCreate_Stack,
																							(StaticTask_t* )&AppTaskCreate_TCB);
		if (NULL != AppTaskCreate_Handle) /* �����ɹ� */
			
		vTaskStartScheduler(); /* �������񣬿������� */
}

//void testFreeRTOS(void)
//{
//		AppTaskCreate();
//}




