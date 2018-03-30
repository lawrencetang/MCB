/*
stepMotor.c
*/

#include "protocal.h"
#include "task.h"
#include "stepMotor.h"
#include "math.h"
#include "TaskInit.h"
#include "pcComm.h"
#include "absEncoder.h"
#include "eeprom.h"

#include <includes.h>

/*
ת�����Ʋ�����һ��ת�����������У����ڿ��Ƽ��١����١����ٲ�����
*/
STEP_MOTOR g_sm[SM_NUM];


/*
���������ʼ�����������źͲ���
*/
void smInit(void)
{	
	u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;
   
	/* �������1�õ���PD13~PD15, �������2�õ���PD10~PD12 */
	GPIO_InitStructure.GPIO_Pin = SM1_PIN_EN | SM1_PIN_STP | SM1_PIN_DIR | SM2_PIN_EN | SM2_PIN_STP | SM2_PIN_DIR;//lt:PE8-10,PE13-15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 	 	

	/*�������IO�������*/
	g_sm[0].pin.en 	= SM1_PIN_EN;//lt:������ʹ���ź�
	g_sm[0].pin.stp = SM1_PIN_STP;//lt:��������������
	g_sm[0].pin.dir = SM1_PIN_DIR;//lt:�����������ź�
	g_sm[0].para.event = OSSemCreate(0);;//lt:��������ʼ��һ���ź���,�������ͷŸ��¼����źŲ���ִ��

	g_sm[1].pin.en 	= SM2_PIN_EN;
	g_sm[1].pin.stp = SM2_PIN_STP;
	g_sm[1].pin.dir = SM2_PIN_DIR;
	g_sm[1].para.event = OSSemCreate(0);;

	absEncInit();


	/*��ʼ�����Ʋ���*/
	for (i = 0; i < SM_NUM; i++)
	{
		smClearRotate(i); 		

		g_sm[i].flag = SM_STS_IDLE;//lt:���״̬����
		g_sm[i].goalAngle = 0;//lt:Ҫת������Ŀ��Ƕ�
		g_sm[i].para.accTicks = SM_ACC_TICKS; 	/* ���ٶ� *///lt:0.005
		g_sm[i].para.maxTicks = SM_MAX_WAIT_TICKS;//lt:10
		g_sm[i].para.minTicks = SM_MIN_WAIT_TICKS;//lt:2
		
		smUpdateAngle(i);//lt:���ñ������õ���ʵ�ĽǶ�
	} 
}

/*
��䲽������ǶȲ��ϴ�����֡ 
*/
void smUploadAngle(pcFrame *frame)
{
	frame->sts = STS_SUCCESS;//lt:����״̬0x00
	frame->para = 1;//lt:Я������		
	frame->data[0] = ((u32)(g_sm[frame->dev-1].realAngle*10))>>24;//lt:ʵ����ת�Ƕȸ�λ
	frame->data[1] = ((u32)(g_sm[frame->dev-1].realAngle*10))>>16;
	frame->data[2] = ((u32)(g_sm[frame->dev-1].realAngle*10))>>8;
	frame->data[3] = ((u32)(g_sm[frame->dev-1].realAngle*10));//lt:ʵ����ת�Ƕ�*10�ϴ�
	pcTxQueueAddFrame(frame);	
}

void smUploadLogicZero(pcFrame *frame)
{
	float logicZero;
	logicZero = absEncGetLogicZero(frame->dev-1);
	frame->sts = STS_SUCCESS;
	frame->para = 1;		
	frame->data[0] = ((u32)logicZero*10)>>24;//lt:�߼�0���λ
	frame->data[1] = ((u32)logicZero*10)>>16;
	frame->data[2] = ((u32)logicZero*10)>>8;
	frame->data[3] = ((u32)logicZero*10);//lt:�߼�0��*10�ϴ�
	pcTxQueueAddFrame(frame);	
}


