/**
 ****************************************************************************************************
 * @file        ft5206.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       FT5206��������
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

#ifndef __FT5206_H
#define __FT5206_H

#include "delay.h"


/* ���Ŷ��� */
#define FT5206_RST_GPIO_PORT            GPIOI
#define FT5206_RST_GPIO_PIN             GPIO_PIN_8
#define FT5206_RST_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOI_CLK_ENABLE(); } while (0)
#define FT5206_INT_GPIO_PORT            GPIOH
#define FT5206_INT_GPIO_PIN             GPIO_PIN_7
#define FT5206_INT_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)

/* IO���� */
#define FT5206_RST(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define FT5206_INT                      ((HAL_GPIO_ReadPin(FT5206_INT_GPIO_PORT, FT5206_INT_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* FT5206 IIC��д��ַ���� */
#define FT5206_CMD_WR                   0x70    /* д��ַ */
#define FT5206_CMD_RD                   0x71    /* ����ַ */

/* FT5206���ּĴ�����ַ���� */
#define FT5206_DEVIDE_MODE              0x00    /* FT5206ģʽ���ƼĴ��� */
#define FT5206_REG_NUM_FINGER           0x02    /* ����״̬�Ĵ��� */
#define FT5206_TP1_REG                  0x03    /* ��һ�����������ݵ�ַ */
#define FT5206_TP2_REG                  0x09    /* �ڶ������������ݵ�ַ */
#define FT5206_TP3_REG                  0x0F    /* ���������������ݵ�ַ */
#define FT5206_TP4_REG                  0x15    /* ���ĸ����������ݵ�ַ */
#define FT5206_TP5_REG                  0x1B    /* ��������������ݵ�ַ */ 
#define FT5206_ID_G_LIB_VERSION         0xA1    /* �汾 */
#define FT5206_ID_G_MODE                0xA4    /* FT5206�ж�ģʽ���ƼĴ��� */
#define FT5206_ID_G_THGROUP             0x80    /* ������Чֵ���üĴ��� */
#define FT5206_ID_G_PERIODACTIVE        0x88    /* ����״̬�������üĴ��� */

/* �������� */
uint8_t ft5206_init(void);          /* ��ʼ��FT5206 */
uint8_t ft5206_scan(uint8_t mode);  /* ɨ��FT5206�������� */

#endif
