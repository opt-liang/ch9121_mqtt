/**
  ******************************************************************************
  * @file
  * @author  lisck
  * @version V1.0.0
  * @date    2016-01-01
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

#include <stdint.h>
#include "gpio.h"

#define	 F_CS_High  HAL_GPIO_WritePin(GPIOA, FLASH_CS_Pin|FLASH_WP_Pin, GPIO_PIN_SET)
#define	 F_CS_Low   HAL_GPIO_WritePin(GPIOA, FLASH_CS_Pin|FLASH_WP_Pin, GPIO_PIN_RESET)

#define FLASH_WriteEnable		    0x06
#define FLASH_WriteDisable	  	    0x04
#define FLASH_ReadStatusReg		    0x05
#define FLASH_WriteStatusReg		0x01
#define FLASH_ReadData		    	0x03
#define FLASH_FastReadData	  	    0x0B
#define FLASH_FastReadDual	  	    0x3B
#define FLASH_PageProgram	  	    0x02
#define FLASH_BlockErase		  	0xD8
#define FLASH_SectorErase		    0x20
#define FLASH_ChipErase		  	    0xC7
#define FLASH_PowerDown			    0xB9
#define FLASH_ReleasePowerDown	    0xAB
#define FLASH_ManufactDeviceID	    0x90

uint16_t  FLASH_ReadDeviceID(void);
uint8_t   FLASH_ReadSR(void);     
void FLASH_Write_SR(uint8_t sr); 
void FLASH_Write_Enable(void);  
void FLASH_Write_Disable(void);	
void FLASH_Read(uint8_t* Buffer,uint32_t Addr,uint16_t ByteNum);  
void FLASH_Write_Page(uint8_t* Buffer,uint32_t Addr,uint16_t ByteNum); 
void FLASH_Write(uint8_t* Buffer,uint32_t Addr,uint16_t NumByte);
void FLASH_Erase_Chip(void);   
void FLASH_Erase_Sector(uint32_t Dst_Addr);
void FLASH_Erase_Block(uint32_t Bst_Addr); 
void FLASH_Wait_Busy(void);




/* ------------------------------------------------------------------------- */

















/* -------------------------------  end code  --------------------------------*/