/*
*/
u8 smSetCMD(pcFrame frame)
{
	u8 dev_num = frame.dev - 1;
	if(dev_num >= SM_NUM) /*�����Ų��Ϸ�*/
		return STS_DEV_ERROR; 
	if(g_sm[dev_num].flag == SM_STS_BUZY)  //lt:01,���״̬��æ
		return STS_BUZY;
	if(g_sm[dev_num].flag)  //lt:����
		return STS_FAULT; 

	switch(frame.cmd)
	{																					 
		case SM_CHECK:			//������ϲ�Ѱ  //lt:�����0x80
			smUploadAngle((pcFrame *)&frame);//lt:����ת�Ƕ�
			break;

		case SM_START:		   	//������������2������ת����340�����ϵ�����		//lt:�����0x81					
			g_sm[dev_num].goalAngle = (float)((frame.data[0]<<24 | frame.data[1]<<16 | frame.data[2]<<8 | frame.data[3])/10.0);//lt:�����·�ָ��ȷ��Ŀ��Ƕ�,����ԭ�Ƕȵ���ʵֵ
			if((g_sm[frame.dev].goalAngle >= SM_MAX_ANGLE) || ((frame.dev == SM2) && (g_sm[frame.dev].goalAngle > 340)))//lt:δ���豸��-1�Ƿ��ܹ���Ӧ����?
			{
				g_sm[dev_num].goalAngle = 0;
				return STS_DATA_ERROR;//lt:����0x05
			}

			g_sm[dev_num].flag = SM_STS_BUZY;//lt:����Ϊæ��־
			OSSemPost(g_sm[dev_num].para.event);//lt:��ʼ�ź�����Ϊ0,��ִ��һ��HPT(��˯�ߵĲ������1�򲽽����2����)
			break;

		case SM_STOP:		   	//ǿ��ֹͣ���  //lt:�����0x82
			g_sm[dev_num].ro.sts = 0;//lt:����ֹͣת��
			g_sm[dev_num].flag = SM_STS_IDLE;
			smUploadAngle((pcFrame *)&frame);//lt:�ϴ�ʵ����ת�Ƕ�
			break;

		case SM_READAPOS:		//����ǰ�Ƕ�λ�� //lt:�����0x83
			smUploadAngle((pcFrame *)&frame);//lt:�ϴ���һ�ε�ʵ����ת�Ƕ�
			smUpdateAngle(dev_num);//lt:���µ�ǰ�豸��ʵ����ת�Ƕ�
			break;
		case SM_READZERO:  //lt:���߼�0�㣬�����0x84
			smUploadLogicZero((pcFrame *)&frame);
			break;
		case SM_SETAERO:  //lt:�����߼�0�㣬�����0x85
			absEncSetLogicZero(dev_num);//lt:�����ԽǶ�����Ϊ�߼�0�㱣�浽��Ӧ�豸��EEprom����Ӧ��ַ��
			smUploadLogicZero((pcFrame *)&frame);//lt:�ϴ��߼�0��
			break;

		default:
			return STS_CMD_ERROR; //lt:����0x03��ָ�����
	}
	
	return STS_SUCCESS;	//lt:����0x00��ָ��ɹ�
}




/*
���������������е���ת����صĲ���
*/
void smClearRotate(u8 num)
{
	g_sm[num].ro.sts = 0;//lt:����ת��״ָ̬ʾ��0 ת��ֹͣ�� 1 ����ת��
	g_sm[num].ro.totalPulse = 0;
	g_sm[num].ro.remainPulse = 0;
	g_sm[num].ro.ticks = 0;//lt:ʵʱ�������
}


/*
�������ת��
num��������
pulse��ת����������
acc��������ٶ�
*/
void smStartSM(u8 num, u32 pulse, float acc)
{
	if(g_sm[num].flag == SM_STS_IDLE)
	{
		return ;
	}
	g_sm[num].ro.acc = acc;//lt:���ٶ�
	g_sm[num].ro.remainPulse = pulse;//lt:ʣ��������	
	g_sm[num].ro.ticks = g_sm[num].ro.spdTicks;//lt:���������
	g_sm[num].ro.sts = 1;//lt:����ת��	
}


/*
����ת��
num��������
pulse��ת����������
*/
u8 smSpeedUp(u8 num, u32 pulse)
{
	u8 err;
	g_sm[num].ro.spdTicks = g_sm[num].para.maxTicks;
	smStartSM(num, pulse, 0-g_sm[num].para.accTicks);//lt:����������Ϊ����
	OSSemPend(g_sm[num].para.event, 5000, &err);//lt:�����ź�,��ʱʱ��Ϊ0.5s
	return err;
}

/*
����ת��
num��������
pulse��ת����������
*/
u8 smSlowDown(u8 num, u32 pulse)
{
	u8 err;
	smStartSM(num, pulse, g_sm[num].para.accTicks);//lt:����������Ϊ����
	OSSemPend(g_sm[num].para.event, 5000, &err);//ltnotice���Ƿ�Ҫ��ǰ��smstartsm��������ǰ����סacc�����ĸı�?
	return err; //lt:����0�޴�
}


