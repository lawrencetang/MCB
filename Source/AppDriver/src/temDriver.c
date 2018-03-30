/*
temperature_drive.c

����ϵͳ�ײ�����ʵ��
��Ҫ���ڶ�����ȡ��������ţ�ʵ�ּ�����������ơ�

*/

#include "temDriver.h"


TEM_IO_CTRL g_temx_io[TEM_NUM];


/*
�����������
���ȣ�C1 = 0 �� C2 = 1
���䣺C1 = 1 �� C2 = 0
�رգ�C1 = 0 �� C2 = 0  (����������1ʱҲ��ֹͣ)

SD = 0 ʱ��ֹͣ����
SD = 1 ʱ����������
*/



/* 
��ʼ��
�������¶ȿ�����������ΪOUT_PPģʽ
*/
void temPinInit(void)
{							   
	u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;
   
	/* �¿�����õ��� PD5-PD7 */
	GPIO_InitStructure.GPIO_Pin = TEM_PIN_C1 | TEM_PIN_C2 | TEM_PIN_SD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//lt:�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	g_temx_io[TEM1].C1 = TEM_PIN_C1;//lt:PD7
	g_temx_io[TEM1].C2 = TEM_PIN_C2;//lt:PD6
	g_temx_io[TEM1].SD = TEM_PIN_SD;//lt:PD5


	/* �ر������¿� */
	for(i = TEM1; i < TEM_NUM; i++)
	{
		temStop(i);
	}
}



/* 
����
��������Ĳ���ʹ��Ӧ���¿ؼ���
p_id����·�¿��е�һ·��ȡֵ0-5������ֵ������
����ֵ������0 ��������1��ʾ������¿�ID�������⡣
*/
u8 temHeat(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].SD); 		/* SD = 1 �ر� */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C1); 		/* C1 = 0 */
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].C2); 		/* C2 = 1 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].SD);		/* SD = 0 ���� */
	return 0;
}

/* 
����
��������Ĳ���ʹ��Ӧ���¿�����
p_id����·�¿��е�һ·��ȡֵ0-5������ֵ������
����ֵ������0 ��������1��ʾ������¿�ID�������⡣
*/
u8 temCool(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].SD); 		/* SD = 0 �ر� */
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].C1); 		/* C1 = 1 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C2); 		/* C2 = 0 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].SD);		/* SD = 1 ���� */ //lt:�ߵ�ƽʱHO��LO����INʱ��
	return 0;
}


/* 
ֹͣ
��������Ĳ���ʹ��Ӧ���¿�ֹͣ
p_id����·�¿��е�һ·��ȡֵ0-5������ֵ������
����ֵ������0 ��������1��ʾ������¿�ID�������⡣
*/
u8 temStop(u8 p_id)
{
	if(p_id >= TEM_NUM)
	{
		return 1;
	}
	GPIO_SetBits	(TEM_GPIOX, g_temx_io[p_id].SD); 		/* SD = 0 �ر� */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C1); 		/* C1 = 0 */
	GPIO_ResetBits	(TEM_GPIOX, g_temx_io[p_id].C2); 		/* C2 = 0 */
	return 0;
}


