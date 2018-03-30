
/*
eeprom.c

*/
#include "ucos_ii.h"
#include "eeprom.h"
#include "common.h"

void iicInit(void)
{       
    GPIO_InitTypeDef  GPIO_InitStructure;
 
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN | IIC_SDA_PIN;//lt:PB6 and PB7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//lt:��©���

    GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


void iicStart(void)
{   
    GPIO_SetBits(GPIOB,IIC_SDA_PIN); //sda=1;����������
    delayMS(2);
    GPIO_SetBits(GPIOB,IIC_SCL_PIN); //scl=1;ʱ��������
    delayMS(2);
    GPIO_ResetBits(GPIOB,IIC_SDA_PIN); //sda=0;������������ɿ�ʼ
    delayMS(2);
}


void iicStop(void)
{  
    GPIO_ResetBits(GPIOB,IIC_SDA_PIN); //sda=0;����������
    delayMS(2);
    GPIO_SetBits(GPIOB,IIC_SCL_PIN); //scl=1;ʱ��������
    delayMS(2);
    GPIO_SetBits(GPIOB,IIC_SDA_PIN); //sda=1;������������ɽ���
    delayMS(2);
}


void iicAck(void)
{      
    u16 i = 0;
    GPIO_SetBits(GPIOB,IIC_SCL_PIN);//scl=1;
    delayMS(2);
    while(GPIO_ReadInputDataBit(GPIOB,IIC_SDA_PIN)&&(i<0x2b0)) 
	{
		i++;
	}//lt:�ڵھŸ�ʱ���ź�ʱ����һ��ʱ���ڶ������ߣ����ͱ�ʾȷ�ϣ����߱�ʾδȷ��
    GPIO_ResetBits(GPIOB,IIC_SCL_PIN);//scl=0;
    delayMS(2);
}


void iicWriteByte(u8 a)
{
    u16 i;
    GPIO_ResetBits(GPIOB,IIC_SCL_PIN);  //scl=0;lt:ʱ��������
    delayMS(2);
    for(i=0;i<8;i++)
    { 
        if(a&0x80)  
            GPIO_SetBits(GPIOB,IIC_SDA_PIN);//sda=1;
        else
            GPIO_ResetBits(GPIOB,IIC_SDA_PIN);	//sda = 0
        a=a<<1;//lt:ʱ���������ڼ䣬���ݽ��б仯,�ȴ��͸�λMSB
        GPIO_SetBits(GPIOB,IIC_SCL_PIN);//scl=1;lt:ʱ�������߶�����
        delayMS(2);
        GPIO_ResetBits(GPIOB,IIC_SCL_PIN);//scl=0;lt:ʱ�������ͽ�����һλ���ݵĵı仯�Ͷ�
        delayMS(2);    
    }   
    GPIO_SetBits(GPIOB,IIC_SDA_PIN);//sda=1; 
    delayMS(2);//lt:��ȷ���ź����4ms
}


u8 iicReadByte()
{
    u8 i,temp;
    temp=0;//lt:��ջ���
    GPIO_SetBits(GPIOB,IIC_SDA_PIN);//sda=1;
    delayMS(2);
    GPIO_ResetBits(GPIOB,IIC_SCL_PIN);      //scl=0;
    delayMS(2);
    for(i=0;i<8;i++)
    {
        GPIO_SetBits(GPIOB,IIC_SCL_PIN);  //scl=1;//lt:ʱ��������׼��������
        delayMS(2);
        temp=(temp<<1)|GPIO_ReadInputDataBit(GPIOB,IIC_SDA_PIN);  //read sda
        delayMS(2);
        GPIO_ResetBits(GPIOB,IIC_SCL_PIN);  //scl=0;//lt:ʱ��������׼������
        delayMS(2);
    }
    GPIO_SetBits(GPIOB,IIC_SDA_PIN);//sda=1;
    delayMS(2);
    GPIO_ResetBits(GPIOB,IIC_SCL_PIN);//scl=0;
    delayMS(2);
    return temp;
}

void iicWrite(u16 add, u8 *buffer, u16 len)
{
	u16 i;

	for(i=0; i<len; i++)
	{
    	iicStart();//lt:��ʼ
    	iicWriteByte(EEPROM_WRITE);//lt:дһ����ַ+дλ����ַΪ1010 001��дλΪ0
    	iicAck();
    	iicWriteByte(((add+i)>>8) & 0xff);//lt:дeeprom��ַ��λ
		iicAck();
		iicWriteByte((add+i) & 0xff);//lt:дeeprom��ַ��λ
    	iicAck();
		    
		iicWriteByte(buffer[i]);//lt:д���ݵ�����ȷ����eeprom��ַ
    	iicAck();

    	iicStop();
	}//lt:��һ��д�����ַ������,byte writeģʽ
}
u16 iicRead(u16 add, u8 *buffer, u16 len)
{
    u16 i;
    for(i=0; i<len; i++)
	{
		iicStart();
    	iicWriteByte(EEPROM_WRITE);
   		iicAck();
    	iicWriteByte(((add+i)>>8) & 0xff);
		iicAck();
		iicWriteByte((add+i) & 0xff);//lt:дepprom��ַ
    	iicAck();
    	iicStart();
    	iicWriteByte(EEPROM_READ);//lt:дһ����ַ+��λ����ַΪ1010 001�� ��λΪ1
    	iicAck(); 
	
    	buffer[i] = iicReadByte(); //lt:�����ݵ�buffer
		iicStop();

	}//lt:����һ�ζ��������,random readģʽ 
    
    return i;
}











