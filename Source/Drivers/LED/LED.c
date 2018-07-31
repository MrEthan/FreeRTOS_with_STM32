#include "stm32f10x.h"
#include "LED.H"

/*******************************************************************************
* 函 数 名         : LED_Init
* 函数功能		   : LED初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin= LED_PIN;  //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(GPIOC, &GPIO_InitStructure); 	   /* 初始化GPIO */
	
	GPIO_SetBits(GPIOC, LED_PIN);   //将LED端口拉高，熄灭所有LED
  return;
}

void led_toggle_0(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_0, (BitAction)(1 - GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)));
    return;
}

void led_toggle_1(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, (BitAction)(1 - GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)));
    return;
}
