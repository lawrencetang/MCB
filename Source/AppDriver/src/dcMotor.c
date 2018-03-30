/*

*/

#include "pcComm.h"
#include "dcMotor.h"
#include "protocal.h"
#include "timer5.h"
#include "stm32f10xDriver.h"


DCMotor g_dcm[DCM_NUM];

const u16 g_dcm_rate[DCM_RATE_NUM] = {DCM_RATE1, DCM_RATE2, DCM_RATE3};

/*
* ֱ�������ʼ��
*/
void dcmInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    GPIO_InitStructure.GPIO_Pin = DCM1_PIN_OUT | DCM2_PIN_OUT;//lt:PA2,PA3

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//lt:�������죿
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	dcmStopDCM(DCM1);
	dcmStopDCM(DCM2);

	g_dcm[DCM1].sts = 0;
	g_dcm[DCM1].rate = DCM_RATE1;//lt:1%,���ߵ�ƽռ0.1ms������10ms

	g_dcm[DCM2].sts = 0;
	g_dcm[DCM2].rate = DCM_RATE1;

	tim5SetPeriod(DCM_CNT_NUM);//lt:dcm_cnt_mum=10000,���ʱ��Ϊ10ms,��PWM����������Ϊ10ms����ͨ�����������趨�趨PWM��ռ�ձ�
	tchSWInit();
}

/*
* ֱ������������
* ������λ����������ָ�����ֱ�������
*/
u8 dcmSetCMD(pcFrame frame)
{
	u8 dev_num = frame.dev - 1;
	if(dev_num >= DCM_NUM) /*�����Ų��Ϸ�*/
		return STS_DEV_ERROR; 

	switch(frame.cmd)
	{																					 
		case DCM_CHECK:			//������ϲ�Ѱ//lt:0x90
			break;

		case DCM_START:		   	//�������//lt:0x91
			if((frame.data[3] < 1) || (frame.data[3] > DCM_RATE_NUM))
			{
				return STS_DATA_ERROR;
			}
			g_dcm[dev_num].rate = g_dcm_rate[frame.data[3] - 1];
			dcmStartDCM(dev_num);							
			break;

		case DCM_STOP:		   	//ֹͣ���//lt:0x92
			dcmStopDCM(dev_num);
			break;

		default:
			return STS_CMD_ERROR;
	}	
	return STS_SUCCESS;	
}

/*
* ����ֱ�����
* ����PWM��������Ѷ�ʱ����Ӧͨ����PWM�Ƚ�ֵ����Ϊ��Ӧ��ռ�ձȲ���
*/
void dcmStartDCM(u8 num)
{
	
	switch(num)
	{
	case DCM1:
		tim5ConfigPWM(3, (g_dcm[num].rate * DCM_CNT_NUM) / 100);//lt:�������ڶ���������ֵ��ת��ƽ
		break;
	case DCM2:
		tim5ConfigPWM(4, (g_dcm[num].rate * DCM_CNT_NUM) / 100);
		break;
	}
	g_dcm[num].sts = 1;
	tim5StartCounter();
}

/*
* ֱֹͣ�����
* ֹͣPWM���������ʱ����Ӧͨ����PWM�Ƚ�ֵ����Ϊ0����һֱ����͵�ƽ
*/
void dcmStopDCM(u8 num)
{
	
	switch(num)
	{
	case DCM1:
		tim5ConfigPWM(3, 0);
		break;
	case DCM2:
		tim5ConfigPWM(4, 0);
		break;
	}
	g_dcm[num].sts = 0;
	tim5StartCounter();
}


/*
* �ᴥ���س�ʼ��
* �ᴥ��������ֱ�����2���ֶ����ƣ�ÿ��һ��ֱ�����2״̬�͸ı�
* ��ʼ�������������Ϊ�ⲿ�жϽ�
*/
void tchSWInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_InitStructure.GPIO_Pin = TCHSW_PIN_IN;//lt:PB1�ж���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

	 /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0E;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    NVIC_Init(&NVIC_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line1);
}


/*
* �ᴥ�����жϷ�����
* ���ڿ���ֱ�����2����ͣ��ÿ��һ��״̬��ת
*/
void tchSWIsr(void)
{
	pcFrame frame;
	frameInit((pcFrame*)&frame);
	if(g_dcm[DCM2].sts)
	{
		frame.cmd = DCM_START;
		dcmStopDCM(DCM2);
	}
	else
	{
		frame.cmd = DCM_STOP;
		dcmStartDCM(DCM2);
	}//lt?�Ƿ���������߼�����
	frame.dev = 0x02;
	pcTxQueueAddFrame((pcFrame *)&frame);	
}//lt:���ж���1���жϺ����н���


