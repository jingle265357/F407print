#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "HalSysTime.h"

#include "HalUart.h"
#include "HalDmaQueue.h"
/////////////////////////////////////////////////////////////////////////////////////////////////
/*
//#define USART1                   USART1
#define USART1_GPIO              GPIOA
#define USART1_RxPin             GPIO_Pin_10
#define USART1_TxPin             GPIO_Pin_9
//#define USART1_Tx_DMA_Channel    DMA1_Channel4
//#define USART1_Tx_DMA_FLAG       DMA1_FLAG_TC4
#define USART1_Rx_DMA_Channel    DMA1_Channel5
#define USART1_Rx_DMA_FLAG       DMA1_FLAG_TC5 
#define USART1_DR_Base           0x40013804

#define UART1_RX_MAX 272				//���ջ������ֽڿռ�
static u8 UART1_RxBuff[UART1_RX_MAX];	//�������ݻ�����
  */

const USART_TypeDef* UART_TAB[]={USART1, USART2, USART3};
const GPIO_TypeDef *UART_GPIO_TAB[]={GPIOA, GPIOA, GPIOB};
const u16 UART_TxPin_TAB[]={GPIO_Pin_9, GPIO_Pin_2, GPIO_Pin_10};
const u16 UART_RxPin_TAB[]={GPIO_Pin_10, GPIO_Pin_3, GPIO_Pin_11};
const u32 UART_ReMap_TAB[]={0, 0, GPIO_PartialRemap_USART3};

const DMA_Channel_TypeDef* UART_DMA_CHA_TAB[]={	DMA1_Channel5, DMA1_Channel6, DMA1_Channel3};
const u32 USART_DR_Base_TAB[]={0x40013804, 0x40004404, 0x40004804};
const u16 UART_RX_MAX_TAB[]={1040, 1040, 1040};				//���ջ������ֽڿռ�
static u8 UART1_RxBuff[1040];	//�������ݻ�����
static u8 UART2_RxBuff[1040];	//�������ݻ�����
static u8 UART3_RxBuff[1040];	//�������ݻ�����

static u8* UART_BUFF_TAB[]={UART1_RxBuff, UART2_RxBuff, UART3_RxBuff};
static bool	UART_AUTO_CLR_TAB[3];			//��������Զ��������˫��485��ʽ����־	
static QueuePrar UART_RxPrar_TAB[3];		//���ն��в���
u32 UART_Baud_TAB[3]={0,0,0};			//����

//static u8 UARTx_FLAG;	//ָ��ʹ�õ�UART��־��

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
u16 com_ndata(u8 com)
{
	u16 DmaCurrCnt;
	u16 ndata;

	DmaCurrCnt=DMA_GetCurrDataCounter((DMA_Channel_TypeDef *)UART_DMA_CHA_TAB[com]);	//DMA��ʣ��Ĵ�����������Ŀ
	ndata=QueueNdata(DmaCurrCnt, &UART_RxPrar_TAB[com]);
	return ndata;
}
//----��ȡ��������---------------------------------------------------------------------
u8 com_getc(u8 com)
{
	u16 ndata;

	ndata=com_ndata(com);
	if(ndata==0) 				//û�н��յ�����
		return 0;
	return QueueGetch(&UART_RxPrar_TAB[com]);
}
//----��ȡ��������--------------------------------------------------------
bool com_read(u8 com, u8 *buff, vu16 lenth)
{
	if(com_ndata(com)==0) 				//û�н��յ�����
		return FALSE;
	return QueueRead(buff, &UART_RxPrar_TAB[com], lenth);
}
//----��ȡ��������--------------------------------------------------------
bool com_read2(u8 com, u8 *buff, vu16 lenth)
{
	if(com_ndata(com)==0) 				//û�н��յ�����
		return FALSE;
	return QueueRead2(buff, &UART_RxPrar_TAB[com], lenth);
}
//----������ջ�����-------------------------------------------------------
void com_clear(u8 com)
{
	QueueFlush(com_ndata(com), &UART_RxPrar_TAB[com]);
}
//----����һ���ֽ�---------------------------------------------------------
void com_putc(u8 com, u8 ch)
{
	u32 t;

	USART_SendData((USART_TypeDef*)UART_TAB[com], (u8)ch);
	while (USART_GetFlagStatus((USART_TypeDef*)UART_TAB[com], USART_FLAG_TC)==RESET);	//Loop until the end of transmission
	if(UART_AUTO_CLR_TAB[com]==TRUE)		//��������Զ��������˫��485��ʽ����־	
	{
		t=300*38400/UART_Baud_TAB[com];		//38400������ʱΪ0.3mS
		delay_us(t);
		QueueFlush(com_ndata(com), &UART_RxPrar_TAB[com]);
	}
}
//----�����ַ���-----------------------------------------------------------
void com_write(u8 com, u8* buff, vu16 lenth)
{
	u32 t;

	while(lenth)
	{
		lenth--;
		USART_SendData((USART_TypeDef*)UART_TAB[com], *buff++);
		while(USART_GetFlagStatus((USART_TypeDef*)UART_TAB[com], USART_FLAG_TC)==RESET);	//Loop until the end of transmission
	}
	if(UART_AUTO_CLR_TAB[com]==TRUE)		//��������Զ��������˫��485��ʽ����־	
	{
		t=300*38400/UART_Baud_TAB[com];		//38400������ʱΪ0.3mS
		delay_us(t);
		QueueFlush(com_ndata(com), &UART_RxPrar_TAB[com]);
	}
}
//----��������Զ��������˫��485��ʽ��-----------------------------------
void com_autoclear(u8 com, bool f)
{
	UART_AUTO_CLR_TAB[com]=f;
}					

