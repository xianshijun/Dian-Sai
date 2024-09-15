/**
 ****************************************************************************************************
 * @file        ctiic.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       电容触摸屏IIC驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 北极星 H750开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "ctiic.h"
#include "delay.h"

/**
 * @brief   电容触摸屏IIC延时函数
 * @note    用于控制电容触摸屏IIC通信速率
 * @param   无
 * @retval  无
 */
static void ct_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief   初始化电容触摸屏IIC
 * @param   无
 * @retval  无
 */
void ct_iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* 时钟使能 */
    CT_IIC_SCL_GPIO_CLK_ENABLE();
    CT_IIC_SDA_GPIO_CLK_ENABLE();
    
    /* 配置SCL引脚 */
    gpio_init_struct.Pin = CT_IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CT_IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* 配置SDA引脚 */
    gpio_init_struct.Pin = CT_IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CT_IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    ct_iic_stop();
}

/**
 * @brief   产生电容触摸屏IIC起始信号
 * @param   无
 * @retval  无
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
 * @brief   产生电容触摸屏IIC停止信号
 * @param   无
 * @retval  无
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
 * @brief   等待电容触摸屏IIC应答信号
 * @param   无
 * @retval  等待结果
 * @arg     0: 等待电容触摸屏IIC应答信号成功
 * @arg     1: 等待电容触摸屏IIC应答信号失败
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
 * @brief   产生电容触摸屏IIC ACK信号
 * @param   无
 * @retval  无
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
 * @brief   产生电容触摸屏IIC NACK信号
 * @param   无
 * @retval  无
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
 * @brief   电容触摸屏IIC发送一个字节
 * @param   data: 待发送的一字节数据
 * @retval  无
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
 * @brief   电容触摸屏IIC读取一个字节
 * @param   ack: 响应
 * @arg     0: 发送ACK响应
 * @arg     1: 发送NACK响应
 * @retval  读取到的一字节数据
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
