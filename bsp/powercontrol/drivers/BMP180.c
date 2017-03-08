#include "BMP180.h"
#include "log.h"

#define	BMP180_DEBUG						    	0
#define	BMP180_RELEASE						    1
/* 读命令在地址上加1 写命令在地址上加0  */

/* 命令 */
#define 	BMP180_ADDR		 0xee >> 1

#define 	MEASURE_T    0x2e    
#define		MEASURE_P		 0x34
#define 	RESET				 0x00
/* 寄存器 */
#define   BMP180_REG   0xf4
#define   ST_ADDR			 0xf6

#define 	AC1					 0xAA
#define 	AC2					 0xAC
#define 	AC3					 0xAE
#define 	AC4					 0xB0
#define 	AC5					 0xB2
#define 	AC6					 0xB4
#define 	B1					 0xB6
#define 	B2           0xB8
#define 	MB					 0xBA
#define 	MC					 0xBC
#define 	MD					 0xBE
/******************************************************/
//#if 0
long  result_UT=0;
long  result_UP=0;

short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;

//typedef struct BMP180_DATA 
//{
//		rt_uint8_t ST_Address;
//		rt_uint8_t REC_Data[4];    //用于存放数据
//}BMP180_DATA_t;

////结构体中的每一个变量又是一个结构体
//typedef struct BMP180_PARA
//{
//		BMP180_DATA_t  BMP180_PARA_AC1;
//		BMP180_DATA_t  BMP180_PARA_AC2;
//		BMP180_DATA_t  BMP180_PARA_AC3;
//		BMP180_DATA_t  BMP180_PARA_AC4;
//		BMP180_DATA_t  BMP180_PARA_AC5;
//		BMP180_DATA_t  BMP180_PARA_AC6;
//		BMP180_DATA_t  BMP180_PARA_B1;
//		BMP180_DATA_t  BMP180_PARA_B2;
//		BMP180_DATA_t  BMP180_PARA_MB;
//		BMP180_DATA_t  BMP180_PARA_MC;
//		BMP180_DATA_t  BMP180_PARA_MD;
//		BMP180_DATA_t	 BMP180_TEMPERATURE;
//		BMP180_DATA_t	 BMP180_PREASURE;
//}BMP180_PARA_t ;

//BMP180_PARA_t *BMP180_DEVICE_PARA;     //定义结构体指针
//#endif
/******************************************************/

BMP180_PARA_t *BMP180_DEVICE_PARA;     //定义结构体指针

static int writeBMP180(struct rt_i2c_bus_device * i2c_device ,unsigned char  cmd)
{
		struct rt_i2c_msg msg[1];
		rt_uint8_t   READ_TEMP[2] ={BMP180_REG,MEASURE_T};    //0xf4   0x2e     在0xf4这个寄存器写入0x2e
		rt_uint8_t   READ_PRES[2] ={BMP180_REG,MEASURE_P};    //0xf4   0x34			在0xf4这个寄存器写入0x34
		int err;
	  switch( cmd )
		{
			case RT_DEVICE_CTRL_BMP180_MEASURE_TEMP :  //测量温度
			msg[0].addr		=   BMP180_ADDR;             //芯片地址
			msg[0].flags	=		RT_I2C_WR;							 //写指令
			msg[0].len		= 	2;											 //需要写入两个字节
			msg[0].buf		=		READ_TEMP;							 //BMP180_REG ：0xf4  、  MEASURE_T ：0x2e  
	
			err = rt_i2c_transfer(i2c_device,msg ,1);
			if(err != 1)
				return -1;
			else
				return 0;
				
		
			case RT_DEVICE_CTRL_BMP180_MEASURE_PRE :  //测量压力
				
			msg[0].addr		=   BMP180_ADDR;						//芯片地址
			msg[0].flags	=		RT_I2C_WR;							//写指令
			msg[0].len		= 	2;											//需要写入两个字节
			msg[0].buf		=		READ_PRES;							//BMP180_REG ：0xf4  、  MEASURE_P		 0x34  
	
			err = rt_i2c_transfer(i2c_device,msg ,1);    //正常返回ret   否则返回0
			if(err != 1)
				return -1;
			else
				return 0;		
		}	
			return -1;		
}

static int readBMP180(struct rt_i2c_bus_device * i2c_device ,BMP180_DATA_t * data)
{
		struct rt_i2c_msg msg[2];
		rt_uint8_t buf[2];   //临时定义一个缓冲区存入读以的数据
		rt_uint8_t tem_addr = 0xf6;
		int err;
	
		msg[0].addr		=   BMP180_ADDR;   //设备地址:0xee
		msg[0].flags	=		RT_I2C_WR;		 //写标志
		msg[0].len		= 	1;					   //写入地址的长度，寄存器地址   1个字节    //温度、压力读0xf6、0xf7两个地址 
		msg[0].buf		=		&data->ST_Address;
//	msg[0].buf		=		&tem_addr;

//		LOG(BMP180_RELEASE,("READ TEM ADDR : %x \r\n",msg[0].buf));
	
		msg[1].addr		=   BMP180_ADDR;   //设备地址
		msg[1].flags	=		RT_I2C_RD;     //写标志
		msg[1].len		= 	2;             //读回来数据的长度，应该为两个字节
   	msg[1].buf		=		data->REC_Data;           //读到的数据地址
		
		err = rt_i2c_transfer(i2c_device,msg ,2);    //正常返回ret   否则返回0
	
		
	
//		LOG(BMP180_RELEASE,("read buf[0] = %x ,buf[1] = %x \r\n",buf[0],buf[1]));
		if(err == 0)
			return -1;
		else
			return 0;
}

