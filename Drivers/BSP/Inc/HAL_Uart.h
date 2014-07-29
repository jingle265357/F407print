#ifndef __HAL_Uart_H
#define __HAL_Uart_H

////////////////////////////////////////////////////////////////////////////////////
#include "MyTypedef.h"
#include "STM32F4xx_HAL_CONF.h"
#include "STM32F4xx_HAL_UART.h"


#ifndef COM1
	#define COM1 0x00
#endif

#ifndef COM2
	#define COM2 0x01
#endif

#ifndef COM3
	#define COM3 0x02
#endif

bool ComOpen(u8 com, u32 baud, u8 width, u8 stop, u8 par);

//----��ʼ��UART----------------------------------------------------------------
void ComInit(u8 com, u32 baud, u8 width, u8 stop, u8 par);
//----��ȡ�����ֽ���------------------------------------------------------------
u16 ComGetRecvCnt(u8 com);
//----��ȡ����------------------------------------------------------------------
bool ComRecvDatas(u8 com, u8 *buff, u16 lenth);
//----������ջ�����------------------------------------------------------------
void ComClearRecvBuff(u8 com);
//----�����ַ���----------------------------------------------------------------
void ComSendDatas(u8 com, u8* buff, u16 lenth);


#endif
