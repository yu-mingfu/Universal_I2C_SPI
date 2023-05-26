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
*	�� �� ��: bsp_InitI2C
*	����˵��: ��ʼ��GPIO
*	��    �Σ���
*	�� �� ֵ: ��
**********************************************************************************************************/
static void bsp_InitI2C(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);	/* ��GPIOʱ�� */

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* ��Ϊ����� */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);
}


/**********************************************************************************************************
*	�� �� ��: EEPROM_Init
*	����˵��: ��ʼ���ӿ�
*	��    �Σ���
*	�� �� ֵ: ��
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
   
    i2c_Stop( pI2C );   /* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
    
    bsp_InitI2C();    //��ʼ��IO
}


/**********************************************************************************************************
*	�� �� ��: ee_CheckOk
*	����˵��: �жϴ���EERPOM�Ƿ�����
*	��    �Σ���
*	�� �� ֵ: �ɹ�����0 ʧ�ܷ���1
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
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		i2c_Stop(pI2C);		
		return 1;
	}
}


/**********************************************************************************************************
*	�� �� ��: ee_ReadBytes
*	����˵��: �Ӵ���EEPROMָ����ַ����ʼ��ȡ��������
*	��    �Σ�_usAddress : ��ʼ��ַ
*			 			_usSize 	 : ���ݳ��ȣ���λΪ�ֽ�
*			 			_pReadBuf  : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: �ɹ�����0 ʧ�ܷ���1
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
	//�����ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
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
		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != _usSize - 1)
		{
			i2c_Ack(pI2C);	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			i2c_NAck(pI2C);	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	i2c_Stop(pI2C);
	return 0;
	cmd_fail: 
		i2c_Stop(pI2C);
		return 1;
}


/**********************************************************************************************************
*	�� �� ��: ee_WriteBytes
*	����˵��: ����EEPROMָ����ַд���������ݣ�����ҳд�������д��Ч��
*	��    �Σ�_usAddress : ��ʼ��ַ
*			 			_usSize 	 : ���ݳ��ȣ���λΪ�ֽ�
*			 			_pWriteBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: �ɹ�����0 ʧ�ܷ���1
**********************************************************************************************************/
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;
	I2C *pI2C; 
  pI2C = &EEPROM;
	
	/* 
		д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��
		����24xx02��page size = 8
		�򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ûд1���ֽڣ������͵�ַ
		Ϊ���������д��Ч��: ����������page wirte������
	*/

	usAddr = _usAddress;	
	for (i = 0; i < _usSize; i++)
	{
		/* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
		if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
		{
			/*���ڣ�������ֹͣ�źţ������ڲ�д������*/
			i2c_Stop(pI2C);		
			/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms 			
				CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
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
				goto cmd_fail;	/* EEPROM����д��ʱ */
			}
		
			/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
			i2c_SendByte(pI2C,(uint8_t)usAddr);
			if (i2c_WaitAck(pI2C) != 0)
			{
				goto cmd_fail;	/* EEPROM������Ӧ�� */
			}
		}
		i2c_SendByte(pI2C,_pWriteBuf[i]);
		if (i2c_WaitAck(pI2C) != 0)
		{
			goto cmd_fail;	/* EEPROM������Ӧ�� */
		}
		usAddr++;	/* ��ַ��1 */		
	}
	
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	i2c_Stop(pI2C);
	return 0;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop(pI2C);
	return 1;
}


/**********************************************************************************************************
*	�� �� ��: ee_Erase
*	����˵��: �������ݣ�ȫ��д1��
*	��    �Σ���
*	�� �� ֵ: �ɹ�����0 ʧ�ܷ���1
**********************************************************************************************************/
uint8_t ee_Erase(void)
{

	uint16_t i;
	uint8_t buf[EEPROM_SIZE];
	for (i = 0; i < EEPROM_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
	if (ee_WriteBytes(buf, 0, EEPROM_SIZE) == 0)/* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

