/*
adcDriver.c
����AD�ɼ���صĲ���
*/

#include "adcDriver.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x.h"

ADC_DATA g_ad;

/*
AD�����ʼ��
����ADΪ6��ͨ��ѭ���ɼ�������DMA����
*/
void AD_DeviceInit(void)
{
	ADC_InitTypeDef ADC_InitStructure;	
	
	ADC_DeInit(ADC1); //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ  
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //ģ��ת��������ɨ��ģʽ //lt:ֻ��һ��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //ģ��ת������������ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�ⲿ����ת���ر�
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = CHANNEL_NUM; //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure); //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
	
	
	//����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	//ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5 );   //�¶�1

	
	// ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
	ADC_DMACmd(ADC1, ENABLE);//lt:���跢�źŸ�DMA������
	
	
	ADC_Cmd(ADC1, ENABLE); //ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1); //��λָ����ADC1��У׼�Ĵ���
	
	while(ADC_GetResetCalibrationStatus(ADC1)); //��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�
	
	ADC_StartCalibration(ADC1); //��ʼָ��ADC1��У׼״̬
	
	while(ADC_GetCalibrationStatus(ADC1)); //��ȡָ��ADC1��У׼����,����״̬��ȴ� 
}


/*
AD�����ų�ʼ��
*/
void AD_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = ADC_PIN_TEM;  //lt:PC0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/*
����ADC2��DMA����
6��ͨ�������ݴ��䵽�ڴ��g_ad_data������
*/
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1); //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&g_ad; //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
	DMA_InitStructure.DMA_BufferSize = CHANNEL_NUM; //DMAͨ����DMA����Ĵ�С //lt:���ô��������Ĵ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ�� 
}

/*
* AD��ʼ��
* ���������š�DMA��AD
*/
void adcInit(void)
{
	AD_IOInit();	
	DMA_Configuration();
	AD_DeviceInit();	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);   //����AD�ɼ�
	DMA_Cmd(DMA1_Channel1, ENABLE); //����DMAͨ��	   
}



