/*
temControl.c

�¶ȿ���Ӧ�ò�

*/
#include "temControl.h"
#include "protocal.h"
#include "math.h"
#include "adcDriver.h"
#include "common.h"


TEM_CTRL g_tem[TEM_NUM];  //lt:TEM_NUM = 1


/* ���¿���Ӧ�ò��ʼ�� */
u8 temAppInit(void)
{
	u8 i;
	/* ������в��� */
	for(i = TEM1; i < TEM_NUM; i++)   //lt:TEM1 = 0
	{
		temAppClear(i);/* ������� */
		g_tem[i].ad_channel = i;  /* ADͨ���� */
		g_tem[i].Event = OSSemCreate(0); /* ��ʼ���ź��� */
	}
	adcInit();
	temPinInit();
	return 0;
}


/*
��������¶ȿ��Ʋ���
*/
u8 temAppClear(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	g_tem[p_id].start = 0;
	g_tem[p_id].status = 0;

	g_tem[p_id].dt = 0.0;
	g_tem[p_id].e0 = 0.0;
	g_tem[p_id].e1 = 0.0;
	g_tem[p_id].es = 0.0; 	

	return 0;
}

/*
* PT1000����ֵ���¶�֮��Ĺ�ϵ����
* �¶�TEM��PT1000����ֵRPT�Ĺ�ϵΪ RPT = 1000 * (1 + A * TEM + B * TEM * TEM)������ A = 0.00386231, B = -0.000000653149
* ͨ�����㣬�õ��¶ȵ�����ֵ��һ����ʽ������Excel��Matlab���ߣ���Ϸ��ƣ��õ��ӵ��赽�¶ȵĹ�ʽ
* TEM = -245.898 + 0.235898 * RPT + 0.00001 * RPT * RPT
*/

#define VADREF 			3.3
#define VBREAGE 		4.96
#define RUP 	 		1000.0
#define RDOWN 			1000.0

float g_temUpload = 0; 	/* �����ϴ��ĵ�ǰ�¶� */

/* 
* ��ȡ��ǰ�¶ȣ���ͨ��AD��ֵ���Ƴ�PT1000�ĵ���ֵ�����ɵ���ֵ������¶ȡ�
* �ɼ�����12λADֵ�ǵ�ѹ����������ת��ΪAD�������ѹΪ VAD = 3.3 * data / 4095  //lt:�вο���ѹ
* AD����ĵ�ѹVAD�ǵ��ŵĲ�ֵ�ѹ�����Ŵ�õ����Ŵ���ΪGAIN�����ֵ�ѹ VDIV = VAD / GAIN
* ��ֵ�ѹVDIV���ɵ��ŷ�ѹ�õ���ͨ������1K�����һ��PT1000��ɵ��ţ�VDIV = VIP - VIN
* ����ͬ��˵�ѹΪ VIP = 2.5V
* ��������˵�ѹΪ VIN = 5 * 1000 / (1000 + RPT)������RPT��PT1000�ĵ���ֵ��
* RPT = 5000 / VIN - 1000
*/
float temAppGetTem(u8 p_id)
{
	u16 l_ad; 	  				//ADֵ	
	float vad, vdiv, vin; 	//��ѹ
	float rpt;			  		//����ֵ
	float tem; 		//�¶�
 
	if(p_id >= TEM_NUM) 		//����¶ȿ���ϵͳʱ����
	{
		return -1000.0;  //����
	}//lt:p_id = 0

	l_ad = g_ad.tem;
	vad = VADREF * l_ad / 4095;
	vdiv = vad / GAIN; //lt:gainΪ1+(53.6k/10k) = 6.36
	vin = VBREAGE * RDOWN / (RUP + RDOWN) - vdiv;
	rpt = RDOWN * VBREAGE  / vin - RDOWN;
	tem = 0.00001 * rpt * rpt + 0.235898 * rpt - 245.898; 

	g_temUpload = (g_temUpload * 9.0 + tem)	/ 10.0;

	return tem;
}