/*
����ת��
num��������
pulse��ת����������
*/
u8 smUniform(u8 num, u32 pulse)
{
	u8 err;
	smStartSM(num, pulse, 0);
	OSSemPend(g_sm[num].para.event, 5000, &err);
	return err;
}


/*
������п���
num��������
degree��Ҫת���ĽǶȣ�����ת��������dir���档
*/
u8 smRunSM(u8 num, float degree)
{
	u32 accPulse;
	
	if(degree < SM_EQUAL_ANGLE_ERROR)
	{
		return STS_SUCCESS;
	}//lt:�ջ��ж�
	
	SM_CLREN(num);//lt:ʧ��
	SM_SETDIR(num);//lt:ͨ���ṹ���е�dir���Ƶ��ת������
	g_sm[num].ro.totalPulse = SM_DegreeToPulse(degree)*2;//lt:stp�ڷ�ת����
	accPulse = 0;
	if(fabs(g_sm[num].para.accTicks) > 0.0001)  //lt:�������������ֵ,0.005
	{
		accPulse = (g_sm[num].para.maxTicks - g_sm[num].para.minTicks)/g_sm[num].para.accTicks;  //lt:1600,���������ڵ���С�����ڵķ�ת����
	}

	if(g_sm[num].ro.totalPulse <= (accPulse * 2))  //lt:С��180��
	{
		accPulse = g_sm[num].ro.totalPulse / 2;
		if(smSpeedUp(num, accPulse))
		{
			return SM_STS_FAULT;
		}//lt:����
		if(smSlowDown(num, accPulse))
		{
			return SM_STS_FAULT;
		}//lt������
	}
	else
	{
		if(smSpeedUp(num, accPulse))
		{
			return SM_STS_FAULT;
		}
		if(smUniform(num, g_sm[num].ro.totalPulse - 2 * accPulse))
		{
			return SM_STS_FAULT;
		}
		if(smSlowDown(num, accPulse))
		{
			return SM_STS_FAULT;
		}
	}//lt:���ټ�������
	return STS_SUCCESS;
}

/*
ֹͣ���ת��
*/
void smStopSM(u8 num)
{
	smClearRotate(num);
}


/*
����������Ƶ�tick
������ͨ����ʱ�����ã�Ҫ��ʱ����ʱ������20us����ÿ20us����뱾����һ�Ρ�
ÿ�ο��ƶ���Ҫ�������ٺͼ��ٹ���
*/
void smRotateTick(void)
{
	u8 i;
	for(i = 0; i < SM_NUM; i++)
	{
		if(g_sm[i].ro.sts)	/* ����ת���Ƿ���������ת��δ��������뱾���� */
		{
			if(!g_sm[i].ro.ticks--)   	/* ��ʱ�Ƿ� */ //lt:�������0
			{
				if(g_sm[i].ro.remainPulse--)	/* �����Ƿ�ﵽ */ //lt:û������0
				{
					g_sm[i].ro.spdTicks = g_sm[i].ro.spdTicks + g_sm[i].ro.acc;
					if(g_sm[i].ro.spdTicks < g_sm[i].para.minTicks)
					{
						g_sm[i].ro.spdTicks = g_sm[i].para.minTicks;
					}//lt:������С������Ƶ��,����ʱ���Ϊ2
					else if(g_sm[i].ro.spdTicks > g_sm[i].para.maxTicks)
					{
						g_sm[i].ro.spdTicks = g_sm[i].para.maxTicks;
					}//lt:�������������Ƶ��,�´μ�������Ϊ10
					SM_REVSTP(i);//lt:stp�ڷ�ת
					smUpdateAngle(i);//lt:���½Ƕ�
				}
				else
				{
					g_sm[i].ro.sts = 0;//lt:�����������ֹͣת��
					OSSemPost(g_sm[i].para.event);
				}
				g_sm[i].ro.ticks = g_sm[i].ro.spdTicks;//lt:����ʹ��20us�ĸ���Խ��Խ�٣�����ʹ��20us�ĸ���Խ��Խ��
			}			
		}
		else
		{
			smUpdateAngle(i);
		}		
	}
}



