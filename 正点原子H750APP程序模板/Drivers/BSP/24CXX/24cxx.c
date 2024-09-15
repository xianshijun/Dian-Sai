/**
 ****************************************************************************************************
 * @file        24cxx.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       24CXX驱动代码
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

#include "24cxx.h"
#include "delay.h"
#include "myiic.h"

/**
 * @brief   初始化AT24CXX
 * @param   无
 * @retval  无
 */
void at24cxx_init(void)
{
    iic_init();
}

/**
 * @brief   检测AT24CXX是否正常
 * @note    检测原理: 在AT24CXX的末地址写入0x55，然后在读出，
 *          如果读出的值为0x55，则表示AT24CXX正常，反之则异常
 * @param   无
 * @retval  检测结果
 *          0: 正常
 *          1: 异常
 */
uint8_t at24cxx_check(void)
{
    uint8_t data;
    uint16_t addr;
    
    addr = EE_TYPE;
    data = at24cxx_read_one_byte(addr); /* 避免每次开机都写AT24CXX */
    if (data == 0x55)                   /* 读出的数据正常 */
    {
        return 0;
    }
    
    at24cxx_write_one_byte(addr, 0x55); /* 先写入数据 */
    data = at24cxx_read_one_byte(addr); /* 在读取数据 */
    if (data == 0x55)                   /* 读出的数据正常 */
    {
        return 0;
    }
    
    return 1;
}

/**
 * @brief   从AT24CXX指定地址读取一字节数据
 * @param   addr: 指定读取一字节数据的地址
 * @retval  读取到的一字节数据
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t data;
    
    /* 根据不同的24CXX型号，发送高位地址
     * 1，24C16以上的型号，分2个字节发送地址
     * 2，24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址）
     *    对于24C01/02，其器件地址格式（8bit）为: 1 0 1 0 A2  A1 A0 R/W
     *    对于24C04，   其器件地址格式（8bit）为: 1 0 1 0 A2  A1 a8 R/W
     *    对于24C08，   其器件地址格式（8bit）为: 1 0 1 0 A2  a9 a8 R/W
     *    对于24C16，   其器件地址格式（8bit）为: 1 0 1 0 a10 a9 a8 R/W
     *    R/W      : 读/写控制位，0：写；1：读
     *    A0/A1/A2 : 对应器件的1/2/3引脚（只有24C01/02/04/08有这些脚）
     *    a8/a9/a10: 对应存储整列的高位地址，11bit地址最多可以表示2048个位置，可以寻址24C16及以内的型号
     */
    iic_start();                                    /* 产生IIC起始信号 */
    if (EE_TYPE > AT24C16)                          /* 24C16以上的型号，分2个字节发送地址 */
    {
        iic_send_byte(0xA0);                        /* 发送写命令 */
        iic_wait_ack();                             /* 每发送完一个字节都要等待ACK */
        iic_send_byte(addr >> 8);                   /* 发送高字节地址 */
    }
    else                                            /* 24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址） */
    {
        iic_send_byte(0xA0 + ((addr >> 8) << 1));   /* 发送0xA0+高位a8/a9/a10地址，写命令 */
    }
    iic_wait_ack();                                 /* 每发送完一个字节都要等待ACK */
    iic_send_byte(addr % 256);                      /* 发送低位地址 */
    iic_wait_ack();
    
    iic_start();                                    /* 产生IIC起始信号 */
    iic_send_byte(0xA1);                            /* 发送读命令 */
    iic_wait_ack();
    data = iic_read_byte(0);                        /* IIC读取一个字节 */
    iic_stop();                                     /* 产生IIC停止信号 */
    
    return data;
}

/**
 * @brief   往AT24CXX指定地址写入一字节数据
 * @param   addr: 指定写入一字节数据的地址
 * @param   data: 指定写入的一字节数据
 * @retval  无
 */
void at24cxx_write_one_byte(uint16_t addr,uint8_t data)
{
    /* 根据不同的24CXX型号，发送高位地址
     * 1，24C16以上的型号，分2个字节发送地址
     * 2，24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址）
     *    对于24C01/02，其器件地址格式（8bit）为: 1 0 1 0 A2  A1 A0 R/W
     *    对于24C04，   其器件地址格式（8bit）为: 1 0 1 0 A2  A1 a8 R/W
     *    对于24C08，   其器件地址格式（8bit）为: 1 0 1 0 A2  a9 a8 R/W
     *    对于24C16，   其器件地址格式（8bit）为: 1 0 1 0 a10 a9 a8 R/W
     *    R/W      : 读/写控制位，0：写；1：读
     *    A0/A1/A2 : 对应器件的1/2/3引脚（只有24C01/02/04/08有这些脚）
     *    a8/a9/a10: 对应存储整列的高位地址，11bit地址最多可以表示2048个位置，可以寻址24C16及以内的型号
     */
    iic_start();                                    /* 产生IIC起始信号 */
    if (EE_TYPE > AT24C16)                          /* 24C16以上的型号，分2个字节发送地址 */
    {
        iic_send_byte(0xA0);                        /* 发送写命令 */
        iic_wait_ack();                             /* 每发送完一个字节都要等待ACK */
        iic_send_byte(addr >> 8);                   /* 发送高字节地址 */
    }
    else                                            /* 24C16及以下的型号，发送1个低字节地址 + 占用器件地址的bit1~bit3位（用于表示高位地址，最多11位地址） */
    {
        iic_send_byte(0xA0 + ((addr >> 8) << 1));   /* 发送0xA0+高位a8/a9/a10地址，写命令 */
    }
    iic_wait_ack();                                 /* 每发送完一个字节都要等待ACK */
    iic_send_byte(addr % 256);                      /* 发送低位地址 */
    iic_wait_ack();
    
    iic_send_byte(data);                            /* IIC发送一个字节 */
    iic_wait_ack();
    iic_stop();                                     /* 产生IIC停止信号 */
    delay_ms(10);                                   /* EEPROM的写入比较慢，必须等到10ms后再写下一个字节 */
}

/**
 * @brief   从AT24CXX指定地址开始读取指定长度的数据
 * @param   addr: 指定读取数据的起始地址
 * @param   pbuf: 读出数据的保存地址
 * @param   datalen: 指定读取数据的长度
 * @retval  无
 */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while ((datalen--) != 0)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief   往AT24CXX指定地址开始写入指定长度的数据
 * @param   addr: 指定写入数据的起始地址
 * @param   pbuf: 待写数据的保存地址
 * @param   datalen: 指定写入数据的长度
 * @retval  无
 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while ((datalen--) != 0)
    {
        at24cxx_write_one_byte(addr++, *pbuf++);
    }
}
