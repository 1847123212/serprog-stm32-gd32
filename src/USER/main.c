#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "spi.h"
#include "flash.h"


#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"

   	
//Ҫд�뵽W25Q64���ַ�������
const u8 TEXT_Buffer[]={"MiniSTM32 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
 int main(void)
 { 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE;
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	 
	delay_ms(1800);
	USB_Port_Set(0); 	
	delay_ms(700);
	USB_Port_Set(1);	
 	Set_USBClock();   
 	USB_Interrupts_Config();    
 	USB_Init();	 	 
	 
	KEY_Init();				//������ʼ��		 	
	SPI_Flash_Init();  		//SPI FLASH ��ʼ�� 	 
 	
	while(SPI_Flash_ReadID()!=W25Q64)							//��ⲻ��W25Q64
	{
		delay_ms(500);
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}

	FLASH_SIZE=4*1024*1024;	//FLASH ��СΪ4M�ֽ�
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)	//WK_UP ����,д��W25Q64
		{
			
			SPI_Flash_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
//��ʾ�������
		}
		if(key==KEY0_PRES)	//KEY0 ����,��ȡ�ַ�������ʾ
		{

			SPI_Flash_Read(datatemp,FLASH_SIZE-100,SIZE);				//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
//��ʾ�������
				//��ʾ�������ַ���
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}		   
	}
}

