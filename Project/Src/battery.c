#include "battery.h"

enum
{
	batteryWaitForStartByte1,
	batteryWaitForCmd,
	batteryWaitForDataLength,
	batteryWaitForData,
	batteryWaitForChksum,
}batteryRxState;

batteryTxPacket_t batteryData = {0};


void V8530SendDataToBattery(void)
{
		static uint8_t txDataIndex=0;
		if(batteryData.dataPacketLen+BATTERY_UART_FIX_LEN > BATTERY_UART_SEND_DATA_MAX_LEN){
				printf("V8530 send data to charge pedestal is too long\n");
				return;
		}

		if(txDataIndex<batteryData.dataPacketLen){
				UART_SendData(UART2, batteryData.dataPacket[txDataIndex++]);
		}else{
				txDataIndex = 0;
		}
}

void v8530StartSendDataToBattery(uint8_t *payload, uint8_t payloadLen)
{
	if(payloadLen>=BATTERY_UART_SEND_DATA_MAX_LEN-1){
			printf("battery uart payload too much\n");
			return;
	}
	
	/*
	*��Ϊ�����Ƭ����uart1����������Ҫ����һ���ж�,���ڴ�����ͨ������5a ������һ��uart0�ж�
	*���������ж����滹��Ҫ���͵����ݾ���5a�����������,�ҰѺ���Ҫ���͵����ݶ�װ��
	* chargePedestalData.dataPacket�����������  ���ж����淢��
	*/
	batteryData.dataPacketLen = payloadLen;
	memcpy(batteryData.dataPacket , payload, payloadLen);	
	
	UART_SendData(UART2, BATTERY_UART_FRAME_HEAD);//trigger uart1 interrupt start send data
}

void V8530ReceiveFromBattery(uint8_t receiveData)
{
		uint8_t uart2ReceiveData= 0;
		static uint8_t dataIndex = 0;
		static uint8_t rxState = batteryWaitForStartByte1;
		static uint8_t chkSum = 0;
		
		uart2ReceiveData = receiveData;
		switch(rxState)
		{
			case batteryWaitForStartByte1:
					if(BATTERY_UART_FRAME_HEAD == uart2ReceiveData){
							rxState = batteryWaitForCmd;
							chkSum += uart2ReceiveData;
					}else{
							chkSum = 0;
							rxState = batteryWaitForStartByte1;//reset state
					}
				break;

			case batteryWaitForCmd:
					batteryData.cmd = uart2ReceiveData;

					rxState = batteryWaitForDataLength;
					chkSum += uart2ReceiveData;
				break;					
					
			case batteryWaitForDataLength:
					if(uart2ReceiveData <= BATTERY_UART_SEND_DATA_MAX_LEN){
							rxState = batteryWaitForData;
							dataIndex = 0;//dataIndex=0 will store cmd byte
							batteryData.dataPacketLen = uart2ReceiveData;
							chkSum += uart2ReceiveData;
					}else{
							chkSum = 0;
							rxState = batteryWaitForStartByte1;
					}
			
				break;
					
			case batteryWaitForData:
						if(dataIndex > BATTERY_UART_PACKET_LEN){
								printf("batteryData overflow\n");
								return;
						}
						batteryData.dataPacket[dataIndex++] = uart2ReceiveData;
						chkSum += uart2ReceiveData;
						
						if(dataIndex == batteryData.dataPacketLen){//receive complete
								rxState = batteryWaitForChksum;							
						}else{
								rxState = batteryWaitForData;
						}					
				break;	
						
			case batteryWaitForChksum:
					printf("Chsm %02x\n",chkSum);
					//printf("data %02x %02x %02x Chsm %02x\n",batteryData.dataPacketLen,batteryData.dataPacket[0],batteryData.dataPacket[1],chkSum);
					if(chkSum == uart2ReceiveData){
							chkSum = 0;
							rxState = batteryWaitForStartByte1;
							batteryData.dataPacket[0] = batteryData.cmd;//receive complete
							printf("data %02x %02x %02x Chsm %02x\n",batteryData.dataPacketLen,batteryData.dataPacket[0],batteryData.dataPacket[1],chkSum );
					}
					
				break;
			
			default:
				break;
		}
}

/*
*bauterate:9600
*/
void testbatteryUartSend(void)
{
	#define TEST_SCREEN_DATA_LEN 4
	uint8_t testScreenData[TEST_SCREEN_DATA_LEN] = {0x55, 0x01, 0x00, 0x56};
	v8530StartSendDataToBattery(testScreenData, sizeof(testScreenData));
}

