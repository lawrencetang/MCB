 /*
 2012.01.15
 仅仅用来测试用的程序，为了编译通过，不是正式程序，因此未写函数体
 */
#include "stm32f10x.h"
#include "my_include.h"

#ifndef _MY_TEMP_HEADFILE_H
#define _MY_TEMP_HEADFILE_H



void my_usb_send(Original_Frame o_frame);
u16 ssi_read(GPIO_TypeDef *my_gpio, u16 cs_pin, u16 clk_pin, u16 data_pin, u32 len);

//步进电机转动测试
void step_motor_control_just_run(u8 smotor, u16 data)
{
	u32 i;

	GPIO_ResetBits(GPIOE, STEP_MOTOR_A_EN); /* sm1_en = 1 */
	
	//GPIO_SetBits(GPIOE, STEP_MOTOR_A_DIR); /* sm1_dir = 0 */
	GPIO_ResetBits(GPIOE, STEP_MOTOR_A_DIR); /* sm1_dir = 1 */

	for(i=0; i< data; i++)
	{
		GPIO_SetBits(GPIOE, STEP_MOTOR_A_STP); /* sm1_stp = 0 */
		delayus(200);
		GPIO_ResetBits(GPIOE, STEP_MOTOR_A_STP);  /* sm1_stp = 1 */
		delayus(200);
	}
	GPIO_SetBits(GPIOE, STEP_MOTOR_A_EN); /* sm1_en = 0 */
}



#endif
