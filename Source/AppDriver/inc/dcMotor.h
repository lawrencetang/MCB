/*
dcMotor.h
用TIMER5作为直流电机的PWM输出控制
*/

#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include "stm32f10x_type.h"
#include "stm32f10x_gpio.h"
#include "frame.h"



#define DCM_NUM 				2  		/* 直流电机的数量 */


#define DCM_CNT_NUM 			10000	/* 直流电机PWM周期，单位为1个定时tick，一般设置定时器周期为1us */


#define DCM_RATE_NUM 			3 	  	/* 直流电机速度等级个数 */
/* 直流电机PWM输出占空比 */
#define DCM_RATE1 				1	  	/* 百分之 */
#define DCM_RATE2 				10
#define DCM_RATE3 				20


#define DCM1_PIN_OUT 	GPIO_Pin_2 		/* PA2，TIM5_CH3  直流电机1输出控制 */
#define DCM2_PIN_OUT 	GPIO_Pin_3 		/* PA3，TIM5_CH4  直流电机2输出控制 */

#define TCHSW_PIN_IN 	GPIO_Pin_1 		/* PB1，直流电机2的触发控制 */


#define DCM1_ON 	GPIO_SetBits(GPIOA, DCM1_PIN_OUT)
#define DCM1_OFF 	GPIO_ResetBits(GPIOA, DCM1_PIN_OUT)

#define DCM2_ON 	GPIO_SetBits(GPIOA, DCM2_PIN_OUT)
#define DCM2_OFF 	GPIO_ResetBits(GPIOA, DCM2_PIN_OUT)

enum {DCM1 = 0, DCM2 = 1};


/* 步进电机IO输出控制结构体 */
typedef struct _DCMotor
{
	u8 sts; 	/* 直流电机状态：0 停止，1 运行 */
	u8 rate; 	/* 直流电机PWM输出的占空比 0 ~ 100 之间的一个数值 */
}DCMotor;

extern DCMotor g_dcm[DCM_NUM];



/*
* 直流电机初始化
*/
void dcmInit(void);

/*
* 直流电机命令解析
* 解析上位机发下来的指令，操作直流电机。
*/
u8 dcmSetCMD(pcFrame frame);

/*
* 启动直流电机
* 启动PWM波输出，把定时器对应通道的PWM比较值设置为相应的占空比参数
*/
void dcmStartDCM(u8 num);

/*
* 停止直流电机
* 停止PWM波输出，定时器对应通道的PWM比较值设置为0，即一直输出低电平
*/
void dcmStopDCM(u8 num);

/*
* 轻触开关初始化
* 轻触开关用于直流电机2的手动控制，每按一下直流电机2状态就改变
* 初始化电机控制引脚为外部中断脚
*/
void tchSWInit(void);

/*
* 轻触开关中断服务函数
* 用于控制直流电机2的启停，每按一下状态反转
*/
void tchSWIsr(void);

#endif



