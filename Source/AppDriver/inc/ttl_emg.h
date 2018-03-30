/*
ttl_emg.h
TTL输出与电磁阀控制
*/

#ifndef _TTL_EMG_H
#define _TTL_EMG_H

#include "stm32f10x_type.h"
#include "my_frame.h"
#include "stm32f10x_gpio.h"

/* TTL电平输出引脚 */
#define P_TTL1				GPIO_Pin_6		/* PD6，TTL1 */			
#define P_TTL2				GPIO_Pin_7		/* PD7，TTL2 */
#define P_TTL3				GPIO_Pin_9		/* PG9，TTL3 */
#define P_TTL4				GPIO_Pin_10		/* PG10，TTL4 */
#define P_TTL5				GPIO_Pin_14		/* PG14，TTL5 */
#define P_TTL6				GPIO_Pin_13		/* PG13，TTL6 */
#define P_TTL7				GPIO_Pin_12		/* PG12，TTL7 */
#define P_TTL8				GPIO_Pin_11		/* PG11，TTL8 */


/* 电磁阀输出引脚 */
#define P_EMG1				GPIO_Pin_1		/* PG1，EMG1 */	
#define P_EMG2				GPIO_Pin_0		/* PG0，EMG1 */	


extern u16 ttl_emg[10];


void TTL_EMG_Init(void);

void TTL_EMG_Out1(u8 p_out);

void TTL_EMG_Out0(u8 p_out);

u8 TTL_EMG_Set(Original_Frame* o_frame);	

#endif

