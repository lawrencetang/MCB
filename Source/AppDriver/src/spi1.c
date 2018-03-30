/*
spi1.c

*/


#include "stm32f10x_spi.h"
#include "spi1.h" 


u16 g_spi1RxData;	 	//用于保存SPI1收到的数据
u8 g_spi1Flag;			//用于确定SPI1通信是否完整
 

/*
* SPI1初步化
* 初始化为中断发送
*/
void SPI1Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;

	g_spi1RxData = 0;
	g_spi1Flag = 0;

	//CS
	GPIO_InitStructure.GPIO_Pin = SPI1_PIN_CS;//lt:PA4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	//推挽输出
	GPIO_Init(SPI1_GPIO,&GPIO_InitStructure);

	//SCK
	GPIO_InitStructure.GPIO_Pin = SPI1_PIN_MISO | SPI1_PIN_SCK | SPI1_PIN_MOSI;//lt:PA6,PA5,PA7(未用)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   	//复用推挽输出
	GPIO_Init(SPI1_GPIO,&GPIO_InitStructure);


	// SPI1 Config
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//lt:方向：双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//lt:模式：主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;//lt:传输和接收16位帧格式
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//lt:串行时钟稳定状态：高有效
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//lt:时钟第一个上升沿捕捉就有效
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//lt:通过SSI位控制NSS信号   
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//lt:SCK时钟频率为128分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//lt:首先传输的是高位
	SPI_InitStructure.SPI_CRCPolynomial = 7;//lt:crc多项式7项
	SPI_Init(SPI1,&SPI_InitStructure);

	// SPI1 enable
	SPI_Cmd(SPI1,ENABLE);

	SPI1_CS_HIGH;//lt:置CS
}

/*
*  SPI1发送
*  写入数据，并打开接收中断
*/
u8 SPI1Send(u16 data)
{
	if((g_spi1Flag--) || (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)))//lt:发送缓冲器不空不能发送数据
	{
		return 1;
	}
	g_spi1Flag = 200;
	SPI1_CS_LOW;//lt:cs拉低
	SPI_I2S_ReceiveData(SPI1); 
	SPI_I2S_ReceiveData(SPI1);	
	SPI_I2S_SendData(SPI1, data);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
	return 0;	
}

/*
*  SPI1中断
*  只有发送数据时才会进入中断，在中断里面把接收的数据取出来
*/
void SPI1Isr(void)
{
	SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);//lt:清非空标志
	g_spi1RxData = SPI_I2S_ReceiveData(SPI1);	//绝对值编码器需求
	SPI1_CS_HIGH;
	g_spi1Flag = 0;	
}//lt:因为是全双工模式，发送时可同时接收数据到移位寄存器中，主机的移位寄存器发送完成后也完成了数据的接收




