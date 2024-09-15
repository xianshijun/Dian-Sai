/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-06-12
 * @brief       LCD驱动代码
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

#ifndef __LCD_H
#define __LCD_H

#include "delay.h"

/* LCD部分引脚定义
 * LCD D0~D15引脚直接在函数lcd_init()中定义
 */
#define LCD_WR_GPIO_PORT            GPIOD
#define LCD_WR_GPIO_PIN             GPIO_PIN_5
#define LCD_WR_GPIO_AF              GPIO_AF12_FMC
#define LCD_WR_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define LCD_RD_GPIO_PORT            GPIOD
#define LCD_RD_GPIO_PIN             GPIO_PIN_4
#define LCD_RD_GPIO_AF              GPIO_AF12_FMC
#define LCD_RD_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define LCD_BL_GPIO_PORT            GPIOB
#define LCD_BL_GPIO_PIN             GPIO_PIN_5
#define LCD_BL_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

#define LCD_CS_GPIO_PORT            GPIOD
#define LCD_CS_GPIO_PIN             GPIO_PIN_7
#define LCD_CS_GPIO_AF              GPIO_AF12_FMC
#define LCD_CS_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define LCD_RS_GPIO_PORT            GPIOD
#define LCD_RS_GPIO_PIN             GPIO_PIN_13
#define LCD_RS_GPIO_AF              GPIO_AF12_FMC
#define LCD_RS_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

/* LCD引脚操作 */
#define LCD_BL(x)                   do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET);   \
                                    } while (0)

/* FMC相关参数定义
 * 默认通过FMC的存储块1来连接LCD，存储块1有4个片选（FMC_NE1~4）对应FMC存储块1的4个区域块
 */
#define LCD_FMC_NEX                1       /* 使用FMC_NE1连接LCD_CS，范围：1~4 */
#define LCD_FMC_AX                 18      /* 使能FMC_A18连接LCD_RS，范围：0~25 */

/* LCD地址结构体 */
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;

/* LCD_BASE的详细计算方法：
 * 一般使用FMC的存储块1来驱动TFTLCD液晶屏（MCU屏），存储块1地址范围总大小为256MB，均分成4区域块：
 * 存储块1区域块1（FMC_NE1）地址范围：0x60000000~0x63FFFFFF
 * 存储块1区域块2（FMC_NE2）地址范围：0x64000000~0x67FFFFFF
 * 存储块1区域块3（FMC_NE3）地址范围：0x68000000~0x6BFFFFFF
 * 存储块1区域块4（FMC_NE4）地址范围：0x6C000000~0x6FFFFFFF
 *
 * 需要根据硬件连接方式选择合适的片选（连接LCD_CS）和地址线（连接LCD_RS）
 * 正点原子 北极星 H750开发板使用FMC_NE1连接LCD_CS，FMC_A18连接LCD_RS，16位数据线，计算方法如下：
 * 首先FMC_NE1的基地址为: 0x60000000；FMC_NEx（x=1~4）的基址为: 0x60000000 + (0x4000000 * (x - 1))
 * FMC_A18对应地址值：2^18 * 2 = 0x80000；FMC_Ay（y=0~25）的地址为：2^y * 2
 *
 * LCD->LCD_REG，对应LCD_RS = 0（LCD寄存器）；LCD->LCD_RAM，对应LCD_RS = 1（LCD数据）
 * LCD->LCD_RAM的地址为：0x60000000 + 0x80000 = 0x60080000
 * LCD->LCD_REG的地址为：LCD->LCD_RAM之外的任意地址
 * 由于使用了结构体管理LCD_REG和LCD_RAM（LCD_REG在前，LCD_RAM在后，均为16位数据宽度）
 * 因此结构体的基地址（LCD_BASE） = LCD_RAM - sizeof(LCD_REG) = 0x6007FFFE
 *
 * 更加通用的计算公式为（LCD_CS接片选脚FMC_NEx（x=1~4），（LCD_RS接地址线FMC_Ay（y=0~25））：
 * LCD_BASE = (0x60000000 + (0x4000000 * (x - 1))) | (2^y * 2 - 2)
 * 等效于（使用移位操作）
 * LCD_BASE = (0x60000000 + (0x4000000 * (x - 1))) | ((1 << y) * 2 - 2)
 */
#define LCD_BASE                    (uint32_t)((0x60000000 + (0x4000000 * (LCD_FMC_NEX - 1))) | (((1 << LCD_FMC_AX) * 2) - 2))
#define LCD                         ((LCD_TypeDef *)LCD_BASE)

/* LCD重要参数集 */
typedef struct
{
    uint16_t width;                         /* LCD宽度 */
    uint16_t height;                        /* LCD高度 */
    uint16_t id;                            /* LCD ID */
    uint8_t dir;                            /* 横竖屏标志位，0：竖屏；1：横屏 */
    uint16_t wramcmd;                       /* 开始写GRAM指令 */
    uint16_t setxcmd;                       /* 设置X坐标指令 */
    uint16_t setycmd;                       /* 设置Y坐标指令 */
} _lcd_dev;