/*
* ����Ŀ���¶��뵱ǰ�¶ȼ����������ʱ��
* ��ʱ��Ϊ��TEM_TOTAL_TIME
* ��Ŀ���¶ȴ��ڵ�ǰ�¶�ʱ��������������Ŀ���¶�С�ڵ�ǰ�¶�ʱ�����ظ���
*/
s16 temGetHighTime(u8 p_id)
{
	float l_nt;
	s16 high_time; 
	l_nt = temAppGetTem(p_id);
	if(l_nt < -100.0)	
	{
		return -TEM_TOTAL_TIME;
	}

	g_tem[p_id].e1 = g_tem[p_id].e0;
	g_tem[p_id].e0 = g_tem[p_id].dt - l_nt;//lt:Ŀ���¶��뵱ǰ�¶ȵĲ�
	g_tem[p_id].es = g_tem[p_id].es + g_tem[p_id].e0;//lt:��ǰ������

	if(fabs(g_tem[p_id].e0) > TEM_PID_VALID)  //lt:���С��0.2�����PID������0.2��ֱ�ӷ���tem_total_time
	{
		g_tem[p_id].es = 0;
		if(g_tem[p_id].e0 > 0)
		{
			return TEM_TOTAL_TIME;//lt:������0����Ŀ���¶ȴ��ڵ�ǰ�¶�
		}
		else
		{
			return -TEM_TOTAL_TIME;
		}
		
	}

	high_time = KP * g_tem[p_id].e0 + KI * g_tem[p_id].es + KD * (g_tem[p_id].e0 - g_tem[p_id].e1);//lt:PID��ʽ��KP=2.5��KI=0.05��KD=0.01

	if(high_time > TEM_TOTAL_TIME)
	{
		high_time = TEM_TOTAL_TIME; 
	}

	if(high_time < -TEM_TOTAL_TIME)
	{
		high_time = -TEM_TOTAL_TIME;
	}

	return high_time;
}//lt:�����0.2���ڵõ���pidʱ����tem_total_time������Χ�ڷ���



/*
* �����������
* ��ÿ TEM_TOTAL_TIME ʱ���ڽ��бջ����ƣ���ͨ��ʱ����PID�㷨����ó� 
*/
u8 temHeatCool(u8 p_id)
{
	s16 l_high_time; 	/* ������������ĵ�ƽ����TEM_GetHighTime�����õ� */
	u16 l_hc_delay;		/* ���Ȼ��������ʱ�䣬��ʱ��Ϊ TEM_TOTAL_TIME����ͨʱ��Ϊ l_high_time��ʣ����ǶϿ�ʱ�� */
	u32 l_cycle_cnt; 	/* �ջ����Ƶ������� */

	float l_tem_err;
	
	if(p_id >= TEM_NUM)
	{
		return STS_DEV_ERROR;  //����
	} 

	g_tem[p_id].e0 = 0;
	g_tem[p_id].e1 = 0;
	g_tem[p_id].es = 0;
	l_hc_delay = 0;
	l_cycle_cnt = 0;
	l_tem_err = 1000.0;
	
	while(g_tem[p_id].start)
	{
		FANON;	 			//��֤���ȴ�
			
		l_high_time = temGetHighTime(p_id);	//��ȡһ�������ڣ����ȵ�ʱ��

		if(l_high_time < -TEM_TOTAL_TIME)
		{
			temStop(p_id);
			temAppClear(p_id);
			return STS_FAULT;	 //���ִ���ֱ�ӷ���
		}

		if(l_high_time < 0) 	//���ȵ�ʱ��С��0�����¶ȹ��ߣ���Ҫ����
		{
			l_hc_delay = 0 - l_high_time;
			temCool(p_id);  	//���û�����书�ܣ���仰��Ҫ���ε�����ֻ�ṩ���ȹ���//lt:�������书�ܣ�ʱ��l_hc_delay
		}
		else					//�������0����Ҫ����
		{
			l_hc_delay = l_high_time;
			temHeat(p_id);//lt:�������ȹ��ܣ�ʱ��l_hc_delay
		}

		OSTimeDly(l_hc_delay);	//����high_time��ʱ��//lt:������

		temStop(p_id);	 		//ֹͣ 

		OSTimeDly(TEM_TOTAL_TIME - l_hc_delay);//�ȴ�һ���������

	   	//���� TEMP_CYCLE_CTRL �����ں���Ҫ�ж��¶ȱ仯���������¶Ȳ��䣬����Ϊ�쳣������ֹͣ���ȿ���
		if(0 == (l_cycle_cnt % TEM_CYCLE_CTRL))   //lt:tem_cyle_ctrl = 1200
		{//��Ŀ���¶����� TEMP_VALID ���⣬��ǰ�¶ȱ��ϴ��¶���Ŀ���¶�Ҫ�����				
			if((fabs(g_tem[p_id].e0) > TEM_DIS_DIF) && (fabs(l_tem_err) - fabs(g_tem[p_id].e0) < TEM_VALID_ERR))//lt:������5���ұ��������ϴ����ֻС1��(��������)
			{
				g_tem[p_id].start = 0; //ֹͣ���¿���
				return STS_FAULT;
			}
			l_tem_err = g_tem[p_id].e0;	//ÿһ��ʱ���¼һ�µ�ǰ�¶���Ŀ���¶ȵĲ� 
		}

		l_cycle_cnt++; //�¶ȿ�������
	}
	temStop(p_id);
	temAppClear(p_id);		
	return STS_SUCCESS;	
}


