/*
lightControl.c
믵ƿ��������ʱ����ơ�
믵ƺ�CMOS�Ļ���ʱ���ɶ�ʱ������
��ʱ���̶�����Ϊ100us��������ڹ̶�Ϊ15ms��
*/

#include "lightControl.h"

lightCtrl g_lightCtrl;
cmosCtrl g_cmosCtrl;;

void lightInit(void)
{
	g_lightCtrl.flag = 0;
	g_lightCtrl.counter = 0;
	g_lightCtrl.period = FrequentToIrqCnt(LIGHT_MAX_FREQUENT); /* ���õ�����ڣ������Ƶ�ʣ����Դ�EEPROM�ж�ȡ��Ҳ���Թ̶� */

	if(g_lightCtrl.period < FrequentToIrqCnt(LIGHT_MAX_FREQUENT))
	{
		g_lightCtrl.period = FrequentToIrqCnt(LIGHT_MAX_FREQUENT);
	}

	g_cmosCtrl.flag = 0;
	g_cmosCtrl.counter = 0;
	g_cmosCtrl.totalCnt = TimeToIrqCnt(ST_DEFAULT_TIME); 	/* ����ʱ�� */
}

/* ��ʱ���жϵ��ñ����� */
void lightCtrlTick(void)
{	
	g_lightCtrl.counter++;

	if(g_lightCtrl.counter >= (g_lightCtrl.period - 1))
	{
		g_lightCtrl.counter = 0;
		if(g_lightCtrl.flag)
		{/* �򿪵ƿ������ţ����� */
		}	
	}
	else if(g_lightCtrl.counter >= (g_lightCtrl.period/2))
	{
		/* �رյƿ������ţ��ص� */
	}

	if(g_cmosCtrl.flag)
	{/* CMOS���ڽ������ݲɼ� */
		if(g_cmosCtrl.counter < g_cmosCtrl.totalCnt)
		{
			g_cmosCtrl.counter++;
		}
		else
		{
			/* CMOS����ʱ�䵽��ST = 0 */
		}
	}
	else
	{/* CMOS���У��жϵ��ʱ�䣬��ƵĿ�ʼ����ض�ʱ������һ�βɼ����� */
		if(g_lightCtrl.counter == ST_DELAY)
		{
			g_cmosCtrl.flag = 1; 	/* ��2048�����ݶ�ȡ���ʱ���Զ����ñ�ʶ��0 */
			g_cmosCtrl.counter = 0;
			/* CMOS��ʼ���֣�ST = 1 */	
		}
	}
}






