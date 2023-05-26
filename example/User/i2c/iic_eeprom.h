#ifndef __EEPROM_H
#define	__EEPROM_H


#include "stm32f10x.h"

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* AT24C01/02ÿҳ��8���ֽ� 
 * AT24C04/08A/16Aÿҳ��16���ֽ� 
 */
	
#define EEPROM_I2C_WR	0		/* д����bit */
#define EEPROM_I2C_RD	1		/* ������bit */
#define EEPROM_DEV_ADDR			0xA0		/* 24xx02���豸��ַ */
#define EEPROM_PAGE_SIZE		  8			  /* 24xx02��ҳ���С */
#define EEPROM_SIZE				  256			  /* 24xx02������ */

#define GPIO_PORT_I2C	        GPIOB			        			/* GPIO�˿� */
#define RCC_I2C_PORT 	        RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define I2C_SCL_PIN						GPIO_Pin_6							/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN						GPIO_Pin_7							/* ���ӵ�SDA�����ߵ�GPIO */

/* �����дSCL��SDA�ĺ� */
#define I2C_SCL_1()             GPIO_SetBits(GPIO_PORT_I2C,I2C_SCL_PIN)/* SCL = 1 */
#define I2C_SCL_0()             GPIO_ResetBits(GPIO_PORT_I2C,I2C_SCL_PIN)	/* SCL = 0 */

#define I2C_SDA_1()             GPIO_SetBits(GPIO_PORT_I2C,I2C_SDA_PIN)	/* SDA = 1 */
#define I2C_SDA_0()             GPIO_ResetBits(GPIO_PORT_I2C,I2C_SDA_PIN)	/* SDA = 0 */

#define I2C_SDA_READ()         GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN)	/* ��SDA����״̬ */
#define I2C_SCL_READ()          GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SCL_PIN)	/* ��SCL����״̬ */

void EEPROM_Init( void );
uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_Erase(void);


#endif /* EEPROM_H */
