/**
 ****************************************************************************************************
 * @file        myiic.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       IIC驱动代码
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

#include "myiic.h"
#include "delay.h"

/**
 * @brief   IIC延时函数
 * @note    用于控制IIC通信速率
 * @param   无
 * @retval  无
 */
static void iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief   初始化IIC
 * @param   无
 * @retval  无
 */
void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* 时钟使能 */
    IIC_SCL_GPIO_CLK_ENABLE();
    IIC_SDA_GPIO_CLK_ENABLE();
    
    /* 配置SCL引脚 */
    gpio_init_struct.Pin = IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* 配置SDA引脚 */
    gpio_init_struct.Pin = IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    iic_stop();
		printf("%s\r\n",__FUNCTION__);
}

/**
 * @brief   产生IIC起始信号
 * @param   无
 * @retval  无
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
 * @brief   产生IIC停止信号
 * @param   无
 * @retval  无
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
 * @brief   等待IIC应答信号
 * @param   无
 * @retval  等待结果
 * @arg     0: 等待IIC应答信号成功
 * @arg     1: 等待IIC应答信号失败
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
 * @brief   产生IIC ACK信号
 * @param   无
 * @retval  无
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
 * @brief   产生IIC NACK信号
 * @param   无
 * @retval  无
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
 * @brief   IIC发送一个字节
 * @param   data: 待发送的一字节数据
 * @retval  无
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
 * @brief   IIC读取一个字节
 * @param   ack: 响应
 * @arg     0: 发送ACK响应
 * @arg     1: 发送NACK响应
 * @retval  读取到的一字节数据
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
*@功能介绍 使用软件模拟IIC进行写操作
*@注意：PH4-SCL  PH5-SDA的
*@DevAddress IIC设备地址
*@MemAddress 操作寄存器地址
*@Data				写入的数据
****************************************/
void MY_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t Data)
{
	
    // 产生I2C起始信号
    iic_start();
    
    // 发送设备地址和写标志
    iic_send_byte((uint8_t)(DevAddress & 0xFE)); // 确保最后一位为0表示写操作
    if (iic_wait_ack())
    {
        iic_stop();
        return;
    }
    
    // 发送内存地址
    iic_send_byte((uint8_t)MemAddress);
    if (iic_wait_ack())
    {
        iic_stop();
        return;
    }
    
    // 发送数据
    iic_send_byte(Data);
    if (iic_wait_ack())
    {
    }
    
    // 产生I2C停止信号
    iic_stop();

}

