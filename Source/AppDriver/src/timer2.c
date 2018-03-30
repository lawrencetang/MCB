#include "timer2.h"
#include "stm32f10xDriver.h"
#include "AppDriver.h"

/*
* TIM的时钟问题
* TIM2-TIM7用的是PCLK1，PCLK1是从HCLK分频得到
* 如果PCLK1直接等于HCLK，则TIM的时钟等于PCLK1
* 但如果PCLK1等于HCLK经过了分频，则TIM的时钟等于2倍PCLK1
* 本系统中，HCLK = 72M，PCLK1 = 36M，PCLK2 = 72M。
* 则TIM2-TIM7的时钟实际上等于 72M 
*/




/**
 * 函数名	：Timer2Init
 * 描述		：初始化Timer2，Timer2用于UCOS系统时钟，节拍为100us
 */
void Timer2Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM2);
	
	/*
	设置UCOS系统时钟，即定时器的中断时间，定时器时钟源为72MHz
	需要经过72000个计数便能得到1ms的时钟，设置分频数为36000，即UCOS的tick为1ms
	*/
	TIM_TimeBaseStructure.TIM_Period = 999; 	/* 自动重装值为999，实际计数为1000，0~999 */
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 	/* 进行72预分频，值TIM2的计时时钟变为1MHz */

	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 
	TIM_ARRPreloadConfig(TIM2, DISABLE);

	// TIM IT enable
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	// TIM2 enable counter
	TIM_Cmd(TIM2, ENABLE);
}
