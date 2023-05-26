#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

#define I2C_WR	0		/* Ð´¿ØÖÆbit */
#define I2C_RD	1		/* ¶Á¿ØÖÆbit */

typedef struct _i2c {
        void     (*IIC_InitPort)( void );
        void     (*IIC_SDA_H)();
        void     (*IIC_SDA_L)();
        void     (*IIC_SCL_H)();
        void     (*IIC_SCL_L)();
        uint8_t  (*IIC_READ)();
        uint8_t  (*IIC_READ_SCL)();
} I2C;

void i2c_Start( I2C *i2c );
void i2c_Stop( I2C *i2c );
void i2c_SendByte( I2C *i2c, uint8_t _ucByte);
uint8_t i2c_ReadByte( I2C *i2c );
uint8_t i2c_WaitAck( I2C *i2c );
void i2c_Ack( I2C *i2c );
void i2c_NAck( I2C *i2c );
uint8_t i2c_CheckDevice(I2C *i2c, uint8_t _Address);

#endif /*__I2C_H  */
