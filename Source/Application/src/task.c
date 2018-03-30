/*************************************
���������Ӧ�ò���صľ��ڱ��ļ���

yshy119
2014��3��31��
*************************************/


#include "TaskInit.h"

#include "stm32f10xDriver.h"
#include "usbDriver.h"
#include "AppDriver.h"

#include "task.h"

#include "os_cpu.h"





/*
������Ӧ���񣬼�������λ����������ָ�
*/
void task1(void* pdata)
{
	u8 err = 0;
	pcFrame frame;

	// �ȴ���ʼ������
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);//lt:�˳�����̬20��ʱ�ӽ���
	}//lt:δ�������������������������ڸ�ѭ����


	while(1)
	{
		if(pcRxQueueOutFrame((pcFrame *)&frame))
		{
			OSSemPend(g_EventUSBRx, 1000, &err);//lt:����ǰ�������״̬ȡ����д��ȴ������
			continue ;//lt:����������post���źŲ��ܸ�������ܽ��н����ж�
		}//lt:δ���ջ��߲���ȷ���ս����ж�

//		LEDREV();

		if(frame.flag != DOWN_FRAME)/*�ж�����֡����*/
		{
			continue ;
		}  //lt:��������ѭ���������´�ѭ��

		switch(frame.cmd & 0xf0)  //lt:�ж��������࣬������4λ
		{
		case COM_TEST:  //lt:0x00 ��������
			CMDTestDeal(frame);
			break;

		case SM_CMD: /* �����������ָ�� */  //lt:0x80
			frame.sts = smSetCMD(frame);
			if(frame.sts)  //lt:��ָ��ʧ��ִ�У��ش�����
			{
				pcTxQueueAddFrame((pcFrame *)&frame);
			}
			break;

		case DCM_CMD:  //lt:0x90 ֱ�����ָ��
			frame.sts = dcmSetCMD(frame);
			pcTxQueueAddFrame((pcFrame *)&frame);			
			break;

		case TEM_CMD:  //lt:0xA0 �¿�ָ��
			frame.sts = temCMDDeal(frame);
			if(frame.sts)
			{
				pcTxQueueAddFrame((pcFrame *)&frame);
			}
			break;
			
		case BOARD_STATUS:  //lt:0x70 ��ѯָ��
			boardStatusCMD(frame);
			break;

		case ENCRYPT_CMD:  //lt:0xD0 �����豸ָ��
			encryptCMDDeal(frame);
			break;

		default:
			frame.sts = STS_CMD_ERROR;	//lt:����ָ������ǣ�����0x03״ָ̬�����	
			pcTxQueueAddFrame((pcFrame *)&frame);
			break;
		}
		
		memset((u8*)&frame, 0, FRAME_LEN); 
	}
}

/*************************************************
****************  ͨ�ŷ�������  ******************
********************  SM1  **********************/
void task2(void* pdata)
{
	u8 err = 0;

	// �ȴ���ʼ������
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);
	}


	while(1)
	{
		OSSemPend(g_EventUSBTx, 1000, &err);//lt:����ǰ�������״̬ȡ����д��ȴ������

		LEDREV();//lt:���пյ�ʱ�򣬲��ڵȴ����ڵȴ���·��

		while(pcTxQueueLen())
		{
			LEDREV();
			pcSendTxQueue();//lt:��������������ָ�����������ȡ������֮�󣬷�������λ��֮ǰ�����ᣬ-->
			//lt:-->�¼����ƿ��м���ֵΪ2�����ڶ�������ָ��Ļظ�������ɺ����ֵΪ1�������Ҫ�շ�һ������������ܽ���ȴ�״̬��
		}//lt:���������ݾ�Ҫ���͵�usb������
	}
}




/*************************************************
*****************  �������1  ********************
********************  SM1  **********************/
void task3(void* pdata)
{ 
	u8 err;	
	
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);/* �ȴ���ʼ������ */
	}

	Timer3Start();

	while(1)
	{
		OSSemPend(g_sm[SM1].para.event, 500, &err);//lt:���ź�post��err=0		
		smUpdateAngle(SM1);
		if(0 == err)
		{
			smRotate(SM1);//lt:���ź�post����øú���		
		}		
	}
}

/*************************************************
*****************  �������2  ********************
********************  SM2  **********************/
void task4(void* pdata)
{ 
	u8 err;
	
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);/* �ȴ���ʼ������ */
	}

	while(1)
	{
		OSSemPend(g_sm[SM2].para.event, 500, &err);		
		smUpdateAngle(SM2);
		if(0 == err)
		{
			smRotate(SM2);				
		}		
	}
}


/*************************************************
*****************  ��������  *********************
********************  TEM  **********************/
void task5(void* pdata)
{ 
	u8 err;
	
	while (g_u8StartOver == START_NOT_OVER)
	{
		OSTimeDly(TAST_BEGIN_WAIT_TICKS);/* �ȴ���ʼ������ */
	}
	while(1)
	{
		OSSemPend(g_tem[TEM1].Event, 500, &err);
		
		temAppGetTem(TEM1);		

		if(0 == err)
		{
			temTaskDeal(TEM1);	
		}		
	}
}

