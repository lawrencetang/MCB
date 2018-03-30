
/**
TIMER4用于控制步进电机2的转动
 */

#include "timer4.h"

/*
Timer4初始化
*/
void Timer4Init(u16 l_u16SMPeriod)
{		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM4);

	if(l_u16SMPeriod < 50)
		l_u16SMPeriod = 50;  /* 36M时钟，最大不能超过160K的中断频率，即脉冲频率不能超过80K， 36M/160K = 225 */

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 1; //直接用36M时钟
	TIM_TimeBaseStructure.TIM_Prescaler = l_u16SMPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update); 
	TIM_ARRPreloadConfig(TIM4, DISABLE);

	/* TIM IT enable */
	TIM_ITConfig(TIM4,TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM4, 0); 
	TIM_Cmd(TIM4, DISABLE);	
}



/*
启动Timer4
*/
void Timer4Start(void)
{
	TIM_SetCounter(TIM4, 0); 
	TIM_Cmd(TIM4, ENABLE);
}



/*
停止Timer4
*/
void Timer4Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);
	TIM_SetCounter(TIM4, 0);
}



/*
设置定时器间隔
*/
void Timer4Set(u16 l_u8SMPeriod)
{
	Timer4Stop();

	Timer4Init(l_u8SMPeriod); 

	Timer4Start();	
}


