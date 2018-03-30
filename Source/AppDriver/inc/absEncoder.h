
/*
absEncoder.h
绝对值编码器
*/

#ifndef _ABS_ENCODER_H
#define _ABS_ENCODER_H

#include "stm32f10x_type.h"

/* 编码器数量 */
#define ABS_ENC_NUM 		2


/* 绝对值编码器 */
typedef struct _absEncoder
{
	u16 address; 					/* EEPROM中存放逻辑零点的地址 */
	float absAngle;   		/* 编码器绝对角度值 */
	float logZero;
	float logAngle;
}absEncoder;

extern absEncoder g_absEnc[ABS_ENC_NUM];

/*
* 绝对值编码器初始化
*/
void absEncInit(void);


/*
* 读当前角度
*/
float absEncGetAngle(u8 num);


/*
* 更新角度
* 实际上调用该函数更新角度时，得到的是上一次更新的结果，因为读角度操作需要等待，为了不等待，直接取数据。
*/
void absEncUpdateAngle(u8 num);


/*
* 写逻辑零点
*/
u8 absEncWriteZero(u8 num, float angle);


/*
* 读逻辑零点
*/
float absEncReadZero(u8 num);


/*
* 设置当前角度为逻辑零点
* 先更新角度，延时一段时间后将逻辑零点设置为当前的位置，并写入EEPROM中。
*/
void absEncSetLogicZero(u8 num);

/*
* 获取当前逻辑零点的位置
*/
float absEncGetLogicZero(u8 num);

#endif






