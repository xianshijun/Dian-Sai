#ifndef __MYADC_H
#define __MYADC_H
///////////////////////////////////////////////////////////////////
#include "main.h"
#include "stm32h7xx_it.h"
#include "math.h"
#include "arm_math.h"

///////////////////////////////////////////////////////////////////
#define ADC_Quantity 8192
#define FFT_Quantity 8192
#define ADC_Resolution_Ratio 65535


#define fft_psc 1
#define fft_arr 80
//////////////////////////////////�����������ݷ�����ĸ�������//////////////////////////////////////////////////
typedef  struct
{
    int     N;						//��������
    double  fs;						//������
    double  dt;						//����ʱ���� == 1/fs
    double  fm;						//�ź�Ƶ��
    double  VPP;					//���ֵ
    double  mean_value;		//ƽ��ֵ
    float   DC;						//ֱ������
    float   range;				//����
    float   phase;				//��λ
    float   THD;					//��г��ʧ���
		char 		Str1[20];			//�ַ���1
}F_F_T;
extern F_F_T FFT;
///////////////////////////////////*������FFT�ĸ�������*///////////////////////////////////
typedef struct  compx 
{
	float32_t real, imag;//ʵ�� �鲿
}COMPX;   
extern COMPX FFT_ARR[FFT_Quantity];
extern float32_t Be_fm_Arr[1024];//����ɨƵƵ��ͼ

///////////////////////////////////*��������FFT���ͼ��*///////////////////////////////////
typedef struct 
{
		uint16_t index;//��ֵ�±�
		double peak;
		double fs;
}_PEAK;
extern _PEAK Peak_ARR[20];//����ǰ20����ֵ


///////////////////////////////////*�������ADC�ɼ�����ֵ*//////////////////////////////////
extern uint16_t ADC_Val_Arr[ADC_Quantity];

///////////////////////////////////*���޵�FFT��������*///////////////////////////////////
void InitTableFFT(uint32_t n);


///////////////////////////////////*������FFT*///////////////////////////////////
void cfft(struct compx *_ptr, uint32_t FFT_N );


///////////////////////////////////*����ʵʩ����*///////////////////////////////////
void FFT_refresh(void);




///////////////////////////////////*��������THDֵ*///////////////////////////////////
float THD(float fft_outputbuf[]);      



#endif
