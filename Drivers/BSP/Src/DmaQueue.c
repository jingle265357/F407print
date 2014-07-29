#include "DmaQueue.h"

//--------------------------------------------------------------------------
void QueueCreate(u8 *databuff, u16 space, QueuePrar *Qprar)
{
	Qprar->space = space;					//								
	Qprar->LastDmaCurrCnt = Qprar->space;	//DMA���һ��ʣ��Ĵ�����������Ŀ
	Qprar->sta = databuff;								
	Qprar->out = databuff;					//���������ַ
	Qprar->end = databuff + Qprar->space; 	//�����������������ַ
	Qprar->ndata = 0;						//����������Ŀ
}
//--------------------------------------------------------------------------
u16 QueueNdata(u16 DmaCurrCnt, QueuePrar *Qprar)
{
	long newdata;

	newdata = Qprar->LastDmaCurrCnt - DmaCurrCnt;	//ʵ�ʴ���������Ŀ
	Qprar->LastDmaCurrCnt = DmaCurrCnt;

	if(newdata<0)
		Qprar->ndata += ((u16)(newdata+Qprar->space));
	else
		Qprar->ndata += (u16)newdata;
	return Qprar->ndata;
}
//--------------------------------------------------------------------------
char QueueGetch(QueuePrar *Qprar)
{
	char ch;

   	ch = Qprar->out[0];				//���ݳ���
	Qprar->out++;					//��������ָ��
	if(Qprar->out >= Qprar->end)	//��������������Ľ���λ��
		Qprar->out = Qprar->sta;	//ָ���������Ŀ�ʼλ��
	return ch;
}
//--------------------------------------------------------------------------
bool QueueRead(u8 *databuff, QueuePrar *Qprar, u16 lenth)
{
	u32 len;

	if(lenth > Qprar->ndata) 			//�ռ䲻��
		return FALSE;
	
	len=lenth;
	while(len)
	{
    	*databuff = Qprar->out[0];		//���ݳ���
		databuff++;
		Qprar->out++;					//��������ָ��
		if(Qprar->out >= Qprar->end)
			Qprar->out = Qprar->sta;
		len--;
	}
	Qprar->ndata -= lenth;
	return TRUE;
}
//----����������ָ��-----------------------------------------------------------------
bool QueueRead2(u8 *databuff, QueuePrar *Qprar, u16 lenth)
{
	u32 len;
	__IO u8 *up;

	if(!databuff)
		return FALSE;
	if(lenth > Qprar->ndata) 			//�ռ䲻��
		return FALSE;
	
	len=lenth;
	up=Qprar->out;
	while(len)
	{
    	*databuff = *up;		//���ݳ���
		databuff++;
		up++;					//��������ָ��
		if(up >= Qprar->end)
			up = Qprar->sta;
		len--;
	}
	return TRUE;
}
//----�������--------------------------------------------------------------
void QueueFlush(volatile u16 ndata, QueuePrar *Qprar)
{
	while(ndata)
	{
		if(Qprar->out >= Qprar->end)
			Qprar->out = Qprar->sta;
		Qprar->out++;
		ndata--;
	}
	Qprar->ndata = 0;
}

