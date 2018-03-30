
/*
spi1.h
*/

#ifndef _SPI1_H
#define _SPI1_H

#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"

#define SPI1_GPIO 			GPIOA
#define SPI1_PIN_CS   		GPIO_Pin_4      	/* PA4 */
#define SPI1_PIN_SCK   		GPIO_Pin_5      	/* PA5 */
#define SPI1_PIN_MISO   	GPIO_Pin_6      	/* PA6 */
#define SPI1_PIN_MOSI   	GPIO_Pin_7      	/* PA7 */


#define SPI1_CS_HIGH 		GPIO_SetBits(SPI1_GPIO, SPI1_PIN_CS)
#define SPI1_CS_LOW 		GPIO_ResetBits(SPI1_GPIO, SPI1_PIN_CS)


extern u16 g_spi1RxData;


/*
* SPI1������
* ��ʼ��Ϊ�жϷ���
*/
void SPI1Init(void);


/*
*  SPI1����
*  д�����ݣ����򿪽����ж�
*/
u8 SPI1Send(u16 data);


/*
*  SPI1�ж�
*  ֻ�з�������ʱ�Ż�����жϣ����ж�����ѽ��յ�����ȡ����
*/
void SPI1Isr(void);


#endif

