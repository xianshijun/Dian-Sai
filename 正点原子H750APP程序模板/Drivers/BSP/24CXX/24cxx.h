/**
 ****************************************************************************************************
 * @file        24cxx.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       24CXX��������
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

#ifndef __24CXX_H
#define __24CXX_H

#include "delay.h"


/* 24CXX�ͺŶ��� */
#define AT24C01     ((1 << 7) - 1)
#define AT24C02     ((1 << 8) - 1)
#define AT24C04     ((1 << 9) - 1)
#define AT24C08     ((1 << 10) - 1)
#define AT24C16     ((1 << 11) - 1)
#define AT24C32     ((1 << 12) - 1)
#define AT24C64     ((1 << 13) - 1)
#define AT24C128    ((1 << 14) - 1)
#define AT24C256    ((1 << 15) - 1)

/* ����24CXX�ͺŶ��� */
#define EE_TYPE     AT24C02

/* �������� */
void at24cxx_init(void);                                            /* ��ʼ��AT24CXX */
uint8_t at24cxx_check(void);                                        /* ���AT24CXX�Ƿ����� */
uint8_t at24cxx_read_one_byte(uint16_t addr);                       /* ��AT24CXXָ����ַ��ȡһ�ֽ����� */
void at24cxx_write_one_byte(uint16_t addr,uint8_t data);            /* ��AT24CXXָ����ַд��һ�ֽ����� */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);  /* ��AT24CXXָ����ַ��ʼ��ȡָ�����ȵ����� */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen); /* ��AT24CXXָ����ַ��ʼд��ָ�����ȵ����� */

#endif
