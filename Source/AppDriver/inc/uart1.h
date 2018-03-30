/*
uart1.h

*/


#ifndef _UART1_H
#define _UART1_H

#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"

#define UART1_TX_PIN 	GPIO_Pin_9      	/* PA9  */	
#define UART1_RX_PIN 	GPIO_Pin_10      	/* PA10 */	


#define UART1_FIFO_LEN 		7

#define TEST_BUF_LEN 		64

/**********************************************************/
/********************* 对外接口函数 ***********************/
/**********************************************************/
void uart1Init(void);
u8 uart1SendByte(u8 data);
u8 uart1SendStr(u8* buf, u8 len);
void uart1Isr(void);
void uart1SendU16(u16 data);


#endif
