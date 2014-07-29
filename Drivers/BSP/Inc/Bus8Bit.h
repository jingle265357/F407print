#ifndef __HalBus8Bit_H
#define __HalBus8Bit_H 			   
//////////////////////////////////////////////////////////////////////////////////	 
#include "MyTypedef.h"
//#include "MidSystemClock.h"

#include "STM32F4xx_HAL_CONF.h"

////////////////////////////////////////////////////////////////////////////////// 
void HAL_Bus8BitInit(u16 WaitTime);		//���߽ӿڳ�ʼ��

void HAL_Bus8BitWriteCtr(u16 ch);		//����д�����ֽ�
void HAL_Bus8BitWriteData(u16 ch);		//����д�����ֽ�
u16 HAL_Bus8BitReadCtr(void);			//���߶������ֽ�
u16 HAL_Bus8BitReadData(void);			//���߶������ֽ�



#endif