/*
* �¿�ָ������
* ������λ���·����¶ȿ���ָ��
*/
u8 temCMDDeal(pcFrame frame)
{
	u8 l_id;
	s32 l_tem;

	l_id = frame.dev - 1;  /* �¿��豸��� */ 
	if(l_id >= TEM_NUM)
	{
		return STS_DEV_ERROR; /* ��Ų��� */
	}

	switch(frame.cmd)
	{
	case TEM_CHECK:
		break;

	case TEM_START:			/* �������� */ 
		if(1 == g_tem[l_id].start)
		{
			return STS_BUZY; /* ���ڹ��� */
		}
		g_tem[l_id].start = 1;
		g_tem[l_id].status = 0;
		g_tem[l_id].dt = (frame.data[2]*256 + frame.data[3])/10.0;/* �¶Ⱦ���0.1�� */
		if((g_tem[l_id].dt > TEM_MAX_TEMP) || (g_tem[l_id].dt < TEM_MIN_TEMP))
		{
			temAppClear(l_id);
			return STS_DATA_ERROR;
		}		
		OSSemPost(g_tem[l_id].Event);
		break;

	case TEM_STOP:				/* ֹͣ���� */
//		if(0 == g_tem[l_id].start)
//		{
//			return STS_FAILED; /* ���ڹ��� */
//		}
		temAppClear(l_id);
		break;
	case TEM_READ:				/* ��ȡ�¶� */
		break;
	default:
		return STS_CMD_ERROR;
	}

	l_tem = 10 * g_temUpload;  /* �¶Ⱦ���Ϊ0.1�棬�����Ҫ����10 */

	frame.data[0] = 0;
	frame.data[1] = 0;
	frame.data[2] = (l_tem>>8) & 0xff;
	frame.data[3] = l_tem & 0xff;
	frame.para = 1;
	
	pcTxQueueAddFrame((pcFrame *)&frame); 
		
	return STS_SUCCESS;
}

/*
* ���¿���������
*/
void temTaskDeal(u8 p_id)
{
	s32 l_tem;
	pcFrame frame;
	frameInit((pcFrame *)&frame);
	FANON;
	frame.sts = temHeatCool(p_id);
	FANOFF;
	if(frame.sts)
	{		
		frame.cmd = TEM_STOP;
		frame.dev = p_id + 1;
		frame.para = 0x01;
		l_tem = 10 * g_temUpload;  /* �¶Ⱦ���Ϊ0.1�棬�����Ҫ����10 */ 
		frame.data[0] = 0;
		frame.data[1] = 0;
		frame.data[2] = (l_tem>>8) & 0xff;
		frame.data[3] = l_tem & 0xff;
		pcTxQueueAddFrame((pcFrame *)&frame);
	}
	temAppClear(p_id);
}



