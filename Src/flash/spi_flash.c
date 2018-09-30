#include "stm32f1xx_hal.h"
#include "spi_flash.h"
extern SPI_HandleTypeDef hspi1;

//SPI 数据通讯

uint8_t SPI_TRANSMISSION(uint8_t TxData)
{
    uint8_t rxData = 0;
    
    __HAL_SPI_ENABLE( &hspi1 );

    __disable_irq( );

    while( __HAL_SPI_GET_FLAG( &hspi1, SPI_FLAG_TXE ) == RESET );
    hspi1.Instance->DR = ( uint16_t ) ( TxData & 0xFF );

    while( __HAL_SPI_GET_FLAG( &hspi1, SPI_FLAG_RXNE ) == RESET );
    rxData = ( uint16_t ) hspi1.Instance->DR;

    __enable_irq( );

    return( rxData );
}

/* -------------------------------  FLASH 配置 ------------------------------ */

//FLASH 读状态
uint8_t FLASH_ReadSR(void)
{  
	uint8_t byte=0;

	F_CS_Low;
    SPI_TRANSMISSION(FLASH_ReadStatusReg);  
	byte=SPI_TRANSMISSION(0Xff); 
	F_CS_High;
	     
	return byte;   
} 


//FLASH 写状态
void FLASH_Write_SR(uint8_t sr)   
{   
	F_CS_Low;
	SPI_TRANSMISSION(FLASH_WriteStatusReg);
	SPI_TRANSMISSION(sr);
	F_CS_High;
}   



//FLASH 写使能
void FLASH_Write_Enable(void)   
{
	F_CS_Low;                             
  SPI_TRANSMISSION(FLASH_WriteEnable);
	F_CS_High;
} 


//W25Q16 写禁止 
void FLASH_Write_Disable(void)   
{  
	F_CS_Low;                            
  SPI_TRANSMISSION(FLASH_WriteDisable);        
	F_CS_High;                                	      
}


//等待空闲
void FLASH_Wait_Busy(void)   
{   
	while ((FLASH_ReadSR()&0x01)==0x01);
}



// 读取ID
uint16_t FLASH_ReadDeviceID(void)
{
  uint16_t Temp,temp1,temp2;

  F_CS_Low;

  /* Send "RDID " instruction */
  SPI_TRANSMISSION(0x90);
  SPI_TRANSMISSION(0x00);
  SPI_TRANSMISSION(0x00);
  SPI_TRANSMISSION(0x00);
  
  /* Read a byte from the FLASH */
  temp1 = SPI_TRANSMISSION(0x00);
  temp2 = SPI_TRANSMISSION(0x00);

  /* Deselect the FLASH: Chip Select high */
  F_CS_High;
	
  Temp = (temp1<<8) + temp2;

  return Temp;
}


//读取数据
void FLASH_Read(uint8_t* Buffer,uint32_t Addr,uint16_t ByteNum)   
{ 
 	uint16_t i;
	F_CS_Low;
	SPI_TRANSMISSION(FLASH_ReadData);
	SPI_TRANSMISSION((uint8_t)((Addr)>>16));
	SPI_TRANSMISSION((uint8_t)((Addr)>>8));
	SPI_TRANSMISSION((uint8_t)Addr);
	for(i=0;i<ByteNum;i++)
	{ 
		Buffer[i]=SPI_TRANSMISSION(0XFF);
	}
	F_CS_High;
} 



//FLASH 写一页
void FLASH_Write_Page(uint8_t* Buffer,uint32_t Addr,uint16_t ByteNum)
{
 	uint16_t i;
    FLASH_Write_Enable();   
	F_CS_Low;   
	SPI_TRANSMISSION(FLASH_PageProgram);   
	SPI_TRANSMISSION((uint8_t)((Addr)>>16));    
	SPI_TRANSMISSION((uint8_t)((Addr)>>8));   
	SPI_TRANSMISSION((uint8_t)Addr);
	for(i=0;i<ByteNum;i++)SPI_TRANSMISSION(Buffer[i]);  
	F_CS_High;
	FLASH_Wait_Busy();
} 



//FLASH 写
void FLASH_Write(uint8_t* Buffer,uint32_t Addr,uint16_t ByteNum)   
{
	uint16_t pageremain;
		   
	pageremain=256-Addr%256;
	if(ByteNum<=pageremain)pageremain=ByteNum;
	while(1)
	{	   
		FLASH_Write_Page(Buffer, Addr, pageremain);
		if(ByteNum==pageremain)break;
	 	else 
		{
			Buffer+=pageremain;
			Addr+=pageremain;	

			ByteNum-=pageremain;	 
			if(ByteNum>256)pageremain=256;
			else pageremain=ByteNum;
		}
	}	    
} 
 		


// FLASH 擦除一片
void FLASH_Erase_Chip(void)   
{                                             
	FLASH_Write_Enable();                  
	FLASH_Wait_Busy();
	F_CS_Low;                            
	SPI_TRANSMISSION(FLASH_ChipErase);        
	F_CS_High;                                 	      
	FLASH_Wait_Busy();   				  
} 



//FLASH 擦除一扇区
void FLASH_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr*=4096;
	FLASH_Write_Enable();
	FLASH_Wait_Busy();
	F_CS_Low;
	SPI_TRANSMISSION(FLASH_SectorErase);
	SPI_TRANSMISSION((uint8_t)((Dst_Addr)>>16));
	SPI_TRANSMISSION((uint8_t)((Dst_Addr)>>8));
	SPI_TRANSMISSION((uint8_t)Dst_Addr);
	F_CS_High;
    FLASH_Wait_Busy();
}




//FLASH 擦除一块区
void FLASH_Erase_Block(uint32_t Bst_Addr)   
{   
	Bst_Addr*=65536;
	FLASH_Write_Enable();
	FLASH_Wait_Busy();
	F_CS_Low;
	SPI_TRANSMISSION(FLASH_BlockErase);
	SPI_TRANSMISSION((uint8_t)((Bst_Addr)>>16));
	SPI_TRANSMISSION((uint8_t)((Bst_Addr)>>8));
	SPI_TRANSMISSION((uint8_t)Bst_Addr);
	F_CS_High;
    FLASH_Wait_Busy();
}   





/* -------------------------------  end code  --------------------------------*/
