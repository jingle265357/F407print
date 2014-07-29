#include"bsp.h"
#include"delay.h"
#include"printer.h"
#include"test_case.h"

#define STEP_MAX 4
PRT_STM prt_stm;
char code step[8]={0,0,0,1,1,1,1,0};	//�������״̬ѡ������//								 


/***************************��ʼ��������״̬�����������****************************/


void PRT_STM_init (void)
{
	prt_stm.no_paper=1;		  //��ֽ//
	prt_stm.over_hot=0;		  //����//
	prt_stm.step_count=0;
	prt_stm.thermal_head_count=0;
	prt_stm.data_ready=0;
	prt_stm.time0_count=0;
}



/*******************************************************************************************/
/******************************�йط���*****************************************************/
void PRT_DotLine(char *str, int length)
{
		int i,j;
		char temp;
	
	
		for(i=0;i<length;i++)
		{
			temp = *str;
			for(j=0;j<8;j++)
			{
				if(temp & 0x80)
					DI=1;
				else
					DI=0;
				temp <<= 1;
	
				CLK=1;
				delay(1);
				CLK=0;
				delay(1);				
			}
			str++;
		}
	
	

}

void PRT_dry_run(int line_num)
{
	int i;
	for(i=0;i<line_num*2;i++)
	{
		STEP_A=step[prt_stm.step_count*2+1];	 //�����������//
		STEP_B=step[prt_stm.step_count*2];
		prt_stm.step_count++;
		if(prt_stm.step_count >= STEP_MAX)
			prt_stm.step_count=0;
		delay(1);
	}
}




/*******************************************************************************************************/


/**********************�жϺ����������������ر�*************************************************/



void timer0_start(void)		//�жϿ�������//
{
	TMOD=0x01;   //����Ϊ��ʱ��0,������ʽ1
	TH0=(65536-1200)/256;   //װ�����ֵ(������8λ��)
	TL0=(65536-1200)%256;   //װ�����ֵ(������8λ�ģ�

	EA=1;                   //�����ж�
	ET0=1;                  //��ʱ��0�ж�
	TR0=1;                  //������ʱ��


	STEP_A=step[prt_stm.step_count*2+1];   //�������A��//
	STEP_B=step[prt_stm.step_count*2];	  //�������B��//
	STEP_E=1;							  //�������ʹ��//

	prt_stm.step_count++;

}


void time0() interrupt 1
{


#ifdef TEST_CASE_TIME0
	TH0=(65536-1500)/256;  //װ�����ֵ(������8λ��)
	TL0=(65536-1500)%256;  //װ�����ֵ(������8λ�ģ�
	DI=~DI;
	ET0=1;
#else


	ET0=0;	//���ж�//
	TH0=(65536-1500)/256;  //װ���ʼֵ(������8λ��)
	TL0=(65536-1500)%256;  //װ���ʼֵ(������8λ�ģ�

	prt_stm.time0_count++;
	if(prt_stm.time0_count == 1)
	{
		//�ж��Ƿ���dotline����
		if(prt_stm.data_ready == 0 || NO_PAPER == 1)
		{
			prt_stm.time0_count=0;
			ET0=1;
			STB2=0;	   
	   		STB1=0;
			return;	
		}
		prt_stm.data_ready = 0;
		//����
		STB2=0;	   
	   	STB1=1;
		//����
		STEP_A=step[prt_stm.step_count*2+1];	 //�����������//
		STEP_B=step[prt_stm.step_count*2];
		prt_stm.step_count++;
	}
	else
	if(prt_stm.time0_count == 2) 
	{
		STB1=0;
		STB2=1;
	}
	else
	if(prt_stm.time0_count == 3) 
	{
		STB2=0;
		STB1=1;
		STEP_A=step[prt_stm.step_count*2+1];	 //�����������//
		STEP_B=step[prt_stm.step_count*2];
		prt_stm.step_count++;

		if(prt_stm.step_count >= STEP_MAX)
			prt_stm.step_count=0;			
	}
	else
	if(prt_stm.time0_count == 4) 
	{
		STB1=0;
		STB2=1;
		prt_stm.time0_count=0;
	}
	ET0=1;
#endif

}


