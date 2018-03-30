
/*
TIMER3用于控制步进电机1的转动
*/

#include "timer3.h"
#include "stm32f10x_tim.h"

/*
* TIM的时钟问题
* TIM2-TIM7用的是PCLK1，PCLK1是从HCLK分频得到
* 如果PCLK1直接等于HCLK，则TIM的时钟等于PCLK1
* 但如果PCLK1等于HCLK经过了分频，则TIM的时钟等于2倍PCLK1
* 本系统中，HCLK = 72M，PCLK1 = 36M，PCLK2 = 72M。
* 则TIM2-TIM7的时钟实际上等于 72M 
*/


/*
Timer3初始化
*/
void Timer3Init(void)
{
	u16 l_u16SMCnt;		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM3);

	l_u16SMCnt = 1000000*TIMER3_PERIOD;		/* TIM3的计数时钟为1M，计数值 = 1M * 中断时间间隔 */
	if(l_u16SMCnt < 10)					/* 最小计数值10，即中断最高频率为100KHZ */
		l_u16SMCnt = 10;  

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = l_u16SMCnt - 1; 	/* 自动重装值，实际计数为 自动重装值 + 1 */
	TIM_TimeBaseStructure.TIM_Prescaler = 71;			/* 进行72预分频，值TIM3的计时时钟变为1MHz */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 
	TIM_ARRPreloadConfig(TIM3, DISABLE);

	/* TIM IT enable */
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM3 , 0); 
	TIM_Cmd(TIM3, DISABLE);	
}



/*
启动Timer3
*/
void Timer3Start(void)
{
	TIM_SetCounter(TIM3, 0); 
	TIM_Cmd(TIM3, ENABLE);
}



/*
停止Timer3
*/
void Timer3Stop(void)
{
	TIM_Cmd(TIM3, DISABLE);
	TIM_SetCounter(TIM3, 0);
}



/*
设置定时器间隔
*/
void Timer3Set(void)
{
	Timer3Stop();

	Timer3Init(); 

	Timer3Start();	
}


