/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   AT24C02测试（软件模拟I2C）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h" 
#include "./i2c/iic_eeprom.h"
#include "spi_w25q64.h"
 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
		uint32_t temp = 0;
   //// LED_GPIO_Config();
	//	EEPROM_Init();
   // LED_BLUE;
		
    /* 串口初始化 */
		//USART_Config();
		W25Q64_Init();
	//	printf("eeprom 软件模拟i2c测试例程 \r\n");		
  
		
		temp = SPI_FLASH_ReadID();
		printf("%x",temp);
		
		
		
    while(1);
}
/*********************************************END OF FILE**********************/
