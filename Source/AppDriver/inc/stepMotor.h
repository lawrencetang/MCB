/*
2012.01.15
*/

#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H

#include "ucos_ii.h"
#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"
#include "frame.h"
#include "timer3.h"

#define SM_NUM 				2  		/* 步进电机的数量 */

#define SM_STS_IDLE 		0x00	/* 步进电机空闲 */
#define SM_STS_BUZY 		0x01	/* 步进电机繁忙 */
#define SM_STS_FAULT 		0x02	/* 步进电机故障 */
#define SM_STS_POWON 		0x03	/* 步进电机刚上电 */

/* 步进电机1用的是PE13~ED15 */
#define SM1_PIN_EN 			GPIO_Pin_13	  	/*PE13，使能控制引脚，0 使能   1 不使能*/
#define SM1_PIN_STP			GPIO_Pin_14	  	/*PE14，转动控制引脚*/
#define SM1_PIN_DIR			GPIO_Pin_15	  	/*PE15，方向控制引脚，0 正转   1 反转*/	

/* 步进电机2用的是PE8~PE10 */
#define SM2_PIN_EN 			GPIO_Pin_8	  	/*PE8，使能控制引脚，0 使能   1 不使能*/
#define SM2_PIN_STP			GPIO_Pin_9	  	/*PDE9，转动控制引脚 */
#define SM2_PIN_DIR			GPIO_Pin_10	  	/*PE10，方向控制引脚，0 正转   1 反转*/

/*ltj 接近开关信号*/
#define SM_ZS           GPIO_pin_6
#define SM_RS           GPIO_pin_14


#define SM_SETEN(num) 	GPIOE->ODR ^= g_sm[num].pin.en
#define SM_CLREN(num) 	GPIOE->ODR &= ~g_sm[num].pin.en

#define SM_REVSTP(num) 	GPIOE->ODR ^= g_sm[num].pin.stp
#define SM_CLRSTP(num) 	GPIOE->ODR &= ~g_sm[num].pin.stp

#define SM_SETDIR(num) 	g_sm[num].dir?(GPIOE->ODR |= g_sm[num].pin.dir):(GPIOE->ODR &= ~g_sm[num].pin.dir)

#define SM_STEP360   		3200.0      	/*步进电机转360度所需要的步数，根据细分数调整*///lt:步距角1.8°，16细分	

#define SM_MAX_ANGLE 		360

#define SM_ACC_TICKS 		0.005
#define SM_MAX_WAIT_TICKS 	10
#define SM_MIN_WAIT_TICKS 	2

#define SM_EQUAL_ANGLE_ERROR 	1.0 		/* 两个角度相等的最大误差，超过这个误差则认为不等 */



/*步进电机相关转换计算*/
#define SM_DegreeToPulse(d) 			((int)(SM_STEP360*d/360+0.5))  /* 度数转换为对应的电机转动步数 */ //lt:3200/360*d
#define SM_PulseToDegree(p) 			((int)(360*p/SM_STEP360+0.5))  /* 电机转动步数转换为对应的度数 */  //lt:360/3200*p



enum {SM1 = 0, SM2 = 1};




/*
* 步进电机启动和停止的速度都较慢，转动过程中速度较快。
* 每次对电机进行操作，都至少需要经过加速和减速这两个过程。
* 为了方便控制，定时器只负责单个过程的控制，不负责过程转换。
* 因此，定义了单次转动过程参数和完整的一次操作参数。
*
* 单次转动过程参数包括（单次转动没有角度概念，只有脉冲数和时间ticks）：
* 						1 sts：确定是否继续转动
* 						2 totalPulse：本次转动总脉冲数
* 						3 remainPulse：剩余脉冲数
* 						4 spdTicks：每次跳变所需要的ticks数量，即脉冲速度
* 						5 acc：每次跳变后，spdTicks的改变数量，用于控制加速、减速、匀速
* 						6 ticks：用于定时器中断的计数
*
* 一次完整的转动控制参数包括（完整转动是通过角度来控制的）
* 						1 flag：转动标志，用于指示设备状态，上位机设置1来启动转动，转动完成后清0，如果设备故障则无法启动转动
* 						2 goalAgnle：目标角度，上位机设计的目标角度，即本次转动的目标角度。
* 						3 realAngle：当前角度，指示电机当前角度。
*/



/* 步进电机IO输出控制结构体 */
typedef struct _SM_IO_PIN
{
	u16 en;
	u16 stp;
	u16 dir;
}SM_IO_PIN;


/* 步进电机一次转动过程控制参数 */
typedef struct _SM_ROTATE
{
	u8 sts; 			/* 单次转动状态指示，0 转动停止， 1 继续转动 */
	u32 totalPulse; 	/* 本次转动总脉冲 */
	u32 remainPulse; 	/* 本次转动剩余脉冲数 */
	float spdTicks; 	/* STP输出的时间，即定时器中断次数，用于控制转动速度 */	
	float acc; 			/* 转动过程中控制spdTicks变化的参数，加速时为正，减速时为负，匀速时为0 */ 
	s16 ticks; 			/* 用于实时对脉冲进行计数 */	
}SM_ROTATE;


/* 步进电机全局参数，与电机绑定 */
typedef struct _SM_PARA
{
	float accTicks;
	float maxTicks;
	float minTicks;
	OS_EVENT *event;
}SM_PARA;



/*步进电机控制参数结构体*/
typedef struct _STEP_MOTOR
{
	u8 flag; 	 			/* 电机状态，0 空闲， 1 旋转， 2 故障， 3 未初始化 */
	u8 dir;
	float goalAngle;		/* 要转动到的目标角度，由上位机设置 */
	float realAngle;		/* 当前实际角度 */
	SM_IO_PIN pin; 			/* 步进电机对应的控制引脚 */
	SM_ROTATE ro;			/* 步进电机转动控制 */
	SM_PARA para; 			/* 步进电机全局参数 */
}STEP_MOTOR;


extern STEP_MOTOR g_sm[SM_NUM];

/*
步进电机初始化，包括引脚和参数
*/
void smInit(void);



/*
*/
u8 smSetCMD(pcFrame frame);



/*
清除步进电机的所有单次转动相关的参数
*/
void smClearRotate(u8 num);


/*
启动电机转动
*/
void smStartSM(u8 num, u32 pulse, float acc);



/*
停止电机转动
*/
void smStopSM(u8 num);


/*
步进电机控制的tick
本函数通过定时器调用，要求定时器的时间间隔是20us，即每20us会进入本函数一次。
每次控制都需要经过加速和减速过程
*/
void smRotateTick(void);


/*
转动控制
*/
u8 smRotate(u8 num);



/*
步进电机自检
让电机转动10度，再转回原来的位置，判断转动是否正常，如果正常则返回成功，否则返回失败。
*/
u8 smCheck(u8 num);




/*
* 更新步进电机的角度
*/
void smUpdateAngle(u8 num);


/*
判断两个角度是否相等
如果两个角度差小于SM_EQUAL_ANGLE_ERROR，则相等，否则判断为不等
返回值：1 不等，0 相等
*/
u8 smJudgeEqualAngle(float a1, float a2);

#endif