/* 采集各个参数 */
static int readPARA(BMP180_DATA_t * data)
{
		rt_err_t err;
		BMP180Device_t *BMP180Device;
		err = readBMP180(BMP180Device->i2cBus,data);
		if(err == 0)
			return 0;
		return -1;
}

static int	BMP180_DEVICE_PARA_INIT()
{
		rt_err_t err;
		rt_uint8_t a,b = 11;
//		BMP180_DATA_t * BMP180_DATA_INIT;
		BMP180_DEVICE_PARA = rt_malloc(sizeof(BMP180_PARA_t));      //申请内存，如果不申请内存直接使用的话，就直接进入hard fault
	
		BMP180_DEVICE_PARA->BMP180_PARA_AC1.ST_Address    =    AC1;
		BMP180_DEVICE_PARA->BMP180_PARA_AC2.ST_Address		=		 AC2;
		BMP180_DEVICE_PARA->BMP180_PARA_AC3.ST_Address		=		 AC3;
		BMP180_DEVICE_PARA->BMP180_PARA_AC4.ST_Address		=		 AC4;
		BMP180_DEVICE_PARA->BMP180_PARA_AC5.ST_Address		=    AC5;
		BMP180_DEVICE_PARA->BMP180_PARA_AC6.ST_Address		=    AC6;
		BMP180_DEVICE_PARA->BMP180_PARA_B1.ST_Address			=    B1;
		BMP180_DEVICE_PARA->BMP180_PARA_B2.ST_Address     =    B2;
		BMP180_DEVICE_PARA->BMP180_PARA_MB.ST_Address			=		 MB;
		BMP180_DEVICE_PARA->BMP180_PARA_MC.ST_Address			=    MC;
		BMP180_DEVICE_PARA->BMP180_PARA_MD.ST_Address			=    MD;
		BMP180_DEVICE_PARA->BMP180_TEMPERATURE.ST_Address =    ST_ADDR;  // ST_ADDR  == 0XF6
		BMP180_DEVICE_PARA->BMP180_PREASURE.ST_Address    =    ST_ADDR;  // ST_ADDR  == 0XF6
	
	
	  readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_AC1);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_AC2);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_AC3);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_AC4);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_AC5);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_AC6);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_B1);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_B2);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_MB);
		readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_MC);
		err = readPARA( & BMP180_DEVICE_PARA->BMP180_PARA_MD);
		
		ac1 = ( BMP180_DEVICE_PARA->BMP180_PARA_AC1.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_AC1.REC_Data[1];
		ac2 = ( BMP180_DEVICE_PARA->BMP180_PARA_AC2.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_AC2.REC_Data[1];
		ac3 = ( BMP180_DEVICE_PARA->BMP180_PARA_AC3.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_AC3.REC_Data[1];
		ac4 = ( BMP180_DEVICE_PARA->BMP180_PARA_AC4.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_AC4.REC_Data[1];
		ac5 = ( BMP180_DEVICE_PARA->BMP180_PARA_AC5.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_AC5.REC_Data[1];
		ac6 = ( BMP180_DEVICE_PARA->BMP180_PARA_AC6.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_AC6.REC_Data[1];
		b1  = ( BMP180_DEVICE_PARA->BMP180_PARA_B1.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_B1.REC_Data[1];
		b2  = ( BMP180_DEVICE_PARA->BMP180_PARA_B2.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_B2.REC_Data[1];
		mb  = ( BMP180_DEVICE_PARA->BMP180_PARA_MB.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_MB.REC_Data[1];
		mc  = ( BMP180_DEVICE_PARA->BMP180_PARA_MC.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_MC.REC_Data[1];
		md  = ( BMP180_DEVICE_PARA->BMP180_PARA_MD.REC_Data[0] << 8 ) + BMP180_DEVICE_PARA->BMP180_PARA_MD.REC_Data[1];

		
		
		rt_kprintf("AC1 is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_AC1.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_AC1.REC_Data[1]);
		rt_kprintf("AC2 is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_AC2.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_AC2.REC_Data[1]);
		rt_kprintf("AC3 is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_AC3.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_AC3.REC_Data[1]);
		rt_kprintf("AC4 is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_AC4.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_AC4.REC_Data[1]);
		rt_kprintf("AC5 is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_AC5.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_AC5.REC_Data[1]);
		rt_kprintf("AC6 is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_AC5.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_AC6.REC_Data[1]);
		rt_kprintf("B1  is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_B1.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_B1.REC_Data[1]);
		rt_kprintf("B2  is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_B2.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_B2.REC_Data[1]);
		rt_kprintf("MB  is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_MB.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_MB.REC_Data[1]);
		rt_kprintf("MC  is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_MC.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_MC.REC_Data[1]);
		rt_kprintf("MD  is ===== %x  %x\r\n",BMP180_DEVICE_PARA->BMP180_PARA_MD.REC_Data[0],BMP180_DEVICE_PARA->BMP180_PARA_MD.REC_Data[1]);
		rt_kprintf("\r\n \r\n");
		rt_kprintf("AC1 is ===== %d \r\n",ac1);
		rt_kprintf("AC2 is ===== %d \r\n",ac2);
		rt_kprintf("AC3 is ===== %d \r\n",ac3);
		rt_kprintf("AC4 is ===== %d \r\n",ac4);
		rt_kprintf("AC5 is ===== %d \r\n",ac5);
		rt_kprintf("AC6 is ===== %d \r\n",ac6);
		rt_kprintf("B1  is ===== %d \r\n",b1);
		rt_kprintf("B2  is ===== %d \r\n",b2);
		rt_kprintf("MB  is ===== %d \r\n",mb);
		rt_kprintf("MC  is ===== %d \r\n",mc);
		rt_kprintf("MD  is ===== %d \r\n",md);

		
		rt_kprintf("****************************\r\n");
		
		if(err == 0)
			return 0;
		return -1;

}

static rt_err_t rt_bmp180_control(rt_device_t dev,rt_uint8_t cmd,void *args )
{
			rt_err_t err;
			BMP180Device_t * BMP180Device;
			RT_ASSERT(dev != RT_NULL);
			BMP180Device = (BMP180Device_t *)dev;   //把总线加入进去

			switch(cmd)
			{
				/*******************读取温度**********************/
				case READ_TEMPERATURE:
							err = writeBMP180(BMP180Device->i2cBus,RT_DEVICE_CTRL_BMP180_MEASURE_TEMP);
							if(err != 0)
							{
								LOG(BMP180_RELEASE,("write bmp180 failed \r\n"));
								return RT_ERROR;
							}
							else
							{
									rt_thread_delay(1);
									err = readBMP180(BMP180Device->i2cBus,&BMP180_DEVICE_PARA->BMP180_TEMPERATURE);
									result_UT = (BMP180_DEVICE_PARA->BMP180_TEMPERATURE.REC_Data[0] << 8) + BMP180_DEVICE_PARA->BMP180_TEMPERATURE.REC_Data[1];
//									rt_kprintf("TEM is ===== %d \r\n",result_UT);
//									LOG(BMP180_RELEASE,("**************************** \r\n"));
								  if(err != 0)
									{
										LOG(BMP180_RELEASE,("read bmp180 failed \r\n"));
										LOG(BMP180_RELEASE,("**************************** \r\n"));
										return RT_ERROR;
									}
									else 	
										args = &BMP180_DEVICE_PARA->BMP180_TEMPERATURE.REC_Data;
									return RT_EOK;
						  }
				/*******************读取压力**********************/	
				case READ_PRESSURE:
							err = writeBMP180(BMP180Device->i2cBus,RT_DEVICE_CTRL_BMP180_MEASURE_PRE);
							if(err != 0)
							{
								return RT_ERROR;
							}
							else
							 {
								  rt_thread_delay(1);
									err = readBMP180(BMP180Device->i2cBus,&BMP180_DEVICE_PARA->BMP180_PREASURE);
									result_UP = (BMP180_DEVICE_PARA->BMP180_PREASURE.REC_Data[0] << 8) + BMP180_DEVICE_PARA->BMP180_PREASURE.REC_Data[1];
//									rt_kprintf("PRE is ===== %d \r\n",result_UP);
//									LOG(BMP180_RELEASE,("**************************** \r\n"));
									if(err != 0)
									{
										return RT_ERROR;
									}
									return RT_EOK;
								}
				/*******************读取参数**********************/
				case READ_PARA:
							BMP180_DEVICE_PARA_INIT();
							if(err != 0)
							{
								return RT_ERROR;
							}
							return RT_EOK;
			}
	
	
	
		return RT_EOK;
}

void BMP180Init(const char *name,const char *bus)
{
		BMP180Device_t * device;  //定义一个结构体指针，包含IIC总线
		device = rt_malloc(sizeof(BMP180Device_t));
		if(device == RT_NULL)
			return;
		rt_memset(device,0,sizeof(BMP180Device_t));
		device->device.type 	=		RT_Device_Class_Char;
		
		device->device.init		=		RT_NULL;
		device->device.open		=		RT_NULL;
		device->device.close	=		RT_NULL;
		device->device.read		=		RT_NULL;
		device->device.write	=		RT_NULL;
		device->device.control =	rt_bmp180_control;
		
		device->device.user_data	=	RT_NULL;
		device->i2cBus						=	rt_i2c_bus_device_find(bus);
		if(device->i2cBus == RT_NULL)
		{
				rt_kprintf("not find i2c device");
				return;
		}
		rt_device_register(&device->device,name,RT_DEVICE_FLAG_RDWR);
}