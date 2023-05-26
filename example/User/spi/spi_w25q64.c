#include "spi_w25q64.h"
#include "SPI.h"

SPI W25Q64;

static uint8_t bsp_spiRead( void )
{
    return SPI_MISO_READ();
}

static void bsp_spi_NssHigh( void )
{
    SPI_NSS_1();
}

static void bsp_spi_NssLow( void )
{
    SPI_NSS_0();
}

static void bsp_spi_MosiHigh( void )
{
    SPI_MOSI_1();
}

static void bsp_spi_MosiLow( void )
{
    SPI_MOSI_0();
}

static void bsp_spi_SckHigh( void )
{
    SPI_SCK_1();
}

static void bsp_spi_SckLow( void )
{
    SPI_SCK_0();
}


static void bsp_InitSPI(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    //CS引脚初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;           //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //SCK和MOSI引脚初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;           //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //MISO引脚初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;               //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void W25Q64_Init( void )
{
    SPI *pSPI;
    
    pSPI = &W25Q64;
    pSPI->SPI_InitPort 	= bsp_InitSPI;
    pSPI->SPI_NSS_L 	 	= bsp_spi_NssLow;
    pSPI->SPI_NSS_H 	 	= bsp_spi_NssHigh;
    pSPI->SPI_MOSI_H 	 	= bsp_spi_MosiHigh;
    pSPI->SPI_MOSI_L   	= bsp_spi_MosiLow;
    pSPI->SPI_SCK_H 	 	=  bsp_spi_SckHigh;
    pSPI->SPI_SCK_L 	 	=  bsp_spi_SckLow;
		pSPI->SPI_MISO_READ =  bsp_spiRead;
    spi_Stop( pSPI );   /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    
    bsp_InitSPI();    //初始化IO
}


































u32 SPI_FLASH_ReadID(void)
{    
	SPI *pSPI;
    
    
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	pSPI = &W25Q64;
  /* 开始通讯：CS低电平 */
  spi_Start(pSPI);
  /* 发送JEDEC指令，读取ID */
  SPI_ReadWriteByte(pSPI ,0x9F,0);
  /* 读取一个字节数据 */
  Temp0 = SPI_ReadWriteByte(pSPI ,0xFF,0);
  /* 读取一个字节数据 */
  Temp1 = SPI_ReadWriteByte(pSPI ,0xFF ,0);
  /* 读取一个字节数据 */
  Temp2 = SPI_ReadWriteByte(pSPI ,0xFF,0);
 /* 停止通讯：CS高电平 */
  spi_Stop(pSPI);
  /*把数据组合起来，作为函数的返回值*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;

}




