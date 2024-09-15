/**
 ****************************************************************************************************
 * @file        ctiic.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       ���ݴ�����IIC��������
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

#include "ctiic.h"
#include "delay.h"

/**
 * @brief   ���ݴ�����IIC��ʱ����
 * @note    ���ڿ��Ƶ��ݴ�����IICͨ������
 * @param   ��
 * @retval  ��
 */
static void ct_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief   ��ʼ�����ݴ�����IIC
 * @param   ��
 * @retval  ��
 */
void ct_iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʱ��ʹ�� */
    CT_IIC_SCL_GPIO_CLK_ENABLE();
    CT_IIC_SDA_GPIO_CLK_ENABLE();
    
    /* ����SCL���� */
    gpio_init_struct.Pin = CT_IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CT_IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* ����SDA���� */
    gpio_init_struct.Pin = CT_IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CT_IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    ct_iic_stop();
}

/**
 * @brief   �������ݴ�����IIC��ʼ�ź�
 * @param   ��
 * @retval  ��
 */
void ct_iic_start(void)
{
    CT_IIC_SDA(1);
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
}

/**
 * @brief   �������ݴ�����IICֹͣ�ź�
 * @param   ��
 * @retval  ��
 */
void ct_iic_stop(void)
{
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SDA(1);
    ct_iic_delay();
}

/**
 * @brief   �ȴ����ݴ�����IICӦ���ź�
 * @param   ��
 * @retval  �ȴ����
 * @arg     0: �ȴ����ݴ�����IICӦ���źųɹ�
 * @arg     1: �ȴ����ݴ�����IICӦ���ź�ʧ��
 */
uint8_t ct_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    CT_IIC_SDA(1);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    
    while (CT_IIC_SDA_READ != 0)
    {
        waittime++;
        if (waittime > 250)
        {
            ct_iic_stop();
            rack = 1;
            break;
        }
    }
    
    CT_IIC_SCL(0);
    ct_iic_delay();
    
    return rack;
}

/**
 * @brief   �������ݴ�����IIC ACK�ź�
 * @param   ��
 * @retval  ��
 */
void ct_iic_ack(void)
{
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
    CT_IIC_SDA(1);
    ct_iic_delay();
}

/**
 * @brief   �������ݴ�����IIC NACK�ź�
 * @param   ��
 * @retval  ��
 */
void ct_iic_nack(void)
{
    CT_IIC_SDA(1);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
}

/**
 * @brief   ���ݴ�����IIC����һ���ֽ�
 * @param   data: �����͵�һ�ֽ�����
 * @retval  ��
 */
void ct_iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        CT_IIC_SDA((data & 0x80) >> 7);
        ct_iic_delay();
        CT_IIC_SCL(1);
        ct_iic_delay();
        CT_IIC_SCL(0);
        data <<= 1;
    }
    
    CT_IIC_SDA(1);
}

/**
 * @brief   ���ݴ�����IIC��ȡһ���ֽ�
 * @param   ack: ��Ӧ
 * @arg     0: ����ACK��Ӧ
 * @arg     1: ����NACK��Ӧ
 * @retval  ��ȡ����һ�ֽ�����
 */
uint8_t ct_iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;
    
    for (i=0; i<8; i++)
    {
        receive <<= 1;
        CT_IIC_SCL(1);
        ct_iic_delay();
        
        if (CT_IIC_SDA_READ)
        {
            receive++;
        }
        
        CT_IIC_SCL(0);
        ct_iic_delay();
    }
    
    if (ack == 0)
    {
        ct_iic_nack();
    }
    else
    {
        ct_iic_ack();
    }
    
    return receive;
}
