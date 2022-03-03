
#include "uart.h"
#include "target_isr.h"
#include "user.h"
#include "string.h"

#include "main.h"
#include "v_stdio.h"
#include <stdbool.h>
#include "dwinScreenDriver.h"
#include "chargePedestal.h"
#include "battery.h"

const uint8_t CONNECT_QUERY_BUF[UART1_TX_LEN] = {0x55, 0x55,0x01, 0xAB};
const uint8_t DRY_START_BUF[UART1_TX_LEN] = {0x55, 0x55,0x02, 0xAC};
const uint8_t DRY_STOP_BUF[UART1_TX_LEN] = {0x55, 0x55,0x03, 0xAD};

const uint8_t READ_INFO_BUF[UART2_TX_LEN] = {0x55, 0x01,0x00, 0x56};

UART_T uart[UART_NUM];

/**
  * @brief  This function handles UART0 interrupt request.
  * @param  None
  * @retval None
  */
void UART0_IRQHandler(void)
{
	
#if 1
	uint8_t receiveDataTemp = 0;
  if(UART_GetINTStatus(UART0, UART_INTSTS_TXDONE) != 0)  //发送中断
	{
		UART_ClearINTStatus(UART0, UART_INTSTS_TXDONE); 
		V8530SendDataToDWINScreen();	
  }

	if(UART_GetINTStatus(UART0, UART_INTSTS_RX) != 0)  //接收中断
	{
		UART_ClearINTStatus(UART0, UART_INTSTS_RX);
			
		receiveDataTemp = UART_ReceiveData(UART0);
		V8530ReceiveDWINScreenData(receiveDataTemp);
		
  }		
	
#endif	
	
#if 0
	
  if(UART_GetINTStatus(UART0, UART_INTSTS_TXDONE) != 0)  //发送中断
	{
		UART_ClearINTStatus(UART0, UART_INTSTS_TXDONE);
    if (uart[0].tx_cnt < uart[0].tx_len - 1)
		{
      UART_SendData(UART0, uart[0].tx_buf[++uart[0].tx_cnt]);
		}
    else
    {
      uart[0].tx_cnt = 0;
      //UART_INTConfig(UART0, UART_INT_TXDONE, DISABLE);
    }
  }

	if(UART_GetINTStatus(UART0, UART_INTSTS_RX) != 0)  //接收中断
	{
		UART_ClearINTStatus(UART0, UART_INTSTS_RX);
		uart[0].rx_buf[uart[0].rx_len++] = UART_ReceiveData(UART0);
		
		if(toggle){
				toggle = 1 - toggle;
				testGPIOHigh();
		}else{
				toggle = 1 - toggle;
				testGPIOLow();
		}			
  }	
	
#endif
}

/**
  * @brief  This function handles UART1 interrupt request.
  * @param  None
  * @retval None
  */


void UART1_IRQHandler(void)
{
	//static bool toggle;
	
	
	uint8_t receiveDataTemp = 0;
  if(UART_GetINTStatus(UART1, UART_INTSTS_TXDONE) != 0)  //发送中断
	{
		UART_ClearINTStatus(UART1, UART_INTSTS_TXDONE); 
		V8530SendDataToChargePedestal();
  }

	if(UART_GetINTStatus(UART1, UART_INTSTS_RX) != 0)  //接收中断
	{
		UART_ClearINTStatus(UART1, UART_INTSTS_RX);
			
		receiveDataTemp = UART_ReceiveData(UART1);
		
		//this function wait to done
		V8530ReceiveDataFromChargePedestal(receiveDataTemp);
  }	
	

#if 0	
  if(UART_GetINTStatus(UART1, UART_INTSTS_TXDONE) != 0)  //发送中断
	{
		UART_ClearINTStatus(UART1, UART_INTSTS_TXDONE);
    if (uart[1].tx_cnt < uart[1].tx_len - 1)
      UART_SendData(UART1, uart[1].tx_buf[++uart[1].tx_cnt]);
    else
    {
      uart[1].tx_cnt = 0;
      //UART_INTConfig(UART2, UART_INT_TXDONE, DISABLE);
    }
  }

	if(UART_GetINTStatus(UART1, UART_INTSTS_RX) != 0)  //接收中断
	{
		UART_ClearINTStatus(UART1, UART_INTSTS_RX);
		uart[1].rx_buf[uart[1].rx_len++] = UART_ReceiveData(UART1);
		
		if(toggle){
				toggle = 1 - toggle;
				testGPIOHigh();
		}else{
				toggle = 1 - toggle;
				testGPIOLow();
		}			
  }
#endif

}

