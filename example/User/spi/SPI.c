#include "SPI.h"


/**********************************************************************************************************
*	函 数 名: spi_Delay
*	功能说明: spi总线位延迟
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/

static void spi_Delay(void)
{
	volatile uint8_t i;

	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us
		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us
		实际应用选择400KHz左右的速率即可
	*/
	for (i = 0; i < 10; i++);
}


/**********************************************************************************************************
*	函 数 名: spi_Start
*	功能说明: CPU发起SPI总线启动信号
*	形    参：spi：结构体指针
*	返 回 值: 无
**********************************************************************************************************/
void spi_Start( SPI *spi )
{
	spi->SPI_NSS_L();
}


/**********************************************************************************************************
*	函 数 名: spi_Stop
*	功能说明: CPU发起SPI总线停止信号
*	形    参：spi：结构体指针
*	返 回 值: 无
**********************************************************************************************************/
void spi_Stop( SPI *spi )
{
	spi->SPI_NSS_H();
}

/**********************************************************************************************************
*	函 数 名: SPI_ReadWriteByte_0（CPOL == 0 CPHA == 0）MSB first
*	功能说明: CPU向SPI总线设备发送并接收8bit数据
*	形    参：spi：结构体指针，txData ： 等待发送的字节
*	返 回 值: 无
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte_0(SPI *spi ,uint8_t txData)
{
	uint8_t i;
  uint8_t rxData;
	for(i=0;i<8;i++)
	{
		if(txData & 0x80)
		{
			spi->SPI_MOSI_H();
		}
		else
		{
			spi->SPI_MOSI_L();
		}
		txData<<=1;
		spi_Delay();
		spi->SPI_SCK_H();  
		rxData<<=1;
		if(spi->SPI_MISO_READ()){
			rxData|=0x01;
		}			
		spi->SPI_SCK_L();
		spi_Delay();
	}
	return rxData;
}


/**********************************************************************************************************
*	函 数 名: SPI_ReadWriteByte_1（CPOL == 0 CPHA == 1）MSB first
*	功能说明: CPU向SPI总线设备发送并接收8bit数据
*	形    参：spi：结构体指针，txData ： 等待发送的字节
*	返 回 值: 无
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte_1(SPI *spi ,uint8_t txData)
{
	uint8_t i;
  uint8_t rxData;
	for(i=0;i<8;i++)
	{
		spi->SPI_SCK_H();
		if(txData&0x80)
		{
			spi->SPI_MOSI_H();
		}
		else
		{
			spi->SPI_MOSI_L();
		}
		txData<<=1;
		spi_Delay();
		spi->SPI_SCK_L();
    
		rxData<<=1;
		if(spi->SPI_MISO_READ()){
			rxData|=0x01;
		}	
		spi_Delay();
	}
	return rxData;
}


/**********************************************************************************************************
*	函 数 名: SPI_ReadWriteByte_2（CPOL == 1 CPHA == 0）MSB first
*	功能说明: CPU向SPI总线设备发送并接收8bit数据
*	形    参：spi：结构体指针，txData ： 等待发送的字节
*	返 回 值: 无
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte_2(SPI *spi ,uint8_t txData)
{
	uint8_t i;
  uint8_t rxData;
		for(i=0;i<8;i++)//以MSB方式按位接收一个字节数据,上升沿一位数据被存入移位寄存器
		{
			if(txData&0x80)
			{
				spi->SPI_MOSI_H();
			}
			else
			{
				spi->SPI_MOSI_L();
			}
			txData<<=1;
			spi_Delay();
		  spi->SPI_SCK_L();	
			rxData<<=1;
			if(spi->SPI_MISO_READ()){
				rxData|=0x01;
			}				
			spi_Delay();
			spi->SPI_SCK_H();
		}
	return rxData;

}


/**********************************************************************************************************
*	函 数 名: SPI_ReadWriteByte_3（CPOL == 1 CPHA == 1）MSB first
*	功能说明: CPU向SPI总线设备发送并接收8bit数据
*	形    参：spi：结构体指针，txData ： 等待发送的字节
*	返 回 值: 无
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte_3(SPI *spi ,uint8_t txData)
{
	uint8_t i;
  uint8_t rxData;
		for(i=0;i<8;i++)//以MSB方式按位接收一个字节数据,上升沿一位数据被存入移位寄存器
		{
			spi->SPI_SCK_L();
			if(txData&0x80)
			{
				spi->SPI_MOSI_H();
			}
			else
			{
				spi->SPI_MOSI_L();
			}
			txData<<=1;
			spi_Delay();
			spi->SPI_SCK_H();
			
			rxData<<=1;
			if(spi->SPI_MISO_READ()){
				rxData|=0x01;
			}
			spi_Delay();			
		}
	return rxData;

}


/**********************************************************************************************************
*	函 数 名: SPI_ReadWriteByte
*	功能说明: CPU向SPI总线设备发送并接收8bit数据
*	形    参：spi：结构体指针，txData ： 等待发送的字节，ucMode：通信模式
*	返 回 值: 无
**********************************************************************************************************/
uint8_t SPI_ReadWriteByte( SPI *spi ,uint8_t txData ,uint8_t ucMode )
{
	uint8_t rxData;
	switch ( ucMode )
	{
		case 0:rxData = SPI_ReadWriteByte_0(spi,txData);break;
		case 1:rxData = SPI_ReadWriteByte_1(spi,txData);break;
		case 2:rxData = SPI_ReadWriteByte_2(spi,txData);break;
		case 3:rxData = SPI_ReadWriteByte_3(spi,txData);break;
		default:rxData = SPI_ReadWriteByte_0(spi,txData);break;
	}
	return rxData;
}


/**********************************************************************************************************
*	函 数 名: SPI_ReadByte
*	功能说明: CPU向SPI总线设备接收8bit数据
*	形    参：spi：结构体指针，ucMode：通信模式
*	返 回 值: 无
**********************************************************************************************************/
uint8_t SPI_ReadByte(SPI *spi,uint8_t ucMode)
{
    return SPI_ReadWriteByte(spi,Dummy_Byte,ucMode);
}
