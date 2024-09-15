/**
 ****************************************************************************************************
 * @file        24cxx.c
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

#include "24cxx.h"
#include "delay.h"
#include "myiic.h"

/**
 * @brief   ��ʼ��AT24CXX
 * @param   ��
 * @retval  ��
 */
void at24cxx_init(void)
{
    iic_init();
}

/**
 * @brief   ���AT24CXX�Ƿ�����
 * @note    ���ԭ��: ��AT24CXX��ĩ��ַд��0x55��Ȼ���ڶ�����
 *          ���������ֵΪ0x55�����ʾAT24CXX��������֮���쳣
 * @param   ��
 * @retval  �����
 *          0: ����
 *          1: �쳣
 */
uint8_t at24cxx_check(void)
{
    uint8_t data;
    uint16_t addr;
    
    addr = EE_TYPE;
    data = at24cxx_read_one_byte(addr); /* ����ÿ�ο�����дAT24CXX */
    if (data == 0x55)                   /* �������������� */
    {
        return 0;
    }
    
    at24cxx_write_one_byte(addr, 0x55); /* ��д������ */
    data = at24cxx_read_one_byte(addr); /* �ڶ�ȡ���� */
    if (data == 0x55)                   /* �������������� */
    {
        return 0;
    }
    
    return 1;
}

/**
 * @brief   ��AT24CXXָ����ַ��ȡһ�ֽ�����
 * @param   addr: ָ����ȡһ�ֽ����ݵĵ�ַ
 * @retval  ��ȡ����һ�ֽ�����
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t data;
    
    /* ���ݲ�ͬ��24CXX�ͺţ����͸�λ��ַ
     * 1��24C16���ϵ��ͺţ���2���ֽڷ��͵�ַ
     * 2��24C16�����µ��ͺţ�����1�����ֽڵ�ַ + ռ��������ַ��bit1~bit3λ�����ڱ�ʾ��λ��ַ�����11λ��ַ��
     *    ����24C01/02����������ַ��ʽ��8bit��Ϊ: 1 0 1 0 A2  A1 A0 R/W
     *    ����24C04��   ��������ַ��ʽ��8bit��Ϊ: 1 0 1 0 A2  A1 a8 R/W
     *    ����24C08��   ��������ַ��ʽ��8bit��Ϊ: 1 0 1 0 A2  a9 a8 R/W
     *    ����24C16��   ��������ַ��ʽ��8bit��Ϊ: 1 0 1 0 a10 a9 a8 R/W
     *    R/W      : ��/д����λ��0��д��1����
     *    A0/A1/A2 : ��Ӧ������1/2/3���ţ�ֻ��24C01/02/04/08����Щ�ţ�
     *    a8/a9/a10: ��Ӧ�洢���еĸ�λ��ַ��11bit��ַ�����Ա�ʾ2048��λ�ã�����Ѱַ24C16�����ڵ��ͺ�
     */
    iic_start();                                    /* ����IIC��ʼ�ź� */
    if (EE_TYPE > AT24C16)                          /* 24C16���ϵ��ͺţ���2���ֽڷ��͵�ַ */
    {
        iic_send_byte(0xA0);                        /* ����д���� */
        iic_wait_ack();                             /* ÿ������һ���ֽڶ�Ҫ�ȴ�ACK */
        iic_send_byte(addr >> 8);                   /* ���͸��ֽڵ�ַ */
    }
    else                                            /* 24C16�����µ��ͺţ�����1�����ֽڵ�ַ + ռ��������ַ��bit1~bit3λ�����ڱ�ʾ��λ��ַ�����11λ��ַ�� */
    {
        iic_send_byte(0xA0 + ((addr >> 8) << 1));   /* ����0xA0+��λa8/a9/a10��ַ��д���� */
    }
    iic_wait_ack();                                 /* ÿ������һ���ֽڶ�Ҫ�ȴ�ACK */
    iic_send_byte(addr % 256);                      /* ���͵�λ��ַ */
    iic_wait_ack();
    
    iic_start();                                    /* ����IIC��ʼ�ź� */
    iic_send_byte(0xA1);                            /* ���Ͷ����� */
    iic_wait_ack();
    data = iic_read_byte(0);                        /* IIC��ȡһ���ֽ� */
    iic_stop();                                     /* ����IICֹͣ�ź� */
    
    return data;
}

