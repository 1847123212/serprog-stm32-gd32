#include "flash.h" 
#include "spi.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_spi.h"
#include "delay.h" 
#include "hw_config.h"

//Mini STM32������
//W25X16 �������� 
//����ԭ��@ALIENTEK
//2010/6/13
//V1.0
//u16 SPI_FLASH_TYPE=W25Q64;//Ĭ�Ͼ���25Q64
//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25X16
//����Ϊ2M�ֽ�,����32��Block,512��Sector 



/* Do not place const in front of declarations.                  *
 * const variables are stored in flash that needs a 2-cycle wait */
uint8_t  DMA_Clk_Buf = 0;

/* Refer to USB IO for bulk transfer */
extern uint8_t  USB_Tx_Buf[];
extern uint16_t USB_Tx_ptr_in;
extern uint8_t  USB_Rx_Buf[];
extern uint16_t USB_Rx_ptr_out;
extern uint8_t  USB_Rx_len;

extern void usb_putp(void);
extern void usb_getp(void);
extern char usb_getc(void);

/*
//��ʼ��SPI FLASH��IO��
void SPI_Flash_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  //SPI CS
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
	SPI1_Init();		   //��ʼ��SPI
	SPI1_SetSpeed(SPI_BaudRatePrescaler_4);	//����Ϊ18Mʱ��,����ģʽ
	SPI_FLASH_TYPE=SPI_Flash_ReadID();//��ȡFLASH ID.
}  

//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI1_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI1_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS=0;                            //ʹ������   
	SPI1_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPI1_ReadWriteByte(sr);               //д��һ���ֽ�  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
}   
//SPI_FLASHдʹ��	
//��WEL��λ   
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
} 
//SPI_FLASHд��ֹ	
//��WEL����  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
} 			    
//��ȡоƬID W25X16��ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS=0;				    
	SPI1_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS=1;				    
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
    SPI1_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
    SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI1_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI1_ReadWriteByte(0XFF);   //ѭ������  
    }
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    SPI1_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
    SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI1_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 	 
}
//��������оƬ
//��Ƭ����ʱ��:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//�ȴ�ʱ�䳬��...
void SPI_Flash_Erase_Chip(void)   
{                                             
    SPI_FLASH_Write_Enable();                  //SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
	SPI_Flash_Wait_Busy();   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ 0~511 for w25x16
//����һ��ɽ��������ʱ��:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI1_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    SPI_Flash_Wait_Busy();   				   //�ȴ��������
}  
//�ȴ�����
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  
//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    delay_us(3);                               //�ȴ�TPD  
}   
//����
void SPI_Flash_WAKEUP(void)   
{  
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	  SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    delay_us(3);                               //�ȴ�TRES1
}   
*/

/* Quick init definations */
/*
DMA_InitTypeDef DMA_InitStructure_RX  = {
  .DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Base,
  .DMA_MemoryBaseAddr     = (uint32_t)USB_Tx_Buf,
  .DMA_DIR                = DMA_DIR_PeripheralSRC,
  .DMA_BufferSize         = VCP_DATA_SIZE,
  .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
  .DMA_MemoryInc          = DMA_MemoryInc_Enable,
  .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
  .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
  .DMA_Mode               = DMA_Mode_Normal,
  .DMA_Priority           = DMA_Priority_High,
  .DMA_M2M                = DMA_M2M_Disable,
};

DMA_InitTypeDef DMA_InitStructure_TX  = {
  .DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Base,
  .DMA_MemoryBaseAddr     = (uint32_t)USB_Rx_Buf,
  .DMA_DIR                = DMA_DIR_PeripheralDST,
  .DMA_BufferSize         = VCP_DATA_SIZE,
  .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
  .DMA_MemoryInc          = DMA_MemoryInc_Enable,
  .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
  .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
  .DMA_Mode               = DMA_Mode_Normal,
  .DMA_Priority           = DMA_Priority_Low,
  .DMA_M2M                = DMA_M2M_Disable,
};

DMA_InitTypeDef DMA_InitStructure_CLK = {
  .DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Base,
  .DMA_MemoryBaseAddr     = (uint32_t)&DMA_Clk_Buf,
  .DMA_DIR                = DMA_DIR_PeripheralDST,
  .DMA_BufferSize         = VCP_DATA_SIZE,
  .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
  .DMA_MemoryInc          = DMA_MemoryInc_Disable,
  .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
  .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
  .DMA_Mode               = DMA_Mode_Normal,
  .DMA_Priority           = DMA_Priority_Low,
  .DMA_M2M                = DMA_M2M_Disable,
};

SPI_InitTypeDef SPI_InitStructure     = {
  .SPI_Direction          = SPI_Direction_2Lines_FullDuplex,
  .SPI_Mode               = SPI_Mode_Master,
  .SPI_DataSize           = SPI_DataSize_8b,
  .SPI_CPOL               = SPI_CPOL_Low,
  .SPI_CPHA               = SPI_CPHA_1Edge,
  .SPI_NSS                = SPI_NSS_Soft,
  .SPI_FirstBit           = SPI_FirstBit_MSB,
  .SPI_CRCPolynomial      = 7,
};*/
DMA_InitTypeDef DMA_InitStructure_RX;
DMA_InitTypeDef DMA_InitStructure_TX;
DMA_InitTypeDef DMA_InitStructure_CLK;
SPI_InitTypeDef SPI_InitStructure;

