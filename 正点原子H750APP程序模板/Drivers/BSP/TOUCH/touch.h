/**
 ****************************************************************************************************
 * @file        touch.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       ���ݴ�������������
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

#ifndef __TOUCH_H
#define __TOUCH_H

#include "delay.h"


/* ���Ŷ��� */
#define T_PEN_GPIO_PORT             GPIOH
#define T_PEN_GPIO_PIN              GPIO_PIN_7
#define T_PEN_GPIO_CLK_ENABLE()     do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)
#define T_CS_GPIO_PORT              GPIOI
#define T_CS_GPIO_PIN               GPIO_PIN_8
#define T_CS_GPIO_CLK_ENABLE()      do { __HAL_RCC_GPIOI_CLK_ENABLE(); } while (0)
#define T_MISO_GPIO_PORT            GPIOG
#define T_MISO_GPIO_PIN             GPIO_PIN_3
#define T_MISO_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOG_CLK_ENABLE(); } while (0)
#define T_MOSI_GPIO_PORT            GPIOG
#define T_MOSI_GPIO_PIN             GPIO_PIN_7
#define T_MOSI_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOG_CLK_ENABLE(); } while (0)
#define T_CLK_GPIO_PORT             GPIOH
#define T_CLK_GPIO_PIN              GPIO_PIN_6
#define T_CLK_GPIO_CLK_ENABLE()     do { __HAL_RCC_GPIOH_CLK_ENABLE(); } while (0)

/* IO���� */
#define T_MOSI(x)                   do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN, GPIO_PIN_RESET);   \
                                    } while (0)
#define T_CLK(x)                    do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN, GPIO_PIN_SET):       \
                                        HAL_GPIO_WritePin(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN, GPIO_PIN_RESET);     \
                                    } while (0)
#define T_CS(x)                     do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(T_CS_GPIO_PORT, T_CS_GPIO_PIN, GPIO_PIN_SET):         \
                                        HAL_GPIO_WritePin(T_CS_GPIO_PORT, T_CS_GPIO_PIN, GPIO_PIN_RESET);       \
                                    } while (0)
#define T_PEN                       ((HAL_GPIO_ReadPin(T_PEN_GPIO_PORT, T_PEN_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define T_MISO                      ((HAL_GPIO_ReadPin(T_MISO_GPIO_PORT, T_MISO_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* ������ض��� */
#define TP_PRES_DOWN                0x8000  /* ���������� */
#define TP_CATH_PRES                0x4000  /* �а��������� */
#define CT_MAX_TOUCH                10      /* ������֧�ֵĵ���,�̶�Ϊ5�� */

/* ���������ƽṹ�� */
typedef struct
{
    uint8_t (*init)(void);      /* ��ʼ�������������� */
    uint8_t (*scan)(uint8_t);   /* ɨ�败������0����Ļɨ�裻1���������� */
    void (*adjust)(void);       /* ������У׼ */
    uint16_t x[CT_MAX_TOUCH];   /* ��ǰ���� */
    uint16_t y[CT_MAX_TOUCH];   /* �����������10�����꣬����������x[0]��y[0]����˴�ɨ��ʱ����������
                                 * ��x[9]��y[9]�洢��һ�ΰ���ʱ������
                                 */
    uint16_t sta;               /* �ʵ�״̬
                                 * b15��1�����£�0���ɿ�
                                 * b14��0��û�а������£�1���а�������
                                 * b13~b10������
                                 * b9~b0�����ݴ��������µĵ�����0��δ���£�1�����£�
                                 */
    /* 5��У׼������У׼����������������ҪУ׼�� */
    float xfac;                 /* 5��У׼��x����������� */
    float yfac;                 /* 5��У׼��y����������� */
    short xc;                   /* ����X��������ֵ��ADֵ�� */
    short yc;                   /* ����Y��������ֵ��ADֵ�� */
    uint8_t touchtype;          /* �����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�
                                 * b0��0���������ʺ�����ΪX���꣬����ΪY�����TP��
                                 *     1���������ʺ�����ΪY���꣬����ΪX�����TP��
                                 * b1~6������
                                 * b7��0��������
                                 *     1��������
                                 */
} _m_tp_dev;
extern _m_tp_dev tp_dev;

/* �������� */
void tp_draw_big_point(uint16_t x, uint16_t y, uint16_t color); /* ��һ����� */
void tp_save_adjust_data(void);                                 /* ����У׼���� */
uint8_t tp_scan(uint8_t mode);                                  /* ������ɨ�� */
void tp_adjust(void);                                           /* ������У׼ */
uint8_t tp_init(void);                                          /* ��ʼ�������� */

#endif
