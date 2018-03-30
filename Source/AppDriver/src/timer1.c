//#include "timer1.h"
//#include "header.h"
//
////	// 
///**
// * 函数名	：Timer1Init
// * 描述		：初始化Timer1，Timer1用于PSAM卡操作
// *			：设置PSAM卡操作时钟为主时钟的1/2，即：APB2(72M)/(1+TIM_Prescaler)=36M
// *			：TODO：需要在SC_Interface_Init前面调用吗？因为整合前的Timer1Init是在SC_Interface_Init函数结尾调用的
// */
//void Timer1Init(void)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	
//	TIM_DeInit(TIM1);
//	TIM_TimeBaseStructure.TIM_Prescaler = 1; // TIM1 时钟 APB2(72M)/(1+TIM_Prescaler)=36M
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseStructure.TIM_Period =0xF000;			
//	TIM_TimeBaseStructure.TIM_ClockDivision = 2;
//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//	
//	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
//
//	//TIM1 counter disable 	 	
//	TIM_Cmd(TIM1, DISABLE); 
//}
//
//
////延时函数
//uint8 EtuDelay(uint16 counts)
//{   
//    uint16 Temp1,Temp2;
//	uint8  Delay_Flag=1;
//	uint16 Time_Counts_Start=0;
//	uint16 Time_Counts_End=0;
//	if(counts==0)
//	{
//	  return 1;
//	}
//
//	Temp1 = counts;
//	TIM_SetCounter(TIM1, 0);	
//	TIM_Cmd(TIM1, ENABLE);
//
//	Time_Counts_Start = TIM1->CNT;
//    while (Delay_Flag)
//	{
//		Time_Counts_End = TIM1->CNT;
//		Temp2 = Time_Counts_End - Time_Counts_Start;
//		if( Temp2 > Temp1)
//		{
//			Delay_Flag=0;       	
//		}
//	}
//
//	TIM_Cmd(TIM1, DISABLE);
//	
//	return 1; 
//}
//
