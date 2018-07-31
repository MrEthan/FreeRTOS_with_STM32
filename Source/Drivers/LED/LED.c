#include "stm32f10x.h"
#include "LED.H"

/*******************************************************************************
* �� �� ��         : LED_Init
* ��������		   : LED��ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin= LED_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(GPIOC, &GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	
	GPIO_SetBits(GPIOC, LED_PIN);   //��LED�˿����ߣ�Ϩ������LED
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
