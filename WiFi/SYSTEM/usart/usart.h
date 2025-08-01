#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "system.h" 


	
void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void uart3_init(u32 bound);
void UART1_send_byte(char data);
void UART2_send_byte(char data);
void Uart1_SendStr(char*SendBuf);

#endif
