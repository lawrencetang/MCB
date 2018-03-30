

#ifndef __INC_ENC_H
#define __INC_ENC_H

#include "step_motor.h"
#include "my_protocal.h"
#include "header.h"

#define ENC_PULSE360 		2000.0 /* 编码器一圈的脉冲数，两组差分信号各1000个 */

#define ENC_DegreeToPulse(d) 	(ENC_PULSE360*d/360.0) /* 由角度转换成编码器的脉冲 */
#define ENC_PulseToDegree(p) 	(360.0*p/ENC_PULSE360) /* 由编码器的脉冲换成成角度 */

/*编码器参数结构体*/
typedef struct _INC_ENC
{
	u16 cnt;		 //定时器计数值
	s32 pulse;		 //编码器脉冲计数
}INC_ENC;
extern INC_ENC g_enc[SM_No];

/*初始化编码器*/
void ENC_InitENC(u8 num);

/*更新编码器，获得当前位置*/
u8 ENC_UpdatePos(u8 num);

/*将采集到的编码器的脉冲数转化成角度*/
u32 ENC_PulseToAngle(u32 pulse);

/*Timer1初始化*/
void Timer1Init(void);

/*Timer8初始化*/
void Timer8Init(void);

#endif



