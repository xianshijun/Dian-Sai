/*
*
*
*
*
*
*
*
*
*
*/
#ifndef __MINOR_H
#define __MINOR_H
///////////////////////预处理/////////////////////////////
#include "delay.h"
#include "lcd.h"
#include "MY_ADC.h"
///////////////////结构体以及全局变量定义/////////////////
typedef struct //坐标结构体
{
	uint16_t x1;//起始坐标
	uint16_t y1;
	uint16_t x2;//截止坐标
	uint16_t y2;
}coord;

extern const coord Screen;//屏幕范围


//////////////////////////////////////////////////////////
void Waveform_rendering(uint16_t Arr[]);
void Oscilloscope_Window(void);
void Minor_Function(void);



#endif
