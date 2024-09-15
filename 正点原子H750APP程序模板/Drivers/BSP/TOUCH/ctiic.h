/**
 ****************************************************************************************************
 * @file        ctiic.h
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

#ifndef __CTIIC_H
#define __CTIIC_H

#include "delay.h"


/* 引脚定义 */
#define CT_IIC_SCL_GPIO_PORT            GPIOH
#define CT_IIC_SCL_GPIO_PIN             GPIO_PIN_6
#define CT_IIC_SCL_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)
#define CT_IIC_SDA_GPIO_PORT            GPIOG
#define CT_IIC_SDA_GPIO_PIN             GPIO_PIN_7
#define CT_IIC_SDA_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOG_CLK_ENABLE(); } while (0)

/* IO操作 */
#define CT_IIC_SCL(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define CT_IIC_SDA(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define CT_IIC_SDA_READ                 ((HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* 函数声明 */
void ct_iic_init(void);                 /* 初始化电容触摸屏IIC */
void ct_iic_start(void);                /* 产生电容触摸屏IIC起始信号 */
void ct_iic_stop(void);                 /* 产生电容触摸屏IIC停止信号 */
uint8_t ct_iic_wait_ack(void);          /* 等待电容触摸屏IIC应答信号 */
void ct_iic_ack(void);                  /* 产生电容触摸屏IIC ACK信号 */
void ct_iic_nack(void);                 /* 产生电容触摸屏IIC NACK信号 */
void ct_iic_send_byte(uint8_t data);    /* 电容触摸屏IIC发送一个字节 */
uint8_t ct_iic_read_byte(uint8_t ack);  /* 电容触摸屏IIC读取一个字节 */

#endif
