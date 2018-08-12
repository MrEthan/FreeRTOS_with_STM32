#include "stm32f10x.h"
#include "usart.h"

//����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
int fputc(int ch, FILE *p)
{
	USART_SendData(USART3,(u8)ch);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);

	return ch;
}

void usart3_send(u8 *buf, int len)
{
    int i;
    for (i = 0; i < len; i++){
        USART_SendData(USART3, buf[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
    return;
}

void usart3_send_char(u8 ch)
{
    USART_SendData(USART3, ch);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    return;
}
/*******************************************************************************
* �� �� ��         : USART1_Init
* ��������		   : USART1��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/
void USART1_Init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //��ʱ��


    /*  ����GPIO��ģʽ��IO�� */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //�������PA9
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
    GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //��������PA10
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
    GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */


    //USART1 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1

    USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1
    USART_GetFlagStatus(USART1, USART_FLAG_TC); //Ӳ����λ�󣬶�ȡsr��дdr���tc��־λ�������һ���ֽ����ݻᱻ����
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

/*******************************************************************************
* �� �� ��         : USART1_IRQHandler
* ��������		   : USART1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 r;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		r =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		USART_SendData(USART1,r);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
	}
	USART_ClearFlag(USART1,USART_FLAG_TC);
}

//USART3��ʼ��
void USART3_Init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // Configure USART2 Rx (PB.11) as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure USART2 Tx (PB.10) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);
    USART_GetFlagStatus(USART3, USART_FLAG_TC); //Ӳ����λ�󣬶�ȡsr��дdr���tc��־λ�������һ���ֽ����ݻᱻ����
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    //Configure the NVIC Preemption Priority Bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // Enable the USART3 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

extern void fill_rec_buf_isr(char data);
/*******************************************************************************
* �� �� ��         : USART1_IRQHandler
* ��������		   : USART1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void USART3_IRQHandler(void)                	//����1�жϷ������
{
	u8 r;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		r = USART_ReceiveData(USART3); 	//��ȡ���յ�������
        fill_rec_buf_isr(r);
	}else if (USART_GetFlagStatus(USART3, USART_IT_ORE) != RESET){ //��������־
        USART_ReceiveData(USART3);
    }
}

