/*
my_ad.h
*/

#ifndef _MY_AD_H
#define _MY_AD_H

#include "stm32f10x_gpio.h"

#define P_T1_AD				GPIO_Pin_1		/* PA1������1 */			
#define P_T2_AD				GPIO_Pin_0		/* PA0������2 */
#define P_T3_AD				GPIO_Pin_3		/* PC3������3 */
#define P_T4_AD				GPIO_Pin_2		/* PC2������4 */
#define P_T5_AD				GPIO_Pin_1		/* PC1������5 */
#define P_T6_AD				GPIO_Pin_0		/* PC0������6 */
#define P_CH352_AD			GPIO_Pin_1		/* PB1����籶����CH352 */

#define CHANNEL_NUM 		7


void AD_Init(void);

extern u16 g_ad_data[CHANNEL_NUM];

extern u8 ad_flag;
extern u16 ad_data;

#endif




