/**
 ****************************************************************************************************
 * @file        ctiic.h
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

#ifndef __CTIIC_H
#define __CTIIC_H

#include "delay.h"


/* ���Ŷ��� */
#define CT_IIC_SCL_GPIO_PORT            GPIOH
#define CT_IIC_SCL_GPIO_PIN             GPIO_PIN_6
#define CT_IIC_SCL_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)
#define CT_IIC_SDA_GPIO_PORT            GPIOG
#define CT_IIC_SDA_GPIO_PIN             GPIO_PIN_7
#define CT_IIC_SDA_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOG_CLK_ENABLE(); } while (0)

/* IO���� */
#define CT_IIC_SCL(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define CT_IIC_SDA(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define CT_IIC_SDA_READ                 ((HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* �������� */
void ct_iic_init(void);                 /* ��ʼ�����ݴ�����IIC */
void ct_iic_start(void);                /* �������ݴ�����IIC��ʼ�ź� */
void ct_iic_stop(void);                 /* �������ݴ�����IICֹͣ�ź� */
uint8_t ct_iic_wait_ack(void);          /* �ȴ����ݴ�����IICӦ���ź� */
void ct_iic_ack(void);                  /* �������ݴ�����IIC ACK�ź� */
void ct_iic_nack(void);                 /* �������ݴ�����IIC NACK�ź� */
void ct_iic_send_byte(uint8_t data);    /* ���ݴ�����IIC����һ���ֽ� */
uint8_t ct_iic_read_byte(uint8_t ack);  /* ���ݴ�����IIC��ȡһ���ֽ� */

#endif
