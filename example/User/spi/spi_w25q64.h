#ifndef __SPI_W25Q64_H
#define __SPI_W25Q64_H

#include "stm32f10x.h"

#define SPI_NSS_1()              	GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define SPI_NSS_0()             	GPIO_ResetBits(GPIOC, GPIO_Pin_0)

#define SPI_MOSI_1()              GPIO_SetBits(GPIOA, GPIO_Pin_7) 
#define SPI_MOSI_0()             	GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define SPI_SCK_1()              	GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define SPI_SCK_0()              	GPIO_ResetBits(GPIOA, GPIO_Pin_5)	

#define SPI_MISO_READ()          	GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)



 
uint32_t SPI_FLASH_ReadID(void);
void W25Q64_Init( void );
//void bsp_InitSPI(void);


#endif /*__SPI_W25Q64_H  */
