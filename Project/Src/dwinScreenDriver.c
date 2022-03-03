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

#include "dwinScreenDriver.h"
#include "main.h"
#include "string.h"

#include "lib_uart.h"
																			 
enum
{
	waitForStartByte1,
	waitForStartByte2,
	waitForDataLength,
	waitForData,
	//waitForChksum1,
}qSportRxState;																			 

static qsportdata_t rxPacket;
static qsportdata_t txPacket;


void V8530SendDataToDWINScreen(void)
{
		static uint8_t txDataIndex=0;
		if(txPacket.validDataLen+FIX_BYTE_LENGTH > QSPORT_MAX_DATA_SIZE){
				printf("V8530 send data is too long\n");
				return;
		}

		if(txDataIndex<txPacket.validDataLen){
				UART_SendData(UART0, txPacket.mcuData[txDataIndex++]);
		}else{
				txDataIndex = 0;
		}
}

/*
*v8530StartSendData ������ V8530SendDataToDWINScreen����������ʹ�õ�
*v8530StartSendData���������һ�δ���uart0���жϣ������͵�һ���ֽ�
*������ֽڶ�����V8530SendDataToDWINScreen�������ж������
*
*payloadLen�����ĳ���һ���ǳ�ȥA5���ܳ���
*
*ʾ������:
*void testUartSend(void)
*{
*	#define DATALEN 6
*	uint8_t testScreenData[DATALEN] = {0xA5, 0x04, 0x83, 0x00, 0x0F, 0x01};
*
*	txPacket.validDataLen = DATALEN;
*	memcpy(txPacket.mcuData, testScreenData, DATALEN);
*	UART_SendData(UART0, 0x5A);//trigger uart0 interrupt start send data
*
*}
*
*/
void v8530StartSendData(uint8_t *payload, uint8_t payloadLen)
{
	if(payloadLen>=QSPORT_MAX_DATA_SIZE-1){
			printf("payload too much\n");
			return;
	}
	
	/*
	*��Ϊ�����Ƭ����uart0����������Ҫ����һ���ж�,���ڴ�����ͨ������5a ������һ��uart0�ж�
	*���������ж����滹��Ҫ���͵����ݾ���5a�����������,�ҰѺ���Ҫ���͵����ݶ�װ��
	* txPacket.mcuData��������������ж����淢��
	*/
	txPacket.validDataLen = payloadLen;
	memcpy(txPacket.mcuData, payload, payloadLen);
	UART_SendData(UART0, 0x5A);//trigger uart0 interrupt start send data
}

//���Ҫ��ֲ�ⲿ�ֵĴ�������testUartSend���к�������
void testUartSend(void)
{
	//uint8_t testScreenData[6] = {0xA5, 0x04, 0x83, 0x00, 0x0F, 0x01};
	static uint8_t testnum = 0;
	uint8_t testScreenData[9] = {0xA5, 0x07, 0x82, 0x00, 0x84, 0x5a, 0x01, 0x00, 0x03};
	testScreenData[8] = testnum%5;
	testnum++;
	v8530StartSendData(testScreenData, 9);
}


void V8530ReceiveDWINScreenData(uint8_t receiveData)
{
		uint8_t uart0ReceiveData= 0;
		static uint8_t dataIndex = 0;
		static uint8_t rxState = waitForStartByte1;
		
		uart0ReceiveData = receiveData;
		switch(rxState)
		{
			case waitForStartByte1:
					if(FRAME_HEADER1 == uart0ReceiveData){
							rxState = waitForStartByte2;
					}else{
							rxState = waitForStartByte1;//reset state
					}
				break;

			case waitForStartByte2:
					if(FRAME_HEADER2 == uart0ReceiveData){
							rxState = waitForDataLength;
					}else{
							rxState = waitForStartByte1;//reset state
					}
				break;					
					
			case waitForDataLength:
					if(uart0ReceiveData <= MCU_MAX_SEND_DATA_LEN){
							rxState = waitForData;
							dataIndex = 0;
							rxPacket.validDataLen = uart0ReceiveData;
					}else{
							rxState = waitForStartByte1;
					}
			
				break;
					
			case waitForData:
						rxPacket.mcuData[dataIndex++] = uart0ReceiveData;
						if(dataIndex == rxPacket.validDataLen){//receive complete
								rxState = waitForStartByte1;
								printf("data %02x %02x %02x %02x\n",rxPacket.validDataLen,rxPacket.mcuData[0],rxPacket.mcuData[1],rxPacket.mcuData[2]);
						}else{
								rxState = waitForData;
						}				
			
				break;		
			
			default:
				break;
		}
}




