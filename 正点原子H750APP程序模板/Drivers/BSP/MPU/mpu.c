/**
 ****************************************************************************************************
 * @file        mpu.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       MPU��������
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

#include "mpu.h"

/**
 * @brief   ����MPU���򱣻�
 * @param   baseaddr: �������ַ
 * @param   size: �����С
 * @param   rnum: MPU������
 * @arg     MPU_REGION_NUMBER0: MPU����0
 * @arg     MPU_REGION_NUMBER1: MPU����1
 * @arg     MPU_REGION_NUMBER2: MPU����2
 * @arg     MPU_REGION_NUMBER3: MPU����3
 * @arg     MPU_REGION_NUMBER4: MPU����4
 * @arg     MPU_REGION_NUMBER5: MPU����5
 * @arg     MPU_REGION_NUMBER6: MPU����6
 * @arg     MPU_REGION_NUMBER7: MPU����7
 * @arg     MPU_REGION_NUMBER8: MPU����8
 * @arg     MPU_REGION_NUMBER9: MPU����9
 * @arg     MPU_REGION_NUMBER10: MPU����10
 * @arg     MPU_REGION_NUMBER11: MPU����11
 * @arg     MPU_REGION_NUMBER12: MPU����12
 * @arg     MPU_REGION_NUMBER13: MPU����13
 * @arg     MPU_REGION_NUMBER14: MPU����14
 * @arg     MPU_REGION_NUMBER15: MPU����15
 * @param   de: ָ�����״̬
 * @arg     MPU_INSTRUCTION_ACCESS_ENABLE: ʹ��ָ�����
 * @arg     MPU_INSTRUCTION_ACCESS_DISABLE: ��ָֹ�����
 * @param   ap: ����Ȩ��
 * @arg     MPU_REGION_NO_ACCESS: �޷���
 * @arg     MPU_REGION_PRIV_RW: ��֧����Ȩ��д����
 * @arg     MPU_REGION_PRIV_RW_URO: ��ֹ�û�д����
 * @arg     MPU_REGION_FULL_ACCESS: ȫ����
 * @arg     MPU_REGION_PRIV_RO: ��֧����Ȩ������
 * @arg     MPU_REGION_PRIV_RO_URO: ֻ��
 * @param   sen: �ɹ���״̬
 * @arg     MPU_ACCESS_SHAREABLE: ʹ�ܹ���
 * @arg     MPU_ACCESS_NOT_SHAREABLE: ��ֹ����
 * @param   cen: ��Cache״̬
 * @arg     MPU_ACCESS_CACHEABLE: ʹ��Cache
 * @arg     MPU_ACCESS_NOT_CACHEABLE: ��ֹCache
 * @param   ben: �ɻ���״̬
 * @arg     MPU_ACCESS_BUFFERABLE: ʹ�ܻ���
 * @arg     MPU_ACCESS_NOT_BUFFERABLE: ��ֹ����
 * @retval  ��
 */
void mpu_set_protection(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t de, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben)
{
    MPU_Region_InitTypeDef mpu_region_init_struct = {0};
    
    mpu_region_init_struct.Enable = MPU_REGION_ENABLE;
    mpu_region_init_struct.Number = rnum;
    mpu_region_init_struct.BaseAddress = baseaddr;
    mpu_region_init_struct.Size = size;
    mpu_region_init_struct.SubRegionDisable = 0x00;
    mpu_region_init_struct.TypeExtField = MPU_TEX_LEVEL0;
    mpu_region_init_struct.AccessPermission = ap;
    mpu_region_init_struct.DisableExec = de;
    mpu_region_init_struct.IsShareable = sen;
    mpu_region_init_struct.IsCacheable = cen;
    mpu_region_init_struct.IsBufferable = ben;
    HAL_MPU_Disable();
    HAL_MPU_ConfigRegion(&mpu_region_init_struct);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief   ������ش洢����
 * @param   ��
 * @retval  ��
 */
void mpu_memory_protection(void)
{
    /* ��������DTCM����128K�ֽڣ�����ָ����ʣ�����������cache����ֹ���� */
    mpu_set_protection(0x20000000, MPU_REGION_SIZE_128KB, MPU_REGION_NUMBER1, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* ��������AXI SRAM����512K�ֽڣ�����ָ����ʣ�����������Cache����ֹ���� */
    mpu_set_protection(0x24000000, MPU_REGION_SIZE_512KB, MPU_REGION_NUMBER2, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* ��������SRAM1~SRAM3����288K�ֽڣ�����ָ����ʣ���ֹ��������Cache�������� */
    mpu_set_protection(0x30000000, MPU_REGION_SIZE_512KB, MPU_REGION_NUMBER3, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* ��������SRAM4����64K�ֽڣ�����ָ����ʣ���ֹ��������Cache�������� */
    mpu_set_protection(0x38000000, MPU_REGION_SIZE_64KB, MPU_REGION_NUMBER4, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* ����MCU LCD�����ڵ�FMC���򣬹�64M�ֽڣ�����ָ����ʣ���ֹ������ֹCache����ֹ���� */
    mpu_set_protection(0x60000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER5, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);
    
    /* ����SDRAM���򣬹�64M�ֽڣ�����ָ����ʣ�����������Cache�������� */
    mpu_set_protection(0xC0000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER6, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* ��������NAND FLASH���򣬹�256M�ֽڣ���ָֹ����ʣ���ֹ������ֹCache����ֹ���� */
    mpu_set_protection(0x80000000, MPU_REGION_SIZE_256MB, MPU_REGION_NUMBER7, MPU_INSTRUCTION_ACCESS_DISABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);
}
