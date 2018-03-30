
/*
eeprom.h
*/

#ifndef _EEPROM_H
#define _EEPROM_H

#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"
#include "frame.h"

#define IIC_SDA_PIN   GPIO_Pin_7      /* PB7 */
#define IIC_SCL_PIN   GPIO_Pin_6      /* PB6 */

#define EEPROM_READ      0xA3     /* 1010 0011 */ 
#define EEPROM_WRITE     0xA2     /* 1010 0010 */


/***************** EEPROM��ַ���� *****************/
#define EEPROM_ADDR_ABSENC1 	0x0100 		/* ����ֵ������1�߼����洢��ַ */
#define EEPROM_ADDR_ABSENC2 	0x0200 		/* ����ֵ������2�߼����洢��ַ */
#define EEPROM_ADDR_ENCRYPT 	0x0300 		/* �������Ĵ洢��ַ */

void iicStart(void);

void iicStop(void);

void iicAck(void);

void iicWriteByte(u8 a);

u8 iicReadByte(void);

void iicWrite(u16 add, u8 *buffer, u16 len);

u16 iicRead(u16 add, u8 *buffer, u16 len);

void iicInit(void);


#endif





