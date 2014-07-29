#ifndef __HalDmaQueue_H
#define __HalDmaQueue_H

////////////////////////////////////////////////////////////////////////////////////
#include "STM32F4xx_HAL_CONF.h"
#include "MyTypedef.h"

//DMA���нṹ
typedef struct {
    __IO u8	*out; 		//ָ���������λ�� 
    __IO u8	*sta;		//ָ��Buf�Ŀ�ʼλ��
    __IO u8	*end;		//ָ��Buf�Ľ���λ��
    __IO u16 ndata;		//��������Ч�����ֽ���   
    __IO u16 space;		//���е������ֽڿռ�
	__IO u16 LastDmaCurrCnt;	//DMA���һ��ʣ��Ĵ�����������Ŀ
}QueuePrar;

//--------------------------------------------------------------------------
void QueueCreate(u8 *databuff, u16 space, QueuePrar *Qprar);
u16 QueueNdata(u16 DmaCurrCnt, QueuePrar *Qprar);
char QueueGetch(QueuePrar *Qprar);
bool QueueRead(u8 *databuff, QueuePrar *Qprar, __IO u16 lenth);
bool QueueRead2(u8 *databuff, QueuePrar *Qprar, __IO u16 lenth);
void QueueFlush(__IO u16 ndata, QueuePrar *Qprar);										//�������

//////////////////////////////////////////////////////////////////////////////////	 
#endif
