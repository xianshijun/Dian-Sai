/**
 ****************************************************************************************************
 * @file        myiic.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       IIC��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 * 
 * ʵ��ƽ̨:����ԭ�� ������ H750������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "myiic.h"
#include "delay.h"

/**
 * @brief   IIC��ʱ����
 * @note    ���ڿ���IICͨ������
 * @param   ��
 * @retval  ��
 */
static void iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief   ��ʼ��IIC
 * @param   ��
 * @retval  ��
 */
void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʱ��ʹ�� */
    IIC_SCL_GPIO_CLK_ENABLE();
    IIC_SDA_GPIO_CLK_ENABLE();
    
    /* ����SCL���� */
    gpio_init_struct.Pin = IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* ����SDA���� */
    gpio_init_struct.Pin = IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    iic_stop();
		printf("%s\r\n",__FUNCTION__);
}

/**
 * @brief   ����IIC��ʼ�ź�
 * @param   ��
 * @retval  ��
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief   ����IICֹͣ�ź�
 * @param   ��
 * @retval  ��
 */
void iic_stop(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
}

/**
 * @brief   �ȴ�IICӦ���ź�
 * @param   ��
 * @retval  �ȴ����
 * @arg     0: �ȴ�IICӦ���źųɹ�
 * @arg     1: �ȴ�IICӦ���ź�ʧ��
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    
    while (IIC_SDA_READ != 0)
    {
        waittime++;
        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }
    
    IIC_SCL(0);
    iic_delay();
    
    return rack;
}

/**
 * @brief   ����IIC ACK�ź�
 * @param   ��
 * @retval  ��
 */
void iic_ack(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
}

/**
 * @brief   ����IIC NACK�ź�
 * @param   ��
 * @retval  ��
 */
void iic_nack(void)
{
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief   IIC����һ���ֽ�
 * @param   data: �����͵�һ�ֽ�����
 * @retval  ��
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        IIC_SDA((data & 0x80) >> 7);
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        data <<= 1;
    }
    
    IIC_SDA(1);
}

/**
 * @brief   IIC��ȡһ���ֽ�
 * @param   ack: ��Ӧ
 * @arg     0: ����ACK��Ӧ
 * @arg     1: ����NACK��Ӧ
 * @retval  ��ȡ����һ�ֽ�����
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;
    
    for (i=0; i<8; i++)
    {
        receive <<= 1;
        IIC_SCL(1);
        iic_delay();
        
        if (IIC_SDA_READ)
        {
            receive++;
        }
        
        IIC_SCL(0);
        iic_delay();
    }
    
    if (ack == 0)
    {
        iic_nack();
    }
    else
    {
        iic_ack();
    }
    
    return receive;
}


/***********************************************
*@���ܽ��� ʹ�����ģ��IIC����д����
*@ע�⣺PH4-SCL  PH5-SDA��
*@DevAddress IIC�豸��ַ
*@MemAddress �����Ĵ�����ַ
*@Data				д�������
****************************************/
void MY_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t Data)
{
	
    // ����I2C��ʼ�ź�
    iic_start();
    
    // �����豸��ַ��д��־
    iic_send_byte((uint8_t)(DevAddress & 0xFE)); // ȷ�����һλΪ0��ʾд����
    if (iic_wait_ack())
    {
        iic_stop();
        return;
    }
    
    // �����ڴ��ַ
    iic_send_byte((uint8_t)MemAddress);
    if (iic_wait_ack())
    {
        iic_stop();
        return;
    }
    
    // ��������
    iic_send_byte(Data);
    if (iic_wait_ack())
    {
    }
    
    // ����I2Cֹͣ�ź�
    iic_stop();

}

