/*******************************************************************************
 **Company       : xiaoqiao TECHNOLOGY LTD.
 **Project Name	 : C9
 **File:         : highSpeedMotor.c
 **Created by  	 : tianwanjie
 **Created Date	 : 2022.02.28
 **Version:      : 
 **Target  		   : 
 **HardWare		   : 
 **SofeWare      : 
 ******************************************************************************/

#include "chargePedestal.h"
#include "lib_uart.h"


chargePedestaltxPacket_t chargePedestalData = {0};
chargePedestalrxPacket_t V8530ReceiveFromChargePedestalData = {0};

void V8530SendDataToChargePedestal(void)
{
		static uint8_t txDataIndex=0;
		if(chargePedestalData.dataPacketLen+CHARGE_PEDESTAL_FIX_LEN > CHARGE_PEDESTAL_SEND_DATA_MAX_LEN){
				printf("V8530 send data to charge pedestal is too long\n");
				return;
		}

		if(txDataIndex<chargePedestalData.dataPacketLen){
				UART_SendData(UART1, chargePedestalData.dataPacket[txDataIndex++]);
		}else{
				txDataIndex = 0;
		}
}

void v8530StartSendDataToChargePedestal(uint8_t *payload, uint8_t payloadLen)
{
	if(payloadLen>=CHARGE_PEDESTAL_SEND_DATA_MAX_LEN-1){
			printf("charge pedestal payload too much\n");
			return;
	}
	
	/*
	*��Ϊ�����Ƭ����uart1����������Ҫ����һ���ж�,���ڴ�����ͨ������5a ������һ��uart0�ж�
	*���������ж����滹��Ҫ���͵����ݾ���5a�����������,�ҰѺ���Ҫ���͵����ݶ�װ��
	* chargePedestalData.dataPacket�����������  ���ж����淢��
	*/
	chargePedestalData.dataPacketLen = payloadLen;
	memcpy(chargePedestalData.dataPacket , payload, payloadLen);	
	
	UART_SendData(UART1, CHARGE_PEDESTAL_FRAME_HEAD);//trigger uart1 interrupt start send data
}

void V8530ReceiveDataFromChargePedestal(uint8_t receiveData)
{
//		V8530ReceiveFromChargePedestalData.V8530ReceiveFromChargePedestalData[V8530ReceiveFromChargePedestalData.index++] = receiveData;
//		if(V8530ReceiveFromChargePedestalData.index >= ){
//				V8530ReceiveFromChargePedestalData.index = 0;
//		}
}


/*
*bauterate:4800
*/
void testChargePedestalUartSend(void)
{
	static uint8_t testnum = 0;
	uint8_t testScreenData[9] = {0xA5, 0x07, 0x82, 0x00, 0x84, 0x5a, 0x01, 0x00, 0x03};
	testScreenData[8] = testnum%5;
	testnum++;
	v8530StartSendDataToChargePedestal(testScreenData, 9);
}


