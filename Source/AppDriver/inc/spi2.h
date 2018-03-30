
/*
spi1.h
*/

#ifndef _SPI2_H
#define _SPI2_H

#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"


#define SPI2_GPIO 			GPIOB
#define SPI2_PIN_CS   		GPIO_Pin_12      	/* PB12 */
#define SPI2_PIN_SCK   		GPIO_Pin_13      	/* PB13 */
#define SPI2_PIN_MISO   	GPIO_Pin_14      	/* PB14 */
#define SPI2_PIN_MOSI   	GPIO_Pin_15      	/* PB15 */


#define SPI2_CS_HIGH 		GPIO_SetBits(SPI2_GPIO, SPI2_PIN_CS)
#define SPI2_CS_LOW 		GPIO_ResetBits(SPI2_GPIO, SPI2_PIN_CS)


extern u16 g_spi2RxData;


/*
* SPI2��ʼ��
* ��ʼ��Ϊ�жϷ���
*/
void SPI2Init(void);


/*
*  SPI2����
*  д�����ݣ����򿪽����ж�
*/
u8 SPI2Send(u16 data);


/*
*  SPI2�ж�
*  ֻ�з�������ʱ�Ż�����жϣ����ж�����ѽ��յ�����ȡ����
*/
void SPI2Isr(void);


#endif

