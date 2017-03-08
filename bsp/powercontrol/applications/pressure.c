#include "pressure.h"
#include "BMP180.h"
#include "BMP180_IIC.h"
#include "rtdevice.h"
#include <stdio.h>    //sprintf() 需要的头文件
#include <string.h>   //memset()  需要的头文件
 
#define	SUNRY_DEBUG						    	0
#define	SUNRY_RELEASE						    1

/* 应用层读取BMP180温度 */
static int ReadBMP180_MT()
{
		rt_device_t device;
		int err;
		float value;
		char 	buf[8];
	
		device =	rt_device_find("BMP180");   //查找设备
		if(device == RT_NULL)
		{
				return BMP180_ERR;		
				LOG(SUNRY_RELEASE,("not find BMP180 \n"));
		}
//		else LOG(SUNRY_RELEASE,("find BMP180 \n"));
			
		if(rt_device_control(device, READ_TEMPERATURE, &err) != RT_EOK)
		goto readTempErr;
		else 
			return BMP180_OK;
//		rt_thread_delay(1000/RT_TICK_PER_SECOND * 100);
		
			
		readTempErr:
		rt_device_close(device);
		return BMP180_ERR;
}

static int ReadBMP180_MP()
{
		rt_device_t device;
		int err;
		float value;
		char 	buf[8];
	
		device =	rt_device_find("BMP180");   //查找设备
		if(device == RT_NULL)
		{
				return BMP180_ERR;		
				LOG(SUNRY_RELEASE,("not find BMP180 \n"));
		}
//		else LOG(SUNRY_RELEASE,("find BMP180 \n"));
			
		if(rt_device_control(device, READ_PRESSURE, &err) != RT_EOK)
		goto readTempErr;
		else 
			return BMP180_OK;
//		rt_thread_delay(1000/RT_TICK_PER_SECOND * 100);
		
			
		readTempErr:
		rt_device_close(device);
		return BMP180_ERR;

}

static int ReadBMP180_PARA()
{
    rt_device_t device;
		int err;
		float value;
		char 	buf[8];
	
		device =	rt_device_find("BMP180");   //查找设备
		if(device == RT_NULL)
		{
				return BMP180_ERR;		
				LOG(SUNRY_RELEASE,("not find BMP180 \n"));
		}
//		else LOG(SUNRY_RELEASE,("find BMP180 \n"));
			
		if(rt_device_control(device, READ_PARA, &err) != RT_EOK)
		goto readTempErr;
		else 
			return BMP180_OK;
//		rt_thread_delay(1000/RT_TICK_PER_SECOND * 100);
		
			
		readTempErr:
		rt_device_close(device);
		return BMP180_ERR;

}

void BMP180_Calculate()
{
		unsigned int ut,OSS = 0;
		unsigned long up;
		long x1, x2, b5, b6, x3, b3, p;
		unsigned long b4, b7;
		char buf1[8],buf2[8];
		float TEMPERATURE,PRESSURE;

		ut = result_UT;	   // 读取温度
		up = result_UP;  // 读取压强    return pressure;	
		//*************
		x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
		x2 = ((long) mc << 11) / (x1 + md);
		b5 = x1 + x2;
		TEMPERATURE = ((b5 + 8) >> 4) * 0.1;    //rt_kprintf()  可能无法直接输出 float 类型的数据
//		rt_kprintf("TEMPERATURE : %f ℃ \r\n",TEMPERATURE);
		memset(buf1,0,8 * sizeof(char));
		sprintf(buf1,"%.2f",TEMPERATURE);
		
		rt_kprintf("TEMPERATURE : %s ℃ \r\n",buf1);
//		rt_kprintf("****************************\r\n");
		//*************		
		b6 = b5 - 4000;
														 // Calculate B3
		x1 = (b2 * (b6 * b6)>>12)>>11;
		x2 = (ac2 * b6)>>11;
		x3 = x1 + x2;
		b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;	
														 // Calculate B4
		x1 = (ac3 * b6)>>13;
		x2 = (b1 * ((b6 * b6)>>12))>>16;
		x3 = ((x1 + x2) + 2)>>2;
		b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
		
		b7 = ((unsigned long)(up - b3) * (50000>>OSS));
		if (b7 < 0x80000000)
		p = (b7<<1)/b4;
		else
		p = (b7/b4)<<1;
		
		x1 = (p>>8) * (p>>8);
		x1 = (x1 * 3038)>>16;
		x2 = (-7357 * p)>>16;
		PRESSURE = p+((x1 + x2 + 3791)>>4);
		memset(buf2,0,8 * sizeof(char));
		sprintf(buf2,"%.2f",PRESSURE);
//		rt_kprintf("PRESSURE : %d  Pa\r\n",PRESSURE);
		rt_kprintf("PRESSURE    : %s  Pa\r\n",buf2);
		rt_kprintf("****************************\r\n");


}

void rt_thread_entry_BMP180(void * parameter)
{
		LXI2cDeviceInit();
		BMP180Init("BMP180","i2c");
		
		ReadBMP180_PARA();
		
		while(1)
		{
			
//				ReadBMP180_PARA();
//				rt_thread_delay(100);
			
				if(ReadBMP180_MT() != BMP180_OK)
				LOG(SUNRY_RELEASE,("[%s]==> ReadBMP180 error \n","Pressure"));
				rt_thread_delay(100);

				if(ReadBMP180_MP() != BMP180_OK)
				LOG(SUNRY_RELEASE,("[%s]==> ReadBMP180 error \n","Pressure"));
				rt_thread_delay(100);
				
				BMP180_Calculate();
				rt_thread_delay(100);
				
		}
}
void PressureInit(void)
{
		rt_thread_t  rt_bmp180 ;
		rt_bmp180 = rt_thread_create("BMP180",
																	&rt_thread_entry_BMP180,RT_NULL,512,10,10);
		if(rt_bmp180 != RT_NULL)
			
			rt_thread_startup(rt_bmp180);	
}