void DMA_configuration(void)
{
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1|RCC_AHBPeriph_DMA2, ENABLE);	
	  DMA_DeInit(SPI_RX_DMA_CH);
  DMA_DeInit(SPI_TX_DMA_CH);
DMA_InitStructure_RX.DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Base;
DMA_InitStructure_RX.DMA_MemoryBaseAddr     = (uint32_t)USB_Tx_Buf;
DMA_InitStructure_RX.DMA_DIR                = DMA_DIR_PeripheralSRC;
DMA_InitStructure_RX.DMA_BufferSize         = VCP_DATA_SIZE;
DMA_InitStructure_RX.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
DMA_InitStructure_RX.DMA_MemoryInc          = DMA_MemoryInc_Enable;
DMA_InitStructure_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
DMA_InitStructure_RX.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
DMA_InitStructure_RX.DMA_Mode               = DMA_Mode_Normal;
DMA_InitStructure_RX.DMA_Priority           = DMA_Priority_High;
DMA_InitStructure_RX.DMA_M2M                = DMA_M2M_Disable;
	
  DMA_InitStructure_TX.DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Base;
  DMA_InitStructure_TX.DMA_MemoryBaseAddr     = (uint32_t)USB_Rx_Buf;
  DMA_InitStructure_TX.DMA_DIR                = DMA_DIR_PeripheralDST;
  DMA_InitStructure_TX.DMA_BufferSize         = VCP_DATA_SIZE;
  DMA_InitStructure_TX.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure_TX.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure_TX.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure_TX.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure_TX.DMA_Priority           = DMA_Priority_Low;
  DMA_InitStructure_TX.DMA_M2M                = DMA_M2M_Disable;;	
	
  DMA_InitStructure_CLK.DMA_PeripheralBaseAddr = (uint32_t)SPI_DR_Base;
  DMA_InitStructure_CLK.DMA_MemoryBaseAddr     = (uint32_t)&DMA_Clk_Buf;
  DMA_InitStructure_CLK.DMA_DIR                = DMA_DIR_PeripheralDST;
  DMA_InitStructure_CLK.DMA_BufferSize         = VCP_DATA_SIZE;
  DMA_InitStructure_CLK.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure_CLK.DMA_MemoryInc          = DMA_MemoryInc_Disable;
  DMA_InitStructure_CLK.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure_CLK.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure_CLK.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure_CLK.DMA_Priority           = DMA_Priority_Low;
  DMA_InitStructure_CLK.DMA_M2M                = DMA_M2M_Disable;
	
	
  SPI_InitStructure.SPI_Direction          = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode               = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize           = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL               = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA               = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS                = SPI_NSS_Soft;
  SPI_InitStructure.SPI_FirstBit           = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial      = 7;	
	
}





void dma_conf_spiwrite(void) {
	
  DMA_Init(SPI_RX_DMA_CH, &DMA_InitStructure_RX);
  DMA_Init(SPI_TX_DMA_CH, &DMA_InitStructure_TX);
}

void dma_conf_spiread(void) {
  DMA_Init(SPI_RX_DMA_CH, &DMA_InitStructure_RX);
  DMA_Init(SPI_TX_DMA_CH, &DMA_InitStructure_CLK);
}

