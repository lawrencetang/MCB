/*
spi1.c

*/


#include "stm32f10x_spi.h"
#include "spi1.h" 


u16 g_spi1RxData;	 	//���ڱ���SPI1�յ�������
u8 g_spi1Flag;			//����ȷ��SPI1ͨ���Ƿ�����
 

/*
* SPI1������
* ��ʼ��Ϊ�жϷ���
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	//�������
	GPIO_Init(SPI1_GPIO,&GPIO_InitStructure);

	//SCK
	GPIO_InitStructure.GPIO_Pin = SPI1_PIN_MISO | SPI1_PIN_SCK | SPI1_PIN_MOSI;//lt:PA6,PA5,PA7(δ��)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   	//�����������
	GPIO_Init(SPI1_GPIO,&GPIO_InitStructure);


	// SPI1 Config
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//lt:����˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//lt:ģʽ����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;//lt:����ͽ���16λ֡��ʽ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//lt:����ʱ���ȶ�״̬������Ч
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//lt:ʱ�ӵ�һ�������ز�׽����Ч
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//lt:ͨ��SSIλ����NSS�ź�   
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//lt:SCKʱ��Ƶ��Ϊ128��Ƶ
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//lt:���ȴ�����Ǹ�λ
	SPI_InitStructure.SPI_CRCPolynomial = 7;//lt:crc����ʽ7��
	SPI_Init(SPI1,&SPI_InitStructure);

	// SPI1 enable
	SPI_Cmd(SPI1,ENABLE);

	SPI1_CS_HIGH;//lt:��CS
}

/*
*  SPI1����
*  д�����ݣ����򿪽����ж�
*/
u8 SPI1Send(u16 data)
{
	if((g_spi1Flag--) || (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)))//lt:���ͻ��������ղ��ܷ�������
	{
		return 1;
	}
	g_spi1Flag = 200;
	SPI1_CS_LOW;//lt:cs����
	SPI_I2S_ReceiveData(SPI1); 
	SPI_I2S_ReceiveData(SPI1);	
	SPI_I2S_SendData(SPI1, data);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
	return 0;	
}

/*
*  SPI1�ж�
*  ֻ�з�������ʱ�Ż�����жϣ����ж�����ѽ��յ�����ȡ����
*/
void SPI1Isr(void)
{
	SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);//lt:��ǿձ�־
	g_spi1RxData = SPI_I2S_ReceiveData(SPI1);	//����ֵ����������
	SPI1_CS_HIGH;
	g_spi1Flag = 0;	
}//lt:��Ϊ��ȫ˫��ģʽ������ʱ��ͬʱ�������ݵ���λ�Ĵ����У���������λ�Ĵ���������ɺ�Ҳ��������ݵĽ���




