//#include "timer1.h"
//#include "header.h"
//
////	// 
///**
// * ������	��Timer1Init
// * ����		����ʼ��Timer1��Timer1����PSAM������
// *			������PSAM������ʱ��Ϊ��ʱ�ӵ�1/2������APB2(72M)/(1+TIM_Prescaler)=36M
// *			��TODO����Ҫ��SC_Interface_Initǰ���������Ϊ����ǰ��Timer1Init����SC_Interface_Init������β���õ�
// */
//void Timer1Init(void)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	
//	TIM_DeInit(TIM1);
//	TIM_TimeBaseStructure.TIM_Prescaler = 1; // TIM1 ʱ�� APB2(72M)/(1+TIM_Prescaler)=36M
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
////��ʱ����
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
