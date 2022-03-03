#ifndef _DWIN_SCREEN_DRIVER_H
#define _DWIN_SCREEN_DRIVER_H

#include <stdbool.h>
#include "v_stdio.h"

#define FRAME_HEADER1 				0x5A
#define FRAME_HEADER2 				0xA5
#define QSPORT_MAX_DATA_SIZE  32
#define MCU_MAX_SEND_DATA_LEN 32

/* 0X5a   0xa5   dataLength */
/* 1byte  1byte  1byte      */
#define FIX_BYTE_LENGTH       3



/*communication Data structure*/
typedef struct
{
	uint8_t	frameHead1;
	uint8_t	frameHead2;
	uint8_t validDataLen;
	uint8_t reserve;
	uint8_t mcuData[QSPORT_MAX_DATA_SIZE];
}qsportdata_t;



void V8530SendDataToDWINScreen(void);
void V8530ReceiveDWINScreenData(uint8_t receiveData);
void v8530StartSendData(uint8_t *payload, uint8_t payloadLen);

void testUartSend(void);//test function

#endif