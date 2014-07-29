/* Includes ------------------------------------------------------------------*/
#include "hal_debug.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "SystemClock.h"
#include "usart_io.h"
#include "compute.h"
#include "bsp.h"
#include "printer.h"

#define PRT_DOTLINE 1
#define PRT_DRYRUN 2

//UART_HandleTypeDef huart6;
USER_UART_HandleTypeDef huart1;
int Data_Gather(unsigned char *buff);
void Data_ACK(short int cmd);
int readflag;




//----初始化--------------------------------------------------------------------
void main_init(void)
{
	HAL_Init();   //
  SystemClock_Config();
	port_init();

	UartInit();

	USART_IO_Config( 	&huart1, 
						USART1, 
						115200, 
						UART_WORDLENGTH_8B, 
						UART_STOPBITS_1, 
						UART_PARITY_NONE, 
						UART_MODE_TX_RX, 
						UART_HWCONTROL_NONE, 
						UART_OVERSAMPLING_16 );

}


/**********************************************************/
int cnt=0;
u8 buffer[100];

int main(void)
{
	int gather_state;
	short int cmd;

	unsigned char buff[48];

	main_init();
	
	USART_IO_Open( &huart1, 0 );

	while(1)
	{			
		gather_state=Data_Gather(buff);	   //????????
		if(gather_state < 0 )
		{
#ifdef TEST_CASE_RX

			switch(gather_state)
			{
				case -1:	printf("error -1\r\n");
							break;
				case -2:	printf("error -2\r\n");
							break;
				case -3:	printf("error -3\r\n");
							break;
			}
#endif
			continue;
		}
		else
			if(gather_state == 0)
				continue;
		//????
		cmd = *((short int*)&buff[2]);
			
		if(cmd == PRT_DOTLINE)
		{
			Data_ACK(1);
			PRT_DotLine(&buff[6],48);			  //????

		}
		
		else 
		if(cmd == PRT_DRYRUN && NO_PAPER == 0)		
		{											
			Data_ACK(2);
			PRT_dry_run(line_num);					//??N?
		}
		else
		{
			Data_ACK(0);	
		}
			
	}
}



//------------------------------------------------------------------------------
int Data_Gather(unsigned char *buff)
{
	static int step_cnt=0;
	unsigned char head;
	static short int length;
	unsigned char c1,c2;
	int rx_count;
	
	
	rx_count=USART_IO_ReadCount(&huart1);
	
	
	switch(step_cnt)
	{
		case 0:	if(rx_count == 0)
							return 0;
						USART_IO_Read(&huart1,&head,1);
						if(head != 0x55)
							return -1;
						step_cnt=1;
						break;
		case 1:	if(rx_count == 0)
							return 0;
						USART_IO_Read(&huart1,&head,1);
						if(head != 0x55)
							return -2;
						step_cnt=2;
						break;
		case 2:	if(rx_count < 2)
							return 0;
						USART_IO_Read(&huart1,buff,2);
						length=*((short int*)&buff[0]);
						step_cnt=3;
						break;
		case 3:	if(length < rx_count-2)
							return 0;
						USART_IO_Read(&huart1,&buff[2],length-2);
						step_cnt =0;
						checkout (buff, length-2, &c1, &c2);
						if(c1 != buff[length-2] || c2 != buff[length-1])
							return -3;		  //校验错误						}
						return 1;
						
	}	
	
	return 0;

}



//********************************************************************//
void Data_ACK(short int cmd)
{
	int i;
	unsigned char buff[8];
	unsigned char c1=buff[2],c2=buff[3];

	buff[0]=buff[1]=0x55;
	buff[2]=6;
	buff[3]=0;
	buff[4]=cmd;
	buff[5]=0;
	for(i=2;i<6;i++)
	{
		c1 ^= buff[i];
		c2 += buff[i];
	}
	buff[6]=c1;
	buff[7]=c2;

	USART_IO_Write(&huart1, buff, 8);
		
}







/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

