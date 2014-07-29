#ifndef __HalUart_H
#define __HalUart_H

#include "MyTypedef.h"
#include "STM32F4xx_HAL_CONF.h"

////////////////////////////////////////////////////////////////////////////////////
//����ͨѶ����
////////////////////////////////////////////////////////////////////////////////////
#define COM1 0x00
#define COM2 0x01
#define COM3 0x02

//RS232ͨѶ
void com_init(u8 com, u32 baud, u8 width, u8 stop, u8 par);
u16 com_package(u8 com);					//���UART1�������ݰ�
u16 com_ndata(u8 com);
u8 com_getc(u8 com);
bool com_read(u8 com, u8 *buff, vu16 lenth);
bool com_read2(u8 com, u8 *buff, vu16 lenth);
void com_clear(u8 com);
void com_putc(u8 com, u8 ch);
void com_write(u8 com, u8* buff, vu16 lenth);
void com_autoclear(u8 com, bool f);				//��������Զ��������˫��485��ʽ��


//////////////////////////////////////////////////////////////////////////////////	 
#endif
