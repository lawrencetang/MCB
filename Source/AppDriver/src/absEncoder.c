/*
absEncoder.c
����ֵ������Ӧ�ò�
����ֵ������ʹ��SPI�ӿڣ������Ҫ�ǵ���ε���SPI�����ݣ��������ݽ��д���
*/


#include "absEncoder.h"
#include "spi1.h"
#include "spi2.h"
#include "eeprom.h"
#include "protocal.h"
#include "ucos_ii.h"


absEncoder g_absEnc[ABS_ENC_NUM];//lt:ABS_ENC_NUM=2

/*
* ����ֵ��������ʼ��
* ��ʼ��֮ǰһ��Ҫ�ȳ�ʼ��EEPROM��SPI1��SPI2
*/
void absEncInit(void)
{
	u8 data[2];
	u8 i;

	SPI1Init(); 	/* ��ʼ��SPI1 */
	SPI2Init(); 	/* ��ʼ��SPI2 */

	g_absEnc[0].address = EEPROM_ADDR_ABSENC1;//lt:����ֵ������1�߼����洢��ַ
	g_absEnc[1].address = EEPROM_ADDR_ABSENC2;//lt:����ֵ������2�߼����洢��ַ

	for(i = 0; i < ABS_ENC_NUM; i++)
	{	
		iicRead(g_absEnc[i].address, (u8*)&data[0], 2);//lt:�������ֽڵ����ݵ�buffer
		g_absEnc[i].logZero = ((data[0]<<8) + data[1]) / 10.0;//lt:��ü�ǿ��ת��
		if(g_absEnc[i].logZero > 360)
		{
			g_absEnc[i].logZero = 0;
			data[0] = 0;
			data[1] = 0;
			iicWrite(g_absEnc[i].address, (u8*)&data[0], 2);
		}
		absEncUpdateAngle(i);
	}
}


/*
* ����ǰ�Ƕ�
*/
float absEncGetAngle(u8 num)
{
	if(num >= ABS_ENC_NUM)
	{
		return -1000;
	}
	return g_absEnc[num].logAngle;
}


/*
* ���½Ƕ�
* ʵ���ϵ��øú������½Ƕ�ʱ���õ�������һ�θ��µĽ������Ϊ���ǶȲ�����Ҫ�ȴ���Ϊ�˲��ȴ���ֱ��ȡ���ݡ�
*/
void absEncUpdateAngle(u8 num)
{
	u16 data;
	switch(num)
	{
	case 0:
		if(SPI1Send(0xa55a))  //lt:���ݿ��Է����򲻽�����ж�
		{
			return ;
		}
		data = g_spi1RxData;
		break;
	case 1:
		if(SPI2Send(0xa55a))
		{
			return ;
		}
		data = g_spi2RxData;
		break;
	default:
		return;
	}
	g_absEnc[num].absAngle = ((data >> 5) & 0x3FF) * 360.0 / 1024;  	/* ����ֵ��������ֵתΪ�Ƕ�ֵ *///lt:���ݴӸ�λ��ʼ�洢
	if(g_absEnc[num].absAngle > g_absEnc[num].logZero)
	{
		g_absEnc[num].logAngle = g_absEnc[num].absAngle - g_absEnc[num].logZero;
	}
	else if(g_absEnc[num].absAngle < g_absEnc[num].logZero)
	{
		g_absEnc[num].logAngle = 360.0 + g_absEnc[num].absAngle - g_absEnc[num].logZero;
	}
	else
	{
		g_absEnc[num].logAngle = 0.0;
	}		
}

/*
* ���õ�ǰ�Ƕ�Ϊ�߼����
* �ȸ��½Ƕȣ���ʱһ��ʱ����߼��������Ϊ��ǰ��λ�ã���д��EEPROM�С�
*/
void absEncSetLogicZero(u8 num)
{
	u8 data[2];
	u32 angle;
	absEncUpdateAngle(num);
	OSTimeDly(5);
	g_absEnc[num].logZero = g_absEnc[num].absAngle;
	angle = (u32)(g_absEnc[num].logZero *10);//lt:������ת����ǿ��ת��Ϊȡ��ȥ��С�����֣�*10���ٽ���ǿ��ת���ɱ���С�����һλ��ȡ��ʱ��/10���л�ԭ
	data[0] = (angle>>8)&0xff;//lt:data0���λ
	data[1] = angle&0xff;
	iicWrite(g_absEnc[num].address, (u8*)&data[0], 2);
}


/*
* ��ȡ��ǰ�߼�����λ��
*/
float absEncGetLogicZero(u8 num)
{
	return g_absEnc[num].logZero;
}




