/*
temControl.h

*/


#ifndef _TEMPERATURE_APP_H
#define _TEMPERATURE_APP_H

#include "ucos_ii.h"
#include "stm32f10x_gpio.h"
#include "frame.h"
#include "temDriver.h"
#include "pcComm.h"

/* 设备固有参数 */
#define GAIN 					6.36  	//放大器的增益


#define TEM_TOTAL_TIME 			100		//一个波形的周期，单位是ms


#define TEM_MAX_TEMP 			100.0 	//目标温度最大值
#define TEM_MIN_TEMP 			10.0 	//目标温度最小值

/* 
温度异常判断
每 TEMP_CYCLE_CTRL 个 PWM 波检查一次温度情况
离目标温度正负 TEMP_VALID 以外，两次检查的温度差在正负 TEMP_VALID_ERR 以内，认为温度没有变化，即温度异常
 */
#define TEM_CYCLE_CTRL 			1200 	//加热多少个PWM波形之后，温度没有变化，认为是异常
#define TEM_DIS_DIF				5.0 	//离目标温度多少度以内允许长时间不变，否则会认为是异常
#define TEM_VALID_ERR 			1.0 	//离目标温度较远时，每次检查必须的温度变化值

#define TEM_PID_VALID 			0.2  	//在目标温度一定范围内才启动PID算法，否则PID算法无效

#define KP 	2.5
#define KI  0.05
#define KD  0.01

/*恒温控制结构体*/
typedef struct _TEM_CTRL
{
	/* 用于记录是否开启恒温控制，并记录当前的状态 */
	u8 start;    /* 启停，0 表示关闭恒温，1 表示启动恒温 */
	u8 status;   /* 状态，0 表示未进行恒温状态，1 表示已进入恒温状态，当未启动恒温时参数没有意义 */

	/* 用于记录启动恒温后的各种参数，用于PID算法，当未启动恒温时参数没有意义 */
	float dt;   /* 目标温度 */
	float e0;   /* 目标温度与当前温度的差 */
	float e1;   /* 上次控制时目标温度与上次温度的差 */
	float es;   /* 积分项 */

	u8 ad_channel; /* 恒温系统对应的AD通道号，这个在初始化时确定，之后不允许改动 */
	OS_EVENT *Event; 	  	
}TEM_CTRL;

extern TEM_CTRL g_tem[TEM_NUM];


/* 恒温控制应用层初始化 */
u8 temAppInit(void);

/*
清除所有温度控制参数
*/
u8 temAppClear(u8 p_id);

/* 
* 获取当前温度
* 输入通道，输出实际温度值，实际温度值是由计算出来的温度再加上误差 
*/
float temAppGetTem(u8 p_id);

/*
* 根据目标温度与当前温度计算加热制冷时间
* 总时间为：TEM_TOTAL_TIME
* 当目标温度大于当前温度时，返回正数，当目标温度小于当前温度时，返回负数
*/
s16 temGetHighTime(u8 p_id);


/*
* 加热制冷控制
* 在每 TEM_TOTAL_TIME 时间内进行闭环控制，导通的时间由PID算法计算得出 
*/
u8 temHeatCool(u8 p_id);


/*
* 温控指令处理程序
* 处理上位机下发的温度控制指令
*/
u8 temCMDDeal(pcFrame frame);


/*
* 恒温控制任务函数
*/
void temTaskDeal(u8 p_id);

#endif

