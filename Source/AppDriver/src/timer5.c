
/*
timer5.c
TIMER5���ڿ���ֱ�����PWM���
*/

#include "timer5.h"
#include "stm32f10x_tim.h"
#include "stm32f10xDriver.h"


/*
* TIM��ʱ������
* TIM2-TIM7�õ���PCLK1��PCLK1�Ǵ�HCLK��Ƶ�õ�
* ���PCLK1ֱ�ӵ���HCLK����TIM��ʱ�ӵ���PCLK1
* �����PCLK1����HCLK�����˷�Ƶ����TIM��ʱ�ӵ���2��PCLK1
* ��ϵͳ�У�HCLK = 72M��PCLK1 = 36M��PCLK2 = 72M��
* ��TIM2-TIM7��ʱ��ʵ���ϵ��� 72M 
*///lt:APB1��prescaler=2,����TIM��ʱ����APB1��ʱ��*2


/*
* ���ö�ʱ��5������
* period��������������1us�������м���ʱ��Ϊ1MHz����1us
* ���������ǣ�����ʱ��5����Ϊ���ϼ���
*/
void tim5SetPeriod(u16 period)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  
    /* Time base configuration */                                            
    TIM_TimeBaseStructure.TIM_Period = period-1;  
    TIM_TimeBaseStructure.TIM_Prescaler = 71;                                   //����Ԥ��Ƶ��Ԥ��Ƶ=72����1MHz  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                //����ʱ�ӷ�Ƶϵ��������Ƶ  //lt:TIM_CKD_DIV1=0X00
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //���ϼ������ģʽ  
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM5, DISABLE);//lt:ʧ��Ԥװ�ؼĴ�����������д��Ԥװ�ص�ֵ��ʱ��Ч
}

/*
* ���ö�ʱ��5ĳͨ��PWM�ĸߵ�ƽʱ��
* channel��ͨ����
* highPulse���ߵ�ƽʱ�䣬���Զ�װ��ֵ��ϣ��ߵ�ƽռ�ձ� = highPulse / �Զ�װ��ֵ
*/
void tim5ConfigPWM(u8 channel, u16 highPulse)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //����ΪPWMģʽ1  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
    TIM_OCInitStructure.TIM_Pulse = highPulse;                                  //��������ֵ�������������������ֵʱ����ƽ�������� //lt:���� 
    TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;                    //����ʱ������ֵС��CCR1ʱΪ�ߵ�ƽ
	  
    switch(channel)
	{
	case 1:
		TIM_OC1Init(TIM5, &TIM_OCInitStructure);                                    //ʹ��ͨ��1      
	    TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case 2:
	    TIM_OC2Init(TIM5, &TIM_OCInitStructure);                                    //ʹ��ͨ��2  
    	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case 3:
		TIM_OC3Init(TIM5, &TIM_OCInitStructure);                                    //ʹ��ͨ��1      
	    TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case 4:
	    TIM_OC4Init(TIM5, &TIM_OCInitStructure);                                    //ʹ��ͨ��2  
    	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	default:
		break;
	}
}


/*
* ������ʱ��5
*/
void tim5StartCounter(void)
{
    TIM_ARRPreloadConfig(TIM5, ENABLE);                                         //ʹ��TIM5���ؼĴ���ARR  
    TIM_Cmd(TIM5, ENABLE); 
}



