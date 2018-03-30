/*
adcDriver.c
所有AD采集相关的操作
*/

#include "adcDriver.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x.h"

ADC_DATA g_ad;

/*
AD外设初始化
配置AD为6个通道循环采集，采用DMA传输
*/
void AD_DeviceInit(void)
{
	ADC_InitTypeDef ADC_InitStructure;	
	
	ADC_DeInit(ADC1); //将外设 ADC1 的全部寄存器重设为缺省值  
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //模数转换工作在扫描模式 //lt:只有一个通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //外部触发转换关闭
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = CHANNEL_NUM; //顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
	
	
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5 );   //温度1

	
	// 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
	ADC_DMACmd(ADC1, ENABLE);//lt:外设发信号给DMA控制器
	
	
	ADC_Cmd(ADC1, ENABLE); //使能指定的ADC1
	
	ADC_ResetCalibration(ADC1); //复位指定的ADC1的校准寄存器
	
	while(ADC_GetResetCalibrationStatus(ADC1)); //获取ADC1复位校准寄存器的状态,设置状态则等待
	
	ADC_StartCalibration(ADC1); //开始指定ADC1的校准状态
	
	while(ADC_GetCalibrationStatus(ADC1)); //获取指定ADC1的校准程序,设置状态则等待 
}


/*
AD的引脚初始化
*/
void AD_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = ADC_PIN_TEM;  //lt:PC0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/*
配置ADC2用DMA传输
6个通道的数据传输到内存的g_ad_data数组中
*/
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&g_ad; //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
	DMA_InitStructure.DMA_BufferSize = CHANNEL_NUM; //DMA通道的DMA缓存的大小 //lt:配置传输数量寄存器
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道 
}

/*
* AD初始化
* 包括：引脚、DMA、AD
*/
void adcInit(void)
{
	AD_IOInit();	
	DMA_Configuration();
	AD_DeviceInit();	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);   //启动AD采集
	DMA_Cmd(DMA1_Channel1, ENABLE); //启动DMA通道	   
}