/* LCD参数导出 */
extern _lcd_dev lcddev; /* 用于管理LCD重要的参数 */

/* LCD扫描方向定义 */
#define L2R_U2D                     0       /* 从左到右，从上到下 */
#define L2R_D2U                     1       /* 从左到右，从下到上 */
#define R2L_U2D                     2       /* 从右到左，从上到下 */
#define R2L_D2U                     3       /* 从右到左，从下到上 */
#define U2D_L2R                     4       /* 从上到下，从左到右 */
#define U2D_R2L                     5       /* 从上到下，从右到左 */
#define D2U_L2R                     6       /* 从下到上，从左到右 */
#define D2U_R2L                     7       /* 从下到上，从右到左 */
#define DFT_SCAN_DIR                L2R_U2D /* 默认的扫描方向 */

/* 颜色定义 */
#define WHITE                       0xFFFF  /* 白色 */
#define BLACK                       0x0000  /* 黑色 */
#define RED                         0xF800  /* 红色 */
#define GREEN                       0x07E0  /* 绿色 */
#define BLUE                        0x001F  /* 蓝色 */
#define MAGENTA                     0xF81F  /* 品红色 */
#define YELLOW                      0xFFE0  /* 黄色 */
#define CYAN                        0x07FF  /* 青色 */
#define BROWN                       0xBC40  /* 棕色 */
#define BRRED                       0xFC07  /* 棕红色 */
#define GRAY                        0x8430  /* 灰色 */
#define DARKBLUE                    0x01CF  /* 深蓝色 */
#define LIGHTBLUE                   0x7D7C  /* 浅蓝色 */
#define GRAYBLUE                    0x5458  /* 灰蓝色 */
#define LIGHTGREEN                  0x841F  /* 浅绿色 */
#define LGRAY                       0xC618  /* 浅灰色 */
#define LGRAYBLUE                   0xA651  /* 浅灰蓝色 */
#define LBBLUE                      0x2B12  /* 浅棕蓝色 */

/* SSD1963相关配置定义（一般无需修改） */
#define SSD_HOR_RESOLUTION          800     /* LCD水平分辨率 */
#define SSD_VER_RESOLUTION          480     /* LCD垂直分辨率 */
#define SSD_HOR_PULSE_WIDTH         1       /* 水平脉宽 */
#define SSD_HOR_BACK_PORCH          46      /* 水平前廊 */
#define SSD_HOR_FRONT_PORCH         210     /* 水平后廊 */
#define SSD_VER_PULSE_WIDTH         1       /* 垂直脉宽 */
#define SSD_VER_BACK_PORCH          23      /* 垂直前廊 */
#define SSD_VER_FRONT_PORCH         22      /* 垂直前廊 */
#define SSD_HT                      (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS                     (SSD_HOR_BACK_PORCH)
#define SSD_VT                      (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS                     (SSD_VER_BACK_PORCH)

/* 绘制LCD时的背景色 */
extern uint32_t g_back_color;               /* 默认为白色 */

/* 函数声明 */
void lcd_wr_data(volatile uint16_t data);                                                                               /* LCD写数据 */
void lcd_wr_regno(volatile uint16_t regno);                                                                             /* LCD写寄存器编号或地址 */
void lcd_write_reg(uint16_t regno, uint16_t data);                                                                      /* LCD写寄存器 */
void lcd_init(void);                                                                                                    /* 初始化LCD */
void lcd_display_on(void);                                                                                              /* LCD开启显示 */
void lcd_display_off(void);                                                                                             /* LCD关闭显示 */
void lcd_scan_dir(uint8_t dir);                                                                                         /* 设置LCD的自动扫描方向（对RGB屏无效） */
void lcd_display_dir(uint8_t dir);                                                                                      /* 设置LCD显示方向 */
void lcd_ssd_backlight_set(uint8_t pwm);                                                                                /* 设置SSD1963背光亮度 */
void lcd_write_ram_prepare(void);                                                                                       /* 准备写GRAM */
void lcd_set_cursor(uint16_t x, uint16_t y);                                                                            /* 设置光标位置（对RGB屏无效） */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);                                         /* 设置窗口（对RGB屏无效） */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);                                                            /* 画点 */
uint32_t lcd_read_point(uint16_t x, uint16_t y);                                                                        /* 读取个某点的颜色值 */
void lcd_clear(uint16_t color);                                                                                         /* 清屏 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);                                      /* 在指定区域内填充单个颜色 */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);                               /* 在指定区域内填充指定颜色块 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                                 /* 画线 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                                              /* 画水平线 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                            /* 画矩形 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);                                              /* 画圆 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                                               /* 画实心圆 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);                       /* 在指定位置显示一个字符 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);                     /* 显示len个数字 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);      /* 扩展显示len个数字（显示高位0） */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);   /* 显示字符串 */


#endif
