
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//lt:开漏输出

    GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


void iicStart(void)
{   
    GPIO_SetBits(GPIOB,IIC_SDA_PIN); //sda=1;数据线拉高
    delayMS(2);
    GPIO_SetBits(GPIOB,IIC_SCL_PIN); //scl=1;时钟线拉高
    delayMS(2);
    GPIO_ResetBits(GPIOB,IIC_SDA_PIN); //sda=0;数据线拉低完成开始
    delayMS(2);
}


void iicStop(void)
{  
    GPIO_ResetBits(GPIOB,IIC_SDA_PIN); //sda=0;数据线拉低
    delayMS(2);
    GPIO_SetBits(GPIOB,IIC_SCL_PIN); //scl=1;时钟线拉高
    delayMS(2);
    GPIO_SetBits(GPIOB,IIC_SDA_PIN); //sda=1;数据线拉低完成结束
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
	}//lt:在第九个时钟信号时，在一定时间内读数据线，拉低表示确认，拉高表示未确认
    GPIO_ResetBits(GPIOB,IIC_SCL_PIN);//scl=0;
    delayMS(2);
}


void iicWriteByte(u8 a)
{
    u16 i;
    GPIO_ResetBits(GPIOB,IIC_SCL_PIN);  //scl=0;lt:时钟线拉低
    delayMS(2);
    for(i=0;i<8;i++)
    { 
        if(a&0x80)  
            GPIO_SetBits(GPIOB,IIC_SDA_PIN);//sda=1;
        else
            GPIO_ResetBits(GPIOB,IIC_SDA_PIN);	//sda = 0
        a=a<<1;//lt:时钟线拉低期间，数据进行变化,先传送高位MSB
        GPIO_SetBits(GPIOB,IIC_SCL_PIN);//scl=1;lt:时钟线拉高读数据
        delayMS(2);
        GPIO_ResetBits(GPIOB,IIC_SCL_PIN);//scl=0;lt:时钟线拉低进行下一位数据的的变化和读
        delayMS(2);    
    }   
    GPIO_SetBits(GPIOB,IIC_SDA_PIN);//sda=1; 
    delayMS(2);//lt:和确认信号相隔4ms
}


u8 iicReadByte()
{
    u8 i,temp;
    temp=0;//lt:清空缓存
    GPIO_SetBits(GPIOB,IIC_SDA_PIN);//sda=1;
    delayMS(2);
    GPIO_ResetBits(GPIOB,IIC_SCL_PIN);      //scl=0;
    delayMS(2);
    for(i=0;i<8;i++)
    {
        GPIO_SetBits(GPIOB,IIC_SCL_PIN);  //scl=1;//lt:时钟线拉高准备读数据
        delayMS(2);
        temp=(temp<<1)|GPIO_ReadInputDataBit(GPIOB,IIC_SDA_PIN);  //read sda
        delayMS(2);
        GPIO_ResetBits(GPIOB,IIC_SCL_PIN);  //scl=0;//lt:时钟线拉低准备数据
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
    	iicStart();//lt:开始
    	iicWriteByte(EEPROM_WRITE);//lt:写一个地址+写位，地址为1010 001，写位为0
    	iicAck();
    	iicWriteByte(((add+i)>>8) & 0xff);//lt:写eeprom地址高位
		iicAck();
		iicWriteByte((add+i) & 0xff);//lt:写eeprom地址低位
    	iicAck();
		    
		iicWriteByte(buffer[i]);//lt:写数据到上面确定的eeprom地址
    	iicAck();

    	iicStop();
	}//lt:可一次写多个地址的数据,byte write模式
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
		iicWriteByte((add+i) & 0xff);//lt:写epprom地址
    	iicAck();
    	iicStart();
    	iicWriteByte(EEPROM_READ);//lt:写一个地址+读位，地址为1010 001， 读位为1
    	iicAck(); 
	
    	buffer[i] = iicReadByte(); //lt:读数据到buffer
		iicStop();

	}//lt:可以一次读多个数据,random read模式 
    
    return i;
}











