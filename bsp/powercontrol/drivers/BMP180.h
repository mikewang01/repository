#ifndef __BMP180_H__
#define __BMP180_H__
#include "rtdevice.h"

typedef struct BMP180Device
{
	struct rt_device device;
	struct rt_i2c_bus_device *i2cBus;

}BMP180Device_t;

/********************************************/
//#if 1
extern long  result_UT;
extern long  result_UP;

extern short ac1;
extern short ac2; 
extern short ac3; 
extern unsigned short ac4;
extern unsigned short ac5;
extern unsigned short ac6;
extern short b1; 
extern short b2;
extern short mb;
extern short mc;
extern short md;

typedef struct BMP180_DATA 
{
		rt_uint8_t ST_Address;
		rt_uint8_t REC_Data[4];    //用于存放数据
}BMP180_DATA_t;

//结构体中的每一个变量又是一个结构体
typedef struct BMP180_PARA
{
		BMP180_DATA_t  BMP180_PARA_AC1;
		BMP180_DATA_t  BMP180_PARA_AC2;
		BMP180_DATA_t  BMP180_PARA_AC3;
		BMP180_DATA_t  BMP180_PARA_AC4;
		BMP180_DATA_t  BMP180_PARA_AC5;
		BMP180_DATA_t  BMP180_PARA_AC6;
		BMP180_DATA_t  BMP180_PARA_B1;
		BMP180_DATA_t  BMP180_PARA_B2;
		BMP180_DATA_t  BMP180_PARA_MB;
		BMP180_DATA_t  BMP180_PARA_MC;
		BMP180_DATA_t  BMP180_PARA_MD;
		BMP180_DATA_t	 BMP180_TEMPERATURE;
		BMP180_DATA_t	 BMP180_PREASURE;
}BMP180_PARA_t ;

extern BMP180_PARA_t *BMP180_DEVICE_PARA;     //定义结构体指针

//#endif
/********************************************/


#define RT_DEVICE_CTRL_BMP180_MEASURE_TEMP			0X04
#define RT_DEVICE_CTRL_BMP180_MEASURE_PRE				0X05
#define RT_DEVICE_CTRL_BMP180_RESET							0X06
#define RT_DEVICE_CTRL_BMP180_GET_DATA  				0X07
#define RT_DEVICE_CTRL_BMP180_WRITE_PARA				0x08
#define RT_DEVICE_CTRL_BMP180_WRITE_REG 				0x09

#define READ_TEMPERATURE												0x0A
#define READ_PRESSURE														0x0B
#define READ_PARA																0x0C
void BMP180Init(const char *name,const char *bus);

#endif