/**
  * @brief  This function handles UART2 interrupt request.
  * @param  None
  * @retval None
  */
void UART2_IRQHandler(void)
{
	uint8_t receiveDataTemp = 0;
  if(UART_GetINTStatus(UART2, UART_INTSTS_TXDONE) != 0)  //发送中断
	{
		UART_ClearINTStatus(UART2, UART_INTSTS_TXDONE); 
		V8530SendDataToBattery();
  }

	if(UART_GetINTStatus(UART2, UART_INTSTS_RX) != 0)  //接收中断
	{
		UART_ClearINTStatus(UART2, UART_INTSTS_RX);
			
		receiveDataTemp = UART_ReceiveData(UART2);
		
		//this function wait to done
		V8530ReceiveFromBattery(receiveDataTemp);
  }		


#if 0	
  if(UART_GetINTStatus(UART2, UART_INTSTS_TXDONE) != 0)  //发送中断
	{
		UART_ClearINTStatus(UART2, UART_INTSTS_TXDONE);
    if (uart[2].tx_cnt < uart[2].tx_len - 1)
      UART_SendData(UART2, uart[2].tx_buf[++uart[2].tx_cnt]);
    else
    {
      uart[2].tx_cnt = 0;
      //UART_INTConfig(UART2, UART_INT_TXDONE, DISABLE);
    }
  }

	if(UART_GetINTStatus(UART2, UART_INTSTS_RX) != 0)  //接收中断
	{
		UART_ClearINTStatus(UART2, UART_INTSTS_RX);
		uart[2].rx_buf[uart[2].rx_len++] = UART_ReceiveData(UART2);
		
		if(toggle){
				toggle = 1 - toggle;
				testGPIOHigh();
		}else{
				toggle = 1 - toggle;
				testGPIOLow();
		}			
  } 
	
#endif	
}

/**
  * @brief  This function handles UART3 interrupt request.
  * @param  None
  * @retval None
  */
void UART3_IRQHandler(void)  //电机控制串口
{
//  /* Transmit handler */
//  if (UART_GetINTStatus(UART3, UART_INTSTS_TXDONE))
//  {
//    UART_ClearINTStatus(UART3, UART_INTSTS_TXDONE);
//    if (motor_TxCnt < motor_TxTotal)
//    {
//      UART_SendData(UART3, motor_TxBuff[motor_TxCnt++]);
//    }
//    else
//    {
//      UART_INTConfig(UART3, UART_INT_TXDONE, DISABLE);
//    }
//  }
//  
//  /* Receive handler */
//  if (UART_GetINTStatus(UART3, UART_INTSTS_RX))
//  {
//    UART_ClearINTStatus(UART3, UART_INTSTS_RX);
//    if (motor_RxCnt < motor_TxTotal)
//    {
//      motor_RxBuff[motor_RxCnt++] = UART_ReceiveData(UART3);
//    }
//    if (motor_RxCnt == motor_TxTotal)
//    {
//      UART_INTConfig(UART3, UART_INT_RX, DISABLE);
//    } 
//  }		
}

