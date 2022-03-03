/**
  * @file    user.h
  * @author  xqiaotech
  * @version V0.0.1
  * @date    2021-05-12
  * @brief   Main program head.
******************************************************************************/

#ifndef __UART_H
#define __UART_H

#include "target.h"

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

#define    UART_NUM                   4
#define    UART1_TX_LEN               4
#define    UART_SEND_RETRY_TIMES      2

#define    UART2_TX_LEN               4

typedef union
{
		uint8_t val;
	  struct
		{
			uint8_t comm_start:1;//串口通信开始
			uint8_t tx_done:1;
			uint8_t rx_wait:1;  //接收等待中
			uint8_t rx_done:1;  //接收完成
			uint8_t rx_ok:1;    //接收数据是否正确
		} b;
} UART_FLAGS;

typedef struct
{
	UART_TypeDef *uart_ch;
	uint8_t (*rx_check)(void);
	uint8_t tx_buf[16];
	uint8_t tx_len;
	uint8_t tx_cnt;
	uint8_t tx_times;
	uint8_t tx_interval;//发送间隔
	uint8_t rx_buf[16];
	uint8_t rx_len;
	uint8_t rx_timeout_cnt;	
	UART_FLAGS flags;
} UART_T;
extern UART_T uart[UART_NUM];

typedef enum 
{
	CONNECT_QUERY,
	DRY_START,
	DRY_STOP,
} UART1_CMD_ENUM;

typedef enum 
{
	READ_INFO,
} UART2_CMD_ENUM;

void uarts_data_init(void);
void uart1_send_cmd(UART1_CMD_ENUM cmd);
void uart2_send_cmd(UART2_CMD_ENUM cmd);
void uarts_comm_proc(void);

#ifdef __cplusplus
}
#endif     

#endif /* __UART_H */

/*********************************** END OF FILE ******************************/
