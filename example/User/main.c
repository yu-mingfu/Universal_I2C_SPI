/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   AT24C02���ԣ����ģ��I2C��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h" 
#include "./i2c/iic_eeprom.h"
#include "spi_w25q64.h"
 /**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
		uint32_t temp = 0;
   //// LED_GPIO_Config();
	//	EEPROM_Init();
   // LED_BLUE;
		
    /* ���ڳ�ʼ�� */
		//USART_Config();
		W25Q64_Init();
	//	printf("eeprom ���ģ��i2c�������� \r\n");		
  
		
		temp = SPI_FLASH_ReadID();
		printf("%x",temp);
		
		
		
    while(1);
}
/*********************************************END OF FILE**********************/
