#include "SPI.h"


/**********************************************************************************************************
*	�� �� ��: spi_Delay
*	����˵��: spi����λ�ӳ�
*	��    �Σ���
*	�� �� ֵ: ��
**********************************************************************************************************/

static void spi_Delay(void)
{
	volatile uint8_t i;

	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us
		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us
		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < 10; i++);
}


/**********************************************************************************************************
*	�� �� ��: spi_Start
*	����˵��: CPU����SPI���������ź�
*	��    �Σ�spi���ṹ��ָ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void spi_Start( SPI *spi )
{
	spi->SPI_NSS_L();
}


/**********************************************************************************************************
*	�� �� ��: spi_Stop
*	����˵��: CPU����SPI����ֹͣ�ź�
*	��    �Σ�spi���ṹ��ָ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void spi_Stop( SPI *spi )
{
	spi->SPI_NSS_H();
}

/**********************************************************************************************************
*	�� �� ��: SPI_ReadWriteByte_0��CPOL == 0 CPHA == 0��MSB first
*	����˵��: CPU��SPI�����豸���Ͳ�����8bit����
*	��    �Σ�spi���ṹ��ָ�룬txData �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
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
*	�� �� ��: SPI_ReadWriteByte_1��CPOL == 0 CPHA == 1��MSB first
*	����˵��: CPU��SPI�����豸���Ͳ�����8bit����
*	��    �Σ�spi���ṹ��ָ�룬txData �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
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
*	�� �� ��: SPI_ReadWriteByte_2��CPOL == 1 CPHA == 0��MSB first
*	����˵��: CPU��SPI�����豸���Ͳ�����8bit����
*	��    �Σ�spi���ṹ��ָ�룬txData �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte_2(SPI *spi ,uint8_t txData)
{
	uint8_t i;
  uint8_t rxData;
		for(i=0;i<8;i++)//��MSB��ʽ��λ����һ���ֽ�����,������һλ���ݱ�������λ�Ĵ���
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
*	�� �� ��: SPI_ReadWriteByte_3��CPOL == 1 CPHA == 1��MSB first
*	����˵��: CPU��SPI�����豸���Ͳ�����8bit����
*	��    �Σ�spi���ṹ��ָ�룬txData �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte_3(SPI *spi ,uint8_t txData)
{
	uint8_t i;
  uint8_t rxData;
		for(i=0;i<8;i++)//��MSB��ʽ��λ����һ���ֽ�����,������һλ���ݱ�������λ�Ĵ���
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
*	�� �� ��: SPI_ReadWriteByte
*	����˵��: CPU��SPI�����豸���Ͳ�����8bit����
*	��    �Σ�spi���ṹ��ָ�룬txData �� �ȴ����͵��ֽڣ�ucMode��ͨ��ģʽ
*	�� �� ֵ: ��
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
*	�� �� ��: SPI_ReadByte
*	����˵��: CPU��SPI�����豸����8bit����
*	��    �Σ�spi���ṹ��ָ�룬ucMode��ͨ��ģʽ
*	�� �� ֵ: ��
**********************************************************************************************************/
uint8_t SPI_ReadByte(SPI *spi,uint8_t ucMode)
{
    return SPI_ReadWriteByte(spi,Dummy_Byte,ucMode);
}