/**
 * @brief   ��AT24CXXָ����ַд��һ�ֽ�����
 * @param   addr: ָ��д��һ�ֽ����ݵĵ�ַ
 * @param   data: ָ��д���һ�ֽ�����
 * @retval  ��
 */
void at24cxx_write_one_byte(uint16_t addr,uint8_t data)
{
    /* ���ݲ�ͬ��24CXX�ͺţ����͸�λ��ַ
     * 1��24C16���ϵ��ͺţ���2���ֽڷ��͵�ַ
     * 2��24C16�����µ��ͺţ�����1�����ֽڵ�ַ + ռ��������ַ��bit1~bit3λ�����ڱ�ʾ��λ��ַ�����11λ��ַ��
     *    ����24C01/02����������ַ��ʽ��8bit��Ϊ: 1 0 1 0 A2  A1 A0 R/W
     *    ����24C04��   ��������ַ��ʽ��8bit��Ϊ: 1 0 1 0 A2  A1 a8 R/W
     *    ����24C08��   ��������ַ��ʽ��8bit��Ϊ: 1 0 1 0 A2  a9 a8 R/W
     *    ����24C16��   ��������ַ��ʽ��8bit��Ϊ: 1 0 1 0 a10 a9 a8 R/W
     *    R/W      : ��/д����λ��0��д��1����
     *    A0/A1/A2 : ��Ӧ������1/2/3���ţ�ֻ��24C01/02/04/08����Щ�ţ�
     *    a8/a9/a10: ��Ӧ�洢���еĸ�λ��ַ��11bit��ַ�����Ա�ʾ2048��λ�ã�����Ѱַ24C16�����ڵ��ͺ�
     */
    iic_start();                                    /* ����IIC��ʼ�ź� */
    if (EE_TYPE > AT24C16)                          /* 24C16���ϵ��ͺţ���2���ֽڷ��͵�ַ */
    {
        iic_send_byte(0xA0);                        /* ����д���� */
        iic_wait_ack();                             /* ÿ������һ���ֽڶ�Ҫ�ȴ�ACK */
        iic_send_byte(addr >> 8);                   /* ���͸��ֽڵ�ַ */
    }
    else                                            /* 24C16�����µ��ͺţ�����1�����ֽڵ�ַ + ռ��������ַ��bit1~bit3λ�����ڱ�ʾ��λ��ַ�����11λ��ַ�� */
    {
        iic_send_byte(0xA0 + ((addr >> 8) << 1));   /* ����0xA0+��λa8/a9/a10��ַ��д���� */
    }
    iic_wait_ack();                                 /* ÿ������һ���ֽڶ�Ҫ�ȴ�ACK */
    iic_send_byte(addr % 256);                      /* ���͵�λ��ַ */
    iic_wait_ack();
    
    iic_send_byte(data);                            /* IIC����һ���ֽ� */
    iic_wait_ack();
    iic_stop();                                     /* ����IICֹͣ�ź� */
    delay_ms(10);                                   /* EEPROM��д��Ƚ���������ȵ�10ms����д��һ���ֽ� */
}

/**
 * @brief   ��AT24CXXָ����ַ��ʼ��ȡָ�����ȵ�����
 * @param   addr: ָ����ȡ���ݵ���ʼ��ַ
 * @param   pbuf: �������ݵı����ַ
 * @param   datalen: ָ����ȡ���ݵĳ���
 * @retval  ��
 */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while ((datalen--) != 0)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief   ��AT24CXXָ����ַ��ʼд��ָ�����ȵ�����
 * @param   addr: ָ��д�����ݵ���ʼ��ַ
 * @param   pbuf: ��д���ݵı����ַ
 * @param   datalen: ָ��д�����ݵĳ���
 * @retval  ��
 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while ((datalen--) != 0)
    {
        at24cxx_write_one_byte(addr++, *pbuf++);
    }
}
