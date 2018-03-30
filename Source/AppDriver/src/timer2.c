#include "timer2.h"
#include "stm32f10xDriver.h"
#include "AppDriver.h"

/*
* TIM��ʱ������
* TIM2-TIM7�õ���PCLK1��PCLK1�Ǵ�HCLK��Ƶ�õ�
* ���PCLK1ֱ�ӵ���HCLK����TIM��ʱ�ӵ���PCLK1
* �����PCLK1����HCLK�����˷�Ƶ����TIM��ʱ�ӵ���2��PCLK1
* ��ϵͳ�У�HCLK = 72M��PCLK1 = 36M��PCLK2 = 72M��
* ��TIM2-TIM7��ʱ��ʵ���ϵ��� 72M 
*/




/**
 * ������	��Timer2Init
 * ����		����ʼ��Timer2��Timer2����UCOSϵͳʱ�ӣ�����Ϊ100us
 */
void Timer2Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM2);
	
	/*
	����UCOSϵͳʱ�ӣ�����ʱ�����ж�ʱ�䣬��ʱ��ʱ��ԴΪ72MHz
	��Ҫ����72000���������ܵõ�1ms��ʱ�ӣ����÷�Ƶ��Ϊ36000����UCOS��tickΪ1ms
	*/
	TIM_TimeBaseStructure.TIM_Period = 999; 	/* �Զ���װֵΪ999��ʵ�ʼ���Ϊ1000��0~999 */
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 	/* ����72Ԥ��Ƶ��ֵTIM2�ļ�ʱʱ�ӱ�Ϊ1MHz */

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
