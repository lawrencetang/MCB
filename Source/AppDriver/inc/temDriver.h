/*
temDrive.h

恒温系统底层驱动实现
主要用于定义加热、制冷引脚，实现加热与制冷控制。

*/


#ifndef _TEMPERATURE_DRIVER_H
#define _TEMPERATURE_DRIVER_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_type.h"


#define TEM_NUM 	1 	/* 温控总路数 */

enum TEM_X{ TEM1 = 0};



/* 温控1控制引脚 */
#define TEM_GPIOX 			GPIOD
#define TEM_PIN_C1 			GPIO_Pin_7 		//PD7
#define TEM_PIN_C2 			GPIO_Pin_6 		//PD6
#define TEM_PIN_SD 			GPIO_Pin_5 		//PD5

/* IO输出控制结构体 */
typedef struct _TEM_IO_CTRL
{
	u16 C1;
	u16 C2;
	u16 SD;
}TEM_IO_CTRL;


extern TEM_IO_CTRL g_tem_io[TEM_NUM];


/* 
初始化
将所有温度控制引脚设置为OUT_PP模式
*/
void temPinInit(void);

/* 
加热
根据输入的参数使对应的温控加热
p_id：六路温控中的一路，取值0-5，其他值将错误
返回值：返回0 则正常，1表示输入的温控ID号有问题。
*/
u8 temHeat(u8 p_id);

/* 
制冷
根据输入的参数使对应的温控制冷
p_id：六路温控中的一路，取值0-5，其他值将错误
返回值：返回0 则正常，1表示输入的温控ID号有问题。
*/
u8 temCool(u8 p_id);


/* 
停止
根据输入的参数使对应的温控停止
p_id：六路温控中的一路，取值0-5，其他值将错误
返回值：返回0 则正常，1表示输入的温控ID号有问题。
*/
u8 temStop(u8 p_id);


#endif


