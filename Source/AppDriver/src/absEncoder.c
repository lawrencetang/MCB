/*
absEncoder.c
绝对值编码器应用层
绝对值编码器使用SPI接口，因此主要是的如何调用SPI读数据，并对数据进行处理。
*/


#include "absEncoder.h"
#include "spi1.h"
#include "spi2.h"
#include "eeprom.h"
#include "protocal.h"
#include "ucos_ii.h"


absEncoder g_absEnc[ABS_ENC_NUM];//lt:ABS_ENC_NUM=2

/*
* 绝对值编码器初始化
* 初始化之前一定要先初始化EEPROM、SPI1、SPI2
*/
void absEncInit(void)
{
	u8 data[2];
	u8 i;

	SPI1Init(); 	/* 初始化SPI1 */
	SPI2Init(); 	/* 初始化SPI2 */

	g_absEnc[0].address = EEPROM_ADDR_ABSENC1;//lt:绝对值编码器1逻辑零点存储地址
	g_absEnc[1].address = EEPROM_ADDR_ABSENC2;//lt:绝对值编码器2逻辑零点存储地址

	for(i = 0; i < ABS_ENC_NUM; i++)
	{	
		iicRead(g_absEnc[i].address, (u8*)&data[0], 2);//lt:读两个字节的数据到buffer
		g_absEnc[i].logZero = ((data[0]<<8) + data[1]) / 10.0;//lt:最好加强制转换
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
* 读当前角度
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
* 更新角度
* 实际上调用该函数更新角度时，得到的是上一次更新的结果，因为读角度操作需要等待，为了不等待，直接取数据。
*/
void absEncUpdateAngle(u8 num)
{
	u16 data;
	switch(num)
	{
	case 0:
		if(SPI1Send(0xa55a))  //lt:数据可以发送则不进入该判断
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
	g_absEnc[num].absAngle = ((data >> 5) & 0x3FF) * 360.0 / 1024;  	/* 绝对值编码器的值转为角度值 *///lt:数据从高位开始存储
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
* 设置当前角度为逻辑零点
* 先更新角度，延时一段时间后将逻辑零点设置为当前的位置，并写入EEPROM中。
*/
void absEncSetLogicZero(u8 num)
{
	u8 data[2];
	u32 angle;
	absEncUpdateAngle(num);
	OSTimeDly(5);
	g_absEnc[num].logZero = g_absEnc[num].absAngle;
	angle = (u32)(g_absEnc[num].logZero *10);//lt:浮点型转整形强制转化为取整去掉小数部分，*10后再进行强制转换可保留小数点后一位，取回时再/10进行还原
	data[0] = (angle>>8)&0xff;//lt:data0存高位
	data[1] = angle&0xff;
	iicWrite(g_absEnc[num].address, (u8*)&data[0], 2);
}


/*
* 获取当前逻辑零点的位置
*/
float absEncGetLogicZero(u8 num)
{
	return g_absEnc[num].logZero;
}




