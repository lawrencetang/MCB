/*
uart1.c

预留串口
*/
#include "stm32f10x_usart.h"
#include "uart1.h"
#include "string.h"

u8 g_TestBuf[TEST_BUF_LEN];
u16 g_TestCnt;

void uart1Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	GPIO_InitStructure.GPIO_Pin = UART1_TX_PIN;//lt:PA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//lt:复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN;//lt:PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//lt:上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//lt:使能usart1的接收数据区不空中断,中断已在其他文件中配置

	USART_Cmd(USART1, ENABLE);

	USART_ClearFlag(USART1, USART_FLAG_TC);
	
}


u8 uart1SendStr(u8* buf, u8 len)
{
	return len;
}

u8 uart1SendByte(u8 data)
{
	USART_SendData(USART1, data);//lt:发送的数据有9位,置TXE位
	return 0;
}

void uart1Isr(void)
{
	u8 data = 0;
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE))    //lt:过载错误
	{
		 USART_ReceiveData(USART1);
	}

    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE))	//接收
	{
		data = USART_ReceiveData(USART1);//lt:清RXNE标志位
		uart1SendByte(data);		
	}

	if(USART_GetFlagStatus(USART1, USART_FLAG_TXE))	//发送//lt:发送寄存器空
	{ 
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);//lt:失能发送缓冲区空中断
	}
}//lt:usart中断函数


void uart1SendU16(u16 data)
{
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData(USART1, data/16);//lt:右移8位，发送高字节	
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData(USART1, data%16);//lt:发送低字节
}



