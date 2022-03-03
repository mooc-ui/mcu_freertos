#include "esp8266.h"
#include <stdio.h>
#include <stdint.h>
#include <lib_uart.h>
#include <target_isr.h>
#include <string.h>
#include <delay.h>

uint8_t error_cnt=0;
char *strx;

void Clear_Buffer(void)   //Çå¿Õ»º´æ
{
    uint8_t i;
    for(i=0;i<Rxcouter;i++)
    RxBuffer[i]=0;//»º´æ
    Rxcouter=0;
}

void  esp8266_Init(void)
{
	printf("AT\r\n"); 
	delay_ms(500);
	strx=strstr((const char*)RxBuffer,(const char*)"OK");//·µ»ØOK
	while(strx==NULL)
	{
		//Clear_Buffer();	
		error_cnt++;
		if(error_cnt>50)
		{
			
		}
		printf("AT\r\n"); 
		delay_ms(500);
		strx=strstr((const char*)RxBuffer,(const char*)"OK");//·µ»ØOK
	}
	error_cnt=0;
}
