#ifndef __USART_H
#define	__USART_H

#include "../../Public/system.h"
#include "stdio.h" 

void USART3_Init(u32 bound);
void USART1_Init(u32 bound);
void usart3_send(u8 *buf, int len);

#endif
