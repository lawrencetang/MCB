/*
uart1.c

Ԥ������
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//lt:�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN;//lt:PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//lt:��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//lt:ʹ��usart1�Ľ��������������ж�,�ж����������ļ�������

	USART_Cmd(USART1, ENABLE);

	USART_ClearFlag(USART1, USART_FLAG_TC);
	
}


u8 uart1SendStr(u8* buf, u8 len)
{
	return len;
}

u8 uart1SendByte(u8 data)
{
	USART_SendData(USART1, data);//lt:���͵�������9λ,��TXEλ
	return 0;
}

void uart1Isr(void)
{
	u8 data = 0;
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE))    //lt:���ش���
	{
		 USART_ReceiveData(USART1);
	}

    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE))	//����
	{
		data = USART_ReceiveData(USART1);//lt:��RXNE��־λ
		uart1SendByte(data);		
	}

	if(USART_GetFlagStatus(USART1, USART_FLAG_TXE))	//����//lt:���ͼĴ�����
	{ 
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);//lt:ʧ�ܷ��ͻ��������ж�
	}
}//lt:usart�жϺ���


void uart1SendU16(u16 data)
{
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData(USART1, data/16);//lt:����8λ�����͸��ֽ�	
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData(USART1, data%16);//lt:���͵��ֽ�
}



