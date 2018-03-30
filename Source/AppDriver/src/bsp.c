#define  BSP_GLOBALS
#include <includes.h>

#include "system_stm32f10x.h"

#include "stm32f10xDriver.h"
#include "AppDriver.h"

#include "TaskInit.h"

#include "usb_lib.h"
#include "hw_config.h"


#define ADC1_DR_Address    ((u32)0x4001244C)
		   


ErrorStatus HSEStartUpStatus;

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);//lt:HSE打开

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//lt:使能flash预取指缓存

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);//lt:设置代码延时两周期
  
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); //lt:前面systeminit()已配置,设置AHB分频
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); //lt:前面systeminit()已配置

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2); //lt:前面systeminit()已配置

    /* ADCCLK = PCLK2/4 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
    
    /* PLLCLK = 8MHz * 7 = 56 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//lt:PLL时钟源为HSE时钟,7倍频//ltg

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);	

  }
}






/*
*********************************************************************************************************
* 函数名: BSP_Init
*
* 功能   ：用来初始化硬件上的配置
*********************************************************************************************************
*/
void BSP_Init(void)
{
	 /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	 /* 本系统中，系统主时钟为72MHz，PCLK1 = 36MHz，PCLK2 = 72MHz */
	SystemInit();//lt:配置系统主时钟为72mhz，该启动文件没有调用systeminit函数,define為STM32F10X_HD
	
	RCC_Configuration();//lt:配置系统时钟为56MHz?


	 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_CRC, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1  | RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD 
						 | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);
	


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);//lt:使能外围设备时钟
	
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);//lt:选择USB分频1.5，usb频率48MHz

	powerON();

	USB_Interrupts_Config();
	USB_Init();

	ledINIT();
	fanInit();
	iicInit();
	uart1Init();
	absEncInit();
	smInit();
	dcmInit();	
	temAppInit();
//	Timer1Init();
	Timer2Init();
	Timer3Init();   
//	Timer4Init();


	NVIC_Configuration(); // 优先级配置，需要配置在232、422更新程序前	
 
    RCC_ClearFlag(); // 清RCC标志
}




