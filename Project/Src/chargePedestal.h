#ifndef __CHARGE_PEDESTAL_H__
#define __CHARGE_PEDESTAL_H__

#include "main.h"
#include "string.h"

#define CHARGE_PEDESTAL_FRAME_HEAD        0xaa
#define CHARGE_PEDESTAL_SEND_DATA_MAX_LEN 32
#define CHARGE_PEDESTAL_PACKET_LEN        12
#define CHARGE_PEDESTAL_FIX_LEN           1


typedef struct chargePedestaltxPacket {
		uint16_t dataPacketLen;
		uint16_t reserve;
		uint8_t dataPacket[CHARGE_PEDESTAL_PACKET_LEN];
}chargePedestaltxPacket_t;
extern chargePedestaltxPacket_t chargePedestalData;

typedef struct chargePedestalrxPacket{
		uint8_t index;
		uint8_t V8530ReceiveFromChargePedestalData[CHARGE_PEDESTAL_SEND_DATA_MAX_LEN];
}chargePedestalrxPacket_t;
extern chargePedestalrxPacket_t V8530ReceiveFromChargePedestalData;

void v8530StartSendDataToChargePedestal(uint8_t *payload, uint8_t payloadLen);
void V8530SendDataToChargePedestal(void);
void V8530ReceiveDataFromChargePedestal(uint8_t receiveData);

void testChargePedestalUartSend(void);

#endif