/*
* ��ȡҪת���ĽǶȴ�С�����Զ�ȡ��ת������
* ͨ��Ŀ��Ƕ��뵱ǰ�Ƕȼ��㻹��Ҫת���ĽǶ�
*/
float smGetAngle(u8 num)
{
    float angle;
	if(num == SM1)
	{
		angle = g_sm[num].goalAngle - g_sm[num].realAngle;	
		if (angle < 0)
		{
			angle = angle + 360;
		}	
		/* �ӵ�ǰ�Ƕ�ת��Ŀ��Ƕȣ���ʱ�룬�Ƕ�С�ڵ���180������ת����������ת�� */
	    g_sm[num].dir = 0;		/* ������ʱ�� */
		if(angle > 180)
		{
			g_sm[num].dir = 1;	/* ����˳ʱ�� */
			angle = 360 - angle;
		}
	}
	else if(num == SM2)
	{
		if(g_sm[num].realAngle > 350) 	/* 350�ȵ�360��֮��ĵ���0�ȴ��� */
		{
			g_sm[num].dir = 0;
			angle = g_sm[num].goalAngle + 360 - g_sm[num].realAngle;
		}
		else
		{
			if(g_sm[num].goalAngle > g_sm[num].realAngle)
			{
				g_sm[num].dir = 0;
				angle = g_sm[num].goalAngle - g_sm[num].realAngle;
			}
			else
			{
				g_sm[num].dir = 1;
				angle = g_sm[num].realAngle - g_sm[num].goalAngle;
			}
		}
	}
	else
	{
		angle = 0;
	}
	return angle;
}



/*
ת������
�����ת�����Ʊ���Ϊ������ת�������١����١����١����ת���ǶȽ�С����ֻ�м��ٺͼ���
ÿ��
*/
u8 smRotate(u8 num)
{
	u8 err;
	pcFrame frame;

	err = smRunSM(num, smGetAngle(num));//lt:�ɹ���0.���ɹ���2
//	err += smRunSM(num, smGetAngle(num));
	g_sm[num].flag = SM_STS_IDLE;

	frame.cmd = SM_START;
	frame.dev = num + 1;
	frame.sts = (err + smJudgeEqualAngle(g_sm[num].realAngle, g_sm[num].goalAngle));
	frame.para = 1;

	frame.data[0] = ((u32)(g_sm[num].realAngle*10))>>24;
	frame.data[1] = ((u32)(g_sm[num].realAngle*10))>>16;
	frame.data[2] = ((u32)(g_sm[num].realAngle*10))>>8;
	frame.data[3] = ((u32)(g_sm[num].realAngle*10));

	pcTxQueueAddFrame((pcFrame *)&frame);
	return err;
}


/*
�ж������Ƕ��Ƿ����
��������ǶȲ�С��SM_EQUAL_ANGLE_ERROR������ȣ������ж�Ϊ����
����ֵ��1 ���ȣ�0 ���
*/
u8 smJudgeEqualAngle(float a1, float a2)
{
	float a;
	a = a1 - a2;
	if(a1 < a2)
	{
		a = a2 - a1;
	}
	
	if(a > 180)
	{
		a = 360 - a;
	}
	return (a > SM_EQUAL_ANGLE_ERROR);//lt:����1�㷵��1
}

/*
��������Լ�
�õ��ת��10�ȣ���ת��ԭ����λ�ã��ж�ת���Ƿ���������������򷵻سɹ������򷵻�ʧ�ܡ�
*/
u8 smCheck(u8 num)
{
	float destAngle, nowAngle;
	if(g_sm[num].flag == SM_STS_BUZY)
	{
		return STS_BUZY;						  
	}
	nowAngle = g_sm[num].realAngle;
	g_sm[num].goalAngle = g_sm[num].realAngle + 10;
	if(g_sm[num].goalAngle > 360)
	{
		g_sm[num].goalAngle = g_sm[num].goalAngle - 360;
	}
	destAngle = g_sm[num].goalAngle;
	smRotate(num);
	if(smJudgeEqualAngle(destAngle, g_sm[num].realAngle))
	{
		return STS_FAULT;
	}
	g_sm[num].goalAngle = nowAngle;
	smRotate(num);
	if(smJudgeEqualAngle(nowAngle, g_sm[num].realAngle))
	{
		return STS_FAULT;
	}
	return STS_SUCCESS; 
}

/*
* ���²�������ĽǶ�
*/
void smUpdateAngle(u8 num)
{
	absEncUpdateAngle(num);//lt:ͨ�������������ʵ����ת�Ƕ�
	g_sm[num].realAngle = absEncGetAngle(num);//lt:��ʵ����ת�Ƕȸ���ṹ
}


