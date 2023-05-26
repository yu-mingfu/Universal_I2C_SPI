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

    //CS���ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;           //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //SCK��MOSI���ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;           //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //MISO���ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;               //��������
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
    spi_Stop( pSPI );   /* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
    
    bsp_InitSPI();    //��ʼ��IO
}


































u32 SPI_FLASH_ReadID(void)
{    
	SPI *pSPI;
    
    
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	pSPI = &W25Q64;
  /* ��ʼͨѶ��CS�͵�ƽ */
  spi_Start(pSPI);
  /* ����JEDECָ���ȡID */
  SPI_ReadWriteByte(pSPI ,0x9F,0);
  /* ��ȡһ���ֽ����� */
  Temp0 = SPI_ReadWriteByte(pSPI ,0xFF,0);
  /* ��ȡһ���ֽ����� */
  Temp1 = SPI_ReadWriteByte(pSPI ,0xFF ,0);
  /* ��ȡһ���ֽ����� */
  Temp2 = SPI_ReadWriteByte(pSPI ,0xFF,0);
 /* ֹͣͨѶ��CS�ߵ�ƽ */
  spi_Stop(pSPI);
  /*�����������������Ϊ�����ķ���ֵ*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;

}