void dma_commit(void) {
  /* Always enable TX channel first, especially under 36MHz clock. */
  SPI_I2S_DMACmd(SPI_BUS_USED, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
  DMA_Cmd(SPI_RX_DMA_CH, ENABLE);
  DMA_Cmd(SPI_TX_DMA_CH, ENABLE);

  /* Wait for transfer to complete */
  while(!DMA_GetFlagStatus(SPI_RX_DMA_FLAG));

  /* Clear up */
  SPI_I2S_DMACmd(SPI_BUS_USED, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, DISABLE);
  DMA_DeInit(SPI_RX_DMA_CH);
  DMA_DeInit(SPI_TX_DMA_CH);
}

uint32_t spi_conf(uint32_t speed_hz) {
  static uint16_t clkdiv;
  static uint32_t relspd;

  /* SPI_BUS_USED is on APB2 which runs @ 72MHz. */
  /* Lowest available */
  clkdiv = SPI_BaudRatePrescaler_256;
  relspd = 281250;
  if(speed_hz >= 562500) {
    clkdiv = SPI_BaudRatePrescaler_128;
    relspd = 562500;
  }
  if(speed_hz >= 1125000) {
    clkdiv = SPI_BaudRatePrescaler_64;
    relspd = 1125000;
  }
  if(speed_hz >= 2250000) {
    clkdiv = SPI_BaudRatePrescaler_32;
    relspd = 2250000;
  }
  if(speed_hz >= 4500000) {
    clkdiv = SPI_BaudRatePrescaler_16;
    relspd = 4500000;
  }
  if(speed_hz >= 9000000) {
    clkdiv = SPI_BaudRatePrescaler_8;
    relspd = 9000000;
  }
  if(speed_hz >= 18000000) {
    clkdiv = SPI_BaudRatePrescaler_4;
    relspd = 18000000;
  }
  if(speed_hz >= 36000000) {
    clkdiv = SPI_BaudRatePrescaler_2;
    relspd = 36000000;
  }

  SPI_I2S_DeInit(SPI_BUS_USED);

  SPI_InitStructure.SPI_BaudRatePrescaler = clkdiv;

  SPI_Init(SPI_BUS_USED, &SPI_InitStructure);
  SPI_CalculateCRC(SPI_BUS_USED, DISABLE);
  SPI_Cmd(SPI_BUS_USED, ENABLE);

  return relspd;
}

void spi_putc(uint8_t c) {
  /* transmit c on the SPI bus */
  SPI_I2S_SendData(SPI_BUS_USED, c);

  /* Those useless data just needs to be collected, or SPI engine will go crazy. */
  while(SPI_I2S_GetFlagStatus(SPI_BUS_USED, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI_BUS_USED);
}

void spi_bulk_write(uint32_t size) {
  /* Prepare alignment */
  if(size >= (USB_Rx_len - USB_Rx_ptr_out)) {
    size -= (USB_Rx_len - USB_Rx_ptr_out);
    while(USB_Rx_ptr_out != USB_Rx_len) spi_putc(usb_getc());
  }
  /* else: size << VCP_DATA_SIZE, no bulk transfer */

  /* Do bulk transfer */
  while(size != 0) {
    usb_getp();

    if(USB_Rx_len < VCP_DATA_SIZE) {
      /* Host is not feeding fast enough / finish the left-over bytes */
      size -= USB_Rx_len;
      while(USB_Rx_ptr_out != USB_Rx_len) spi_putc(usb_getc());
    }
    else {
      size -= VCP_DATA_SIZE;
      /* DMA Engine must be configured for EVERY transfer */
      dma_conf_spiwrite();
      dma_commit();
    }
  }
}

void spi_bulk_read(uint32_t size) {
  /* Flush buffer and make room for DMA */
	 static int i;
	
  if(USB_Tx_ptr_in != 0) usb_putp();

 

  /* Do bulk transfer */
  while(size >= VCP_DATA_SIZE) {
    /* DMA Engine must be configured for EVERY transfer */
    dma_conf_spiread();
    dma_commit();

    USB_Tx_ptr_in = VCP_DATA_SIZE;
    usb_putp();
    size -= VCP_DATA_SIZE;
  }

  /* Finish the left-over bytes */
  if(size != 0) {
    for(i = 0; i < size; i ++) {
      SPI_I2S_SendData(SPI_BUS_USED, 0);
      while(SPI_I2S_GetFlagStatus(SPI_BUS_USED, SPI_I2S_FLAG_RXNE) == RESET);
      USB_Tx_Buf[i] = SPI_I2S_ReceiveData(SPI_BUS_USED);
    }
    USB_Tx_ptr_in = size;
  }
}