//----�������ݰ�----------------------------------------------------
u16 com_package(u8 com)
{
	static u8 t=0;
	u8 t_bound;				//����bound�������ʱ
	static u16 cnt=0;
	u16 ndata;

	ndata=com_ndata(com);
	//δ���յ�����
	if(ndata==0)
		return 0;

	//δ�����ʱ��
	t++;
	t_bound = 11000/UART_Baud_TAB[com] + 2;		//11000pbs�����ʣ�����Ҫ��1mS 	?????
	if(t<=t_bound)
		return 0;
	t=0;

	//���ڽ�����
	if(cnt != ndata)
	{
		cnt=ndata;
		return 0;
	}

	cnt=0;
	return ndata;
}

////////////////////////////////////////////////////////////////////////////////
void com_init(u8 com, u32 baud, u8 width, u8 stop, u8 par)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	u16 WordLenth=USART_WordLength_8b;
	u16 StopBit=USART_StopBits_1;
	u16 Parity=USART_Parity_No;

	QueueCreate(UART_BUFF_TAB[com], UART_RX_MAX_TAB[com], &UART_RxPrar_TAB[com]);		//��������

	if(com==COM1)
		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_USART1, ENABLE);
	else
	if(com==COM2)
  		RCC_APB1PeriphClockCmd(	RCC_APB1Periph_USART2, ENABLE);
	else
	if(com==COM3)
  		RCC_APB1PeriphClockCmd(	RCC_APB1Periph_USART3, ENABLE);

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE);

	//��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = UART_TxPin_TAB[com];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)UART_GPIO_TAB[com], &GPIO_InitStructure);

	//��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = UART_RxPin_TAB[com];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	
	GPIO_Init((GPIO_TypeDef *)UART_GPIO_TAB[com], &GPIO_InitStructure);

	//��Ӧ�ض����
//	if(UART_ReMap_TAB[com]!=0)
//		GPIO_PinRemapConfig(UART_ReMap_TAB[com], ENABLE);

	//����USART����
	if(width==9)
		WordLenth=USART_WordLength_9b;
	if(stop==2)
		StopBit=USART_StopBits_2;
	if(par==1)
		Parity=USART_Parity_Even;
	else
	if(par==2)
		Parity=USART_Parity_Odd;
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = WordLenth;
	USART_InitStructure.USART_StopBits = StopBit;
	USART_InitStructure.USART_Parity = Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init((USART_TypeDef *)UART_TAB[com], &USART_InitStructure);

	RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_DMA1, ENABLE);
	//USART1 RX DMA1 Channel (triggered by USART1 Rx event) Config
	DMA_DeInit((DMA_Channel_TypeDef *)UART_DMA_CHA_TAB[com]);					//DMAͨ��  
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_Base_TAB[com];			//USART���ݵ�ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)UART_BUFF_TAB[com];			    //���ջ�����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//������Ϊ������Դ
	DMA_InitStructure.DMA_BufferSize = UART_RX_MAX_TAB[com];					//DMA�������ֽ�
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//�����ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//�ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//����8λ����
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//�ڴ�8λ����
    //ѭ��ģʽ������Bufferд�����Զ��ص���ʼ��ַ��ʼ����
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//ѭ��ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;							//ӵ�и����ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//�������ڴ浽�ڴ洫
	DMA_Init((DMA_Channel_TypeDef *)UART_DMA_CHA_TAB[com], &DMA_InitStructure);  
	//Enable USARTy RX DMA1 Channel

	DMA_Cmd((DMA_Channel_TypeDef *)UART_DMA_CHA_TAB[com], ENABLE);
	
	//Enable USARTy DMA Rx and TX request 
	USART_DMACmd((USART_TypeDef *)UART_TAB[com], USART_DMAReq_Rx, ENABLE);
	//ʹ�� USART�� �������
	USART_Cmd((USART_TypeDef *)UART_TAB[com], ENABLE);
	delay_ms(1);

	//CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ, �����������1���ֽ��޷���ȷ���ͳ�ȥ������
//	USART_ClearFlag((USART_TypeDef *)UART_TAB[com], USART_FLAG_TC);     //�巢����Ǳ�־��Transmission Complete flag
	while(USART_GetFlagStatus((USART_TypeDef*)UART_TAB[com], USART_FLAG_TC)==RESET);	//Loop until the end of transmission

	UART_AUTO_CLR_TAB[com]=FALSE;
	UART_Baud_TAB[com]=baud;
}

