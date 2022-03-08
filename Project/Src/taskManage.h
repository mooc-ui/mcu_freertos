#ifndef __TASK_MANAGE_H__
#define __TASK_MANAGE_H__
#include "main.h"
#include "string.h"

typedef struct {
    uint32_t id;
    void* iParam;
    void* pParam;
} messageStruct;

//scrubber
enum {	
		QSPORT_SCRUBBER_KEY_EVENT = 1,
	
		EVENT_ID_NONE,
};

#define MESSAGE_QUEUE_LENGTH 16


extern void testFreeRTOS(void);

void sendMessageToTaskQueue(uint32_t eventId, void* iParam, void* pParam);
void sendMessageToTaskQueueFromIsr(uint32_t eventId, void* iParam, void* pParam);

#endif