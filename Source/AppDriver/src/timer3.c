
/*
TIMER3���ڿ��Ʋ������1��ת��
*/

#include "timer3.h"
#include "stm32f10x_tim.h"

/*
* TIM��ʱ������
* TIM2-TIM7�õ���PCLK1��PCLK1�Ǵ�HCLK��Ƶ�õ�
* ���PCLK1ֱ�ӵ���HCLK����TIM��ʱ�ӵ���PCLK1
* �����PCLK1����HCLK�����˷�Ƶ����TIM��ʱ�ӵ���2��PCLK1
* ��ϵͳ�У�HCLK = 72M��PCLK1 = 36M��PCLK2 = 72M��
* ��TIM2-TIM7��ʱ��ʵ���ϵ��� 72M 
*/


/*
Timer3��ʼ��
*/
void Timer3Init(void)
{
	u16 l_u16SMCnt;		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM3);

	l_u16SMCnt = 1000000*TIMER3_PERIOD;		/* TIM3�ļ���ʱ��Ϊ1M������ֵ = 1M * �ж�ʱ���� */
	if(l_u16SMCnt < 10)					/* ��С����ֵ10�����ж����Ƶ��Ϊ100KHZ */
		l_u16SMCnt = 10;  

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = l_u16SMCnt - 1; 	/* �Զ���װֵ��ʵ�ʼ���Ϊ �Զ���װֵ + 1 */
	TIM_TimeBaseStructure.TIM_Prescaler = 71;			/* ����72Ԥ��Ƶ��ֵTIM3�ļ�ʱʱ�ӱ�Ϊ1MHz */
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
����Timer3
*/
void Timer3Start(void)
{
	TIM_SetCounter(TIM3, 0); 
	TIM_Cmd(TIM3, ENABLE);
}



/*
ֹͣTimer3
*/
void Timer3Stop(void)
{
	TIM_Cmd(TIM3, DISABLE);
	TIM_SetCounter(TIM3, 0);
}



/*
���ö�ʱ�����
*/
void Timer3Set(void)
{
	Timer3Stop();

	Timer3Init(); 

	Timer3Start();	
}


