#ifndef _BATTERY_H_
#define _BATTERY_H_
#include "main.h"
#include "string.h"

#define BATTERY_UART_FRAME_HEAD        0x55
#define BATTERY_UART_SEND_DATA_MAX_LEN 16
#define BATTERY_UART_PACKET_LEN        16
#define BATTERY_UART_FIX_LEN           1


typedef struct batteryTxPacket {
		uint16_t dataPacketLen;
		uint8_t cmd;
		uint8_t reserve;
		uint8_t dataPacket[BATTERY_UART_PACKET_LEN];
}batteryTxPacket_t;
extern batteryTxPacket_t batteryData;


typedef struct batteryRxPacket{
		uint8_t index;
		uint8_t V8530ReceiveFromBatteryData[BATTERY_UART_SEND_DATA_MAX_LEN];
}batteryRxPacket_t;

void v8530StartSendDataToBattery(uint8_t *payload, uint8_t payloadLen);
void V8530SendDataToBattery(void);
void V8530ReceiveFromBattery(uint8_t receiveData);

void testbatteryUartSend(void);

#endif