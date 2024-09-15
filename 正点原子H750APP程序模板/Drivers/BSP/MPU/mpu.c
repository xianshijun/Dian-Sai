/**
 ****************************************************************************************************
 * @file        mpu.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       MPU驱动代码
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

#include "mpu.h"

/**
 * @brief   设置MPU区域保护
 * @param   baseaddr: 区域基地址
 * @param   size: 区域大小
 * @param   rnum: MPU区域编号
 * @arg     MPU_REGION_NUMBER0: MPU区域0
 * @arg     MPU_REGION_NUMBER1: MPU区域1
 * @arg     MPU_REGION_NUMBER2: MPU区域2
 * @arg     MPU_REGION_NUMBER3: MPU区域3
 * @arg     MPU_REGION_NUMBER4: MPU区域4
 * @arg     MPU_REGION_NUMBER5: MPU区域5
 * @arg     MPU_REGION_NUMBER6: MPU区域6
 * @arg     MPU_REGION_NUMBER7: MPU区域7
 * @arg     MPU_REGION_NUMBER8: MPU区域8
 * @arg     MPU_REGION_NUMBER9: MPU区域9
 * @arg     MPU_REGION_NUMBER10: MPU区域10
 * @arg     MPU_REGION_NUMBER11: MPU区域11
 * @arg     MPU_REGION_NUMBER12: MPU区域12
 * @arg     MPU_REGION_NUMBER13: MPU区域13
 * @arg     MPU_REGION_NUMBER14: MPU区域14
 * @arg     MPU_REGION_NUMBER15: MPU区域15
 * @param   de: 指令访问状态
 * @arg     MPU_INSTRUCTION_ACCESS_ENABLE: 使能指令访问
 * @arg     MPU_INSTRUCTION_ACCESS_DISABLE: 禁止指令访问
 * @param   ap: 访问权限
 * @arg     MPU_REGION_NO_ACCESS: 无访问
 * @arg     MPU_REGION_PRIV_RW: 仅支持特权读写访问
 * @arg     MPU_REGION_PRIV_RW_URO: 禁止用户写访问
 * @arg     MPU_REGION_FULL_ACCESS: 全访问
 * @arg     MPU_REGION_PRIV_RO: 仅支持特权读访问
 * @arg     MPU_REGION_PRIV_RO_URO: 只读
 * @param   sen: 可共享状态
 * @arg     MPU_ACCESS_SHAREABLE: 使能共享
 * @arg     MPU_ACCESS_NOT_SHAREABLE: 禁止共享
 * @param   cen: 可Cache状态
 * @arg     MPU_ACCESS_CACHEABLE: 使能Cache
 * @arg     MPU_ACCESS_NOT_CACHEABLE: 禁止Cache
 * @param   ben: 可缓冲状态
 * @arg     MPU_ACCESS_BUFFERABLE: 使能缓冲
 * @arg     MPU_ACCESS_NOT_BUFFERABLE: 禁止缓冲
 * @retval  无
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
 * @brief   保护相关存储区域
 * @param   无
 * @retval  无
 */
void mpu_memory_protection(void)
{
    /* 保护整个DTCM，共128K字节，允许指令访问，允许共享，允许cache，禁止缓冲 */
    mpu_set_protection(0x20000000, MPU_REGION_SIZE_128KB, MPU_REGION_NUMBER1, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* 保护整个AXI SRAM，共512K字节，允许指令访问，允许共享，允许Cache，禁止缓冲 */
    mpu_set_protection(0x24000000, MPU_REGION_SIZE_512KB, MPU_REGION_NUMBER2, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* 保护整个SRAM1~SRAM3，共288K字节，允许指令访问，禁止共享，允许Cache，允许缓冲 */
    mpu_set_protection(0x30000000, MPU_REGION_SIZE_512KB, MPU_REGION_NUMBER3, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* 保护整个SRAM4，共64K字节，允许指令访问，禁止共享，允许Cache，允许缓冲 */
    mpu_set_protection(0x38000000, MPU_REGION_SIZE_64KB, MPU_REGION_NUMBER4, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* 保护MCU LCD屏所在的FMC区域，共64M字节，允许指令访问，禁止共享，禁止Cache，禁止缓冲 */
    mpu_set_protection(0x60000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER5, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);
    
    /* 保护SDRAM区域，共64M字节，允许指令访问，允许共享，允许Cache，允许缓冲 */
    mpu_set_protection(0xC0000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER6, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    
    /* 保护整个NAND FLASH区域，共256M字节，禁止指令访问，禁止共享，禁止Cache，禁止缓冲 */
    mpu_set_protection(0x80000000, MPU_REGION_SIZE_256MB, MPU_REGION_NUMBER7, MPU_INSTRUCTION_ACCESS_DISABLE,
                       MPU_REGION_FULL_ACCESS ,MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);
}
