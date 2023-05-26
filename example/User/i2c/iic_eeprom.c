#include "I2C.h"
#include "iic_eeprom.h"


I2C EEPROM;

static uint8_t bsp_i2cRead( void )
{
    return I2C_SDA_READ();
}

static uint8_t bsp_i2cReadSCL( void )
{
    return I2C_SCL_READ();
}

static void bsp_i2c_SclHigh( void )
{
    I2C_SCL_1();
}

static void bsp_i2c_SclLow( void )
{
    I2C_SCL_0();
}

static void bsp_i2c_SdaHigh( void )
{
    I2C_SDA_1();
}

static void bsp_i2c_SdaLow( void )
{
    I2C_SDA_0();
}


/**********************************************************************************************************
*	函 数 名: bsp_InitI2C
*	功能说明: 初始化GPIO
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
static void bsp_InitI2C(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);	/* 打开GPIO时钟 */

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* 设为输出口 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);
}


/**********************************************************************************************************
*	函 数 名: EEPROM_Init
*	功能说明: 初始化接口
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void EEPROM_Init( void )
{
    I2C *pI2C;
    
    pI2C = &EEPROM;
    
    pI2C->IIC_InitPort = bsp_InitI2C;
    pI2C->IIC_READ = bsp_i2cRead;
    pI2C->IIC_READ_SCL = bsp_i2cReadSCL;
    
    pI2C->IIC_SCL_H = bsp_i2c_SclHigh;
    pI2C->IIC_SCL_L = bsp_i2c_SclLow;
    
    pI2C->IIC_SDA_H =  bsp_i2c_SdaHigh;
    pI2C->IIC_SDA_L =  bsp_i2c_SdaLow;
   
    i2c_Stop( pI2C );   /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    
    bsp_InitI2C();    //初始化IO
}


/**********************************************************************************************************
*	函 数 名: ee_CheckOk
*	功能说明: 判断串行EERPOM是否正常
*	形    参：无
*	返 回 值: 成功返回0 失败返回1
**********************************************************************************************************/
uint8_t ee_CheckOk(void)
{
  I2C *pI2C;
  pI2C = &EEPROM;
	if (i2c_CheckDevice(pI2C,EEPROM_DEV_ADDR) == 0)
	{
		return 0;
	}
	else
	{
		/* 失败后，切记发送I2C总线停止信号 */
		i2c_Stop(pI2C);		
		return 1;
	}
}


/**********************************************************************************************************
*	函 数 名: ee_ReadBytes
*	功能说明: 从串行EEPROM指定地址处开始读取若干数据
*	形    参：_usAddress : 起始地址
*			 			_usSize 	 : 数据长度，单位为字节
*			 			_pReadBuf  : 存放读到的数据的缓冲区指针
*	返 回 值: 成功返回0 失败返回1
**********************************************************************************************************/
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i;
	I2C *pI2C; 
  pI2C = &EEPROM;
	i2c_Start(pI2C);
	i2c_SendByte(pI2C,EEPROM_DEV_ADDR | EEPROM_I2C_WR);
	if (i2c_WaitAck(pI2C) != 0)
	{
		goto cmd_fail;
	}
	//发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	i2c_SendByte(pI2C,(uint8_t)_usAddress);
	if (i2c_WaitAck(pI2C) != 0)
	{
		goto cmd_fail;
	}
	i2c_Start(pI2C);
	i2c_SendByte(pI2C,EEPROM_DEV_ADDR | EEPROM_I2C_RD);
	if (i2c_WaitAck(pI2C) != 0)
	{
		goto cmd_fail;
	}	

	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = i2c_ReadByte(pI2C);		
		/* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
		if (i != _usSize - 1)
		{
			i2c_Ack(pI2C);	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
		}
		else
		{
			i2c_NAck(pI2C);	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
		}
	}
	i2c_Stop(pI2C);
	return 0;
	cmd_fail: 
		i2c_Stop(pI2C);
		return 1;
}


/**********************************************************************************************************
*	函 数 名: ee_WriteBytes
*	功能说明: 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
*	形    参：_usAddress : 起始地址
*			 			_usSize 	 : 数据长度，单位为字节
*			 			_pWriteBuf : 存放读到的数据的缓冲区指针
*	返 回 值: 成功返回0 失败返回1
**********************************************************************************************************/
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;
	I2C *pI2C; 
  pI2C = &EEPROM;
	
	/* 
		写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
		对于24xx02，page size = 8
		简单的处理方法为：按字节写操作模式，没写1个字节，都发送地址
		为了提高连续写的效率: 本函数采用page wirte操作。
	*/

	usAddr = _usAddress;	
	for (i = 0; i < _usSize; i++)
	{
		/* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
		if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
		{
			/*　第０步：发停止信号，启动内部写操作　*/
			i2c_Stop(pI2C);		
			/* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms 			
				CLK频率为200KHz时，查询次数为30次左右
			*/
			for (m = 0; m < 1000; m++)
			{				
				i2c_Start(pI2C);
				i2c_SendByte(pI2C,EEPROM_DEV_ADDR | EEPROM_I2C_WR);
				if (i2c_WaitAck(pI2C) == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	/* EEPROM器件写超时 */
			}
		
			/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
			i2c_SendByte(pI2C,(uint8_t)usAddr);
			if (i2c_WaitAck(pI2C) != 0)
			{
				goto cmd_fail;	/* EEPROM器件无应答 */
			}
		}
		i2c_SendByte(pI2C,_pWriteBuf[i]);
		if (i2c_WaitAck(pI2C) != 0)
		{
			goto cmd_fail;	/* EEPROM器件无应答 */
		}
		usAddr++;	/* 地址增1 */		
	}
	
	/* 命令执行成功，发送I2C总线停止信号 */
	i2c_Stop(pI2C);
	return 0;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop(pI2C);
	return 1;
}


/**********************************************************************************************************
*	函 数 名: ee_Erase
*	功能说明: 擦除数据（全部写1）
*	形    参：无
*	返 回 值: 成功返回0 失败返回1
**********************************************************************************************************/
uint8_t ee_Erase(void)
{

	uint16_t i;
	uint8_t buf[EEPROM_SIZE];
	for (i = 0; i < EEPROM_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
	if (ee_WriteBytes(buf, 0, EEPROM_SIZE) == 0)/* 写EEPROM, 起始地址 = 0，数据长度为 256 */
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

