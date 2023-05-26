#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"
#define Dummy_Byte    0xFF
typedef struct _spi {
        void     (*SPI_InitPort)( void );
        void     (*SPI_NSS_L)();
        void     (*SPI_NSS_H)();
        void     (*SPI_MOSI_H)();
        void     (*SPI_MOSI_L)();
				void     (*SPI_SCK_H)();
        void     (*SPI_SCK_L)();	
        uint8_t  (*SPI_MISO_READ)();
} SPI;

void spi_Start( SPI *spi );
void spi_Stop( SPI *spi );
uint8_t SPI_ReadByte(SPI *spi,uint8_t ucMode);
uint8_t SPI_ReadWriteByte( SPI *spi ,uint8_t txData ,uint8_t ucMode );

#endif /*__SPI_H  */
