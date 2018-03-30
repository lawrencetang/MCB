/*
spi2.c

*/


#include "stm32f10x_spi.h"
#include "spi2.h" 


u16 g_spi2RxData;	 	//用于保存SPI2收到的数据
u8 g_spi2Flag;			//用于确定SPI2通信是否完成
 

/*
* SPI2初步化
* 初始化为中断发送
*/
void SPI2Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;

	g_spi2RxData = 0;
	g_spi2Flag = 0;

	//CS
	GPIO_InitStructure.GPIO_Pin = SPI2_PIN_CS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	//推挽输出
	GPIO_Init(SPI2_GPIO,&GPIO_InitStructure);

	//SCK
	GPIO_InitStructure.GPIO_Pin = SPI2_PIN_MISO | SPI2_PIN_SCK | SPI2_PIN_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   	//复用推挽输出
	GPIO_Init(SPI2_GPIO,&GPIO_InitStructure);


	// SPI2 Config
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//lt:64分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2,&SPI_InitStructure);

	// SPI2 enable
	SPI_Cmd(SPI2,ENABLE);

	SPI2_CS_HIGH;
}

/*
*  SPI2发送
*  写入数据，并打开接收中断
*/
u8 SPI2Send(u16 data)
{
	if((g_spi2Flag--) || (!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)))
	{
		return 1;
	}
	g_spi2Flag = 200;
	SPI2_CS_LOW;
	SPI_I2S_ReceiveData(SPI2); 
	SPI_I2S_ReceiveData(SPI2);	
	SPI_I2S_SendData(SPI2, data);
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
	return 0;	
}

/*
*  SPI2中断
*  只有发送数据时才会进入中断，在中断里面把接收的数据取出来
*/
void SPI2Isr(void)
{
	SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
	g_spi2RxData = SPI_I2S_ReceiveData(SPI2);	//绝对值编码器需求
	SPI2_CS_HIGH;
	g_spi2Flag = 0;	
}