/**
  * @brief  This function handles UART4 interrupt request.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
}

/**
  * @brief  This function handles UART5 interrupt request.
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void)
{
  if(UART_GetINTStatus(UART5, UART_INTSTS_TXDONE) != 0)  //发送中断
	{
		UART_ClearINTStatus(UART5, UART_INTSTS_TXDONE);
    if (uart[3].tx_cnt < uart[3].tx_len - 1)
      UART_SendData(UART5, uart[3].tx_buf[++uart[3].tx_cnt]);
    else
    {
      uart[3].tx_cnt = 0;
      //UART_INTConfig(UART5, UART_INT_TXDONE, DISABLE);
    }
  }

	if(UART_GetINTStatus(UART5, UART_INTSTS_RX) != 0)  //接收中断
	{
		UART_ClearINTStatus(UART5, UART_INTSTS_RX);
		uart[3].rx_buf[uart[3].rx_len++] = UART_ReceiveData(UART5);
  } 
}

uint8_t calc_sum(uint8_t *buf, uint8_t len)
{
	uint8_t i = 0;
	uint8_t sum = 0;
	
	for (i = 0; i < len; i++)
		sum += buf[i];
	
	return sum;
}

void uart1_send_cmd(UART1_CMD_ENUM cmd)
{
	switch(cmd)
	{
		case CONNECT_QUERY:
			memcpy(uart[1].tx_buf, CONNECT_QUERY_BUF, UART1_TX_LEN);
		
			break;
		
		case DRY_START:
			memcpy(uart[1].tx_buf, DRY_START_BUF, UART1_TX_LEN);
		
			break;
		
		case DRY_STOP:
			memcpy(uart[1].tx_buf, DRY_STOP_BUF, UART1_TX_LEN);			
		
			break;
		
		default:
			break;		
	}

	uart[1].rx_len = 0;
	uart[1].tx_cnt = 0;
	uart[1].tx_len = UART1_TX_LEN;
	uart[1].tx_times = 0;
	uart[1].flags.b.comm_start = TRUE;
}

void uart2_send_cmd(UART2_CMD_ENUM cmd)
{
	switch(cmd)
	{
		case READ_INFO:
			memcpy(uart[2].tx_buf, READ_INFO_BUF, UART2_TX_LEN);		
			break;
		
		default:
			break;		
	}

	uart[2].rx_len = 0;
	uart[2].tx_cnt = 0;
	uart[2].tx_len = UART2_TX_LEN;
	uart[2].tx_times = 0;
	uart[2].flags.b.comm_start = TRUE;
}

uint8_t uart0_rx_check(void)
{
	
		return 0;
}

uint8_t uart1_rx_check(void)
{
		if (0x55 == uart[1].rx_buf[0] && 0x55 == uart[1].rx_buf[1] && (uart[1].rx_buf[2] | 0x80) == uart[1].rx_buf[2]
		 && calc_sum(uart[1].rx_buf, 3) == uart[1].rx_buf[3])//数据包检查
			return 1;
    
		return 0;
}

//电池通信串口
uint8_t uart2_rx_check(void)
{
	  uint8_t data_len = uart[2].rx_buf[2];
	  uint8_t sum = calc_sum(uart[2].rx_buf, data_len + 3);
	
		return (uart[2].rx_buf[data_len + 3] == sum) ? 1 : 0;
}

uint8_t uart3_rx_check(void)
{
  
		return 0;
}

void uarts_data_init(void)
{
	memset(uart, 0, sizeof(uart));
	
	uart[0].uart_ch = UART0;    //显示屏、语音通信串口
	uart[1].uart_ch = UART1;    //底座通信串口
	uart[2].uart_ch = UART2;    //电池通信串口
	uart[3].uart_ch = UART5;    //wifi通信串口	
	
	uart[0].rx_check = uart0_rx_check;
	uart[1].rx_check = uart1_rx_check;
	uart[2].rx_check = uart2_rx_check;
	uart[3].rx_check = uart3_rx_check;
}

//串口通信统一处理
void uarts_comm_proc(void)
{
	uint8_t i = 0;
	for (i = 0; i < UART_NUM; i++)
	{
		if (uart[i].flags.b.comm_start)
		{
			if (uart[i].rx_len > 0 && ++uart[i].rx_timeout_cnt >= 2)//100ms 原值3
			{
				uart[i].rx_timeout_cnt = 0;
				uart[i].flags.b.rx_ok = uart[i].rx_check();
				if (uart[i].flags.b.rx_ok)
				{
					uart[i].flags.b.rx_done = TRUE;
					uart[i].flags.b.comm_start = FALSE;
				}
			}
			
			if (0 == uart[i].tx_times || (uart[i].tx_times < UART_SEND_RETRY_TIMES && !uart[i].flags.b.rx_ok))
			{
				uart[i].rx_len = 0;
				UART_SendData(uart[i].uart_ch, uart[i].tx_buf[0]); //启动发送中断
				if (++uart[i].tx_times >= UART_SEND_RETRY_TIMES)
				{
						uart[i].flags.b.rx_done = TRUE;
						uart[i].flags.b.comm_start = FALSE;	
				}					
			}			
		}
	}
}

/*********************************** END OF FILE ******************************/
