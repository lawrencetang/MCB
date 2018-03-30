
/*************************************
增量编码器相关函数
*************************************/
#include "inc_encoder.h"
#include "math.h"

INC_ENC g_enc[SM_No];



/*
初始化增量编码器
*/
void ENC_InitENC(u8 num)
{
	switch(num)
	{
	case 0:
		g_enc[0].cnt = 0;
		TIM1->CNT = 0;
		g_enc[0].pulse = 0;
		break;
	case 1:
		g_enc[1].cnt = 0;
		TIM8->CNT = 0;
		g_enc[1].pulse = 0;
		break;
	default:
		break;
	}
}



/*
更新增量编码器的值，获取当前位置
*/
u8 ENC_UpdatePos(u8 num)
{	
	u16 l_cnt;
	s16 l_acc_cnt;
	s32 l_pulse;
	
	switch(num)
	{
	case 0:
		l_cnt = TIM1->CNT;
		break;
	case 1:
		l_cnt = TIM8->CNT;
		break;
	default:
		break;
	}
	
	l_acc_cnt = l_cnt - g_enc[num].cnt; 

	l_pulse = l_acc_cnt;
	
	g_enc[num].pulse += l_pulse;	 

	g_enc[num].cnt = l_cnt;	  

	return NO_ERROR;
}



/*
Timer1初始化，Timer1与电机1的增量编码器连接
*/
void Timer1Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
  
	/* Encoder unit connected to TIM1, 4X mode */    
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
	/* TIM1 clock source enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
	GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PA.08,09 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	/* Enable the TIM1 Update Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Timer configuration in Encoder mode */
	TIM_DeInit(TIM1);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
	TIM_TimeBaseStructure.TIM_Prescaler = 1;  // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 0xfffe;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                //tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);
  
	// Clear all pending interrupts
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM1->CNT = 0;	 
  
	TIM_Cmd(TIM1, ENABLE);				 
}



/*
Timer8初始化，Timer8与电机2的增量编码器连接
*/
void Timer8Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
  
	/* Encoder unit connected to TIM8, 4X mode */    
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
	/* TIM8 clock source enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
  
	GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PC.06,07 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  
	/* Enable the TIM8 Update Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Timer configuration in Encoder mode */
	TIM_DeInit(TIM8);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
	TIM_TimeBaseStructure.TIM_Prescaler = 1;  // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 0xfffe;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                //tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM8, &TIM_ICInitStructure);
  
	// Clear all pending interrupts
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	//Reset counter
	TIM8->CNT = 0;	 
  
	TIM_Cmd(TIM8, ENABLE);				 
}


