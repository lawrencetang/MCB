
/*
timer5.c
TIMER5用于控制直流电机PWM输出
*/

#include "timer5.h"
#include "stm32f10x_tim.h"
#include "stm32f10xDriver.h"


/*
* TIM的时钟问题
* TIM2-TIM7用的是PCLK1，PCLK1是从HCLK分频得到
* 如果PCLK1直接等于HCLK，则TIM的时钟等于PCLK1
* 但如果PCLK1等于HCLK经过了分频，则TIM的时钟等于2倍PCLK1
* 本系统中，HCLK = 72M，PCLK1 = 36M，PCLK2 = 72M。
* 则TIM2-TIM7的时钟实际上等于 72M 
*///lt:APB1的prescaler=2,所以TIM的时钟是APB1的时钟*2


/*
* 配置定时器5的周期
* period：计数周期数（1us），其中计数时钟为1MHz，即1us
* 基本功能是：将定时器5配置为向上计数
*/
void tim5SetPeriod(u16 period)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  
    /* Time base configuration */                                            
    TIM_TimeBaseStructure.TIM_Period = period-1;  
    TIM_TimeBaseStructure.TIM_Prescaler = 71;                                   //设置预分频：预分频=72，即1MHz  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                //设置时钟分频系数：不分频  //lt:TIM_CKD_DIV1=0X00
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //向上计数溢出模式  
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM5, DISABLE);//lt:失能预装载寄存器，即从新写入预装载的值随时有效
}

/*
* 设置定时器5某通道PWM的高电平时间
* channel：通道号
* highPulse：高电平时间，与自动装载值配合，高电平占空比 = highPulse / 自动装载值
*/
void tim5ConfigPWM(u8 channel, u16 highPulse)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //配置为PWM模式1  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
    TIM_OCInitStructure.TIM_Pulse = highPulse;                                  //设置跳变值，当计数器计数到这个值时，电平发生跳变 //lt:参数 
    TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;                    //当定时器计数值小于CCR1时为高电平
	  
    switch(channel)
	{
	case 1:
		TIM_OC1Init(TIM5, &TIM_OCInitStructure);                                    //使能通道1      
	    TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case 2:
	    TIM_OC2Init(TIM5, &TIM_OCInitStructure);                                    //使能通道2  
    	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case 3:
		TIM_OC3Init(TIM5, &TIM_OCInitStructure);                                    //使能通道1      
	    TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case 4:
	    TIM_OC4Init(TIM5, &TIM_OCInitStructure);                                    //使能通道2  
    	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	default:
		break;
	}
}


/*
* 启动定时器5
*/
void tim5StartCounter(void)
{
    TIM_ARRPreloadConfig(TIM5, ENABLE);                                         //使能TIM5重载寄存器ARR  
    TIM_Cmd(TIM5, ENABLE); 
}



