#include "LX_IIC.h"
#include "rtdevice.h"
#include "stm32f10x.h"


/* I2C2 */ 
#define I2C2_GPIO_SCL GPIO_Pin_6
#define I2C2_GPIO_SDA GPIO_Pin_7
#define I2C2_GPIO     GPIOA

/* GPIO */
static void GPIO_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		GPIO_InitStructure.GPIO_Pin = I2C2_GPIO_SCL|I2C2_GPIO_SDA;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(I2C2_GPIO, &GPIO_InitStructure);
		GPIO_SetBits(I2C2_GPIO,I2C2_GPIO_SCL|I2C2_GPIO_SDA);     
}


    
/* rt_hw_i2c_init 设置SDA SCL为高低电平 */

void stm32_set_sda(void *data, rt_int32_t state)
{
    if(state==1)
        GPIO_SetBits(I2C2_GPIO,I2C2_GPIO_SDA); 
    else if(state==0)
        GPIO_ResetBits(I2C2_GPIO,I2C2_GPIO_SDA); 
}

void stm32_set_scl(void *data, rt_int32_t state)
{
    if(state==1)
        GPIO_SetBits(I2C2_GPIO,I2C2_GPIO_SCL); 
    else if(state==0)
        GPIO_ResetBits(I2C2_GPIO,I2C2_GPIO_SCL); 
}

/* 读i2c线状态 */
rt_int32_t stm32_get_sda(void *data)
{
	 return GPIO_ReadInputDataBit(I2C2_GPIO,I2C2_GPIO_SDA);
}
rt_int32_t stm32_get_scl(void *data)
{
    return GPIO_ReadInputDataBit(I2C2_GPIO,I2C2_GPIO_SCL);
}

void stm32_udelay(rt_uint32_t us)
{
    rt_uint32_t delta;
    /* 获得延时经过的tick数 */
    us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));
    /* 获得当前时间 */
    delta = SysTick->VAL;
    /* 循环获得当前时间，直到达到指定的时间后退出循环 */
    while (delta - SysTick->VAL< us);
}


static const struct  rt_i2c_bit_ops stm32_i2c_bit_ops =
{
//  .data    = 
    (void*)0xaa,
//  .set_sda = 
    stm32_set_sda,
//  .set_scl = 
    stm32_set_scl,
//  .get_sda = 
    stm32_get_sda,
//  .get_scl = 
    stm32_get_scl,
//  .udelay = 
    stm32_udelay,
//  .delay_us = 
    20,
//  .timeout = 
    5
};

void tvocI2cDeviceInit(void)
{
    static struct rt_i2c_bus_device stm32_i2c;
    GPIO_Configuration();
    rt_memset((void *)&stm32_i2c, 0, sizeof(struct rt_i2c_bus_device));  //从stm32_i2c 这个地址开始，然后就0进行清除一共sizeof(struct rt_i2c_bus_device)的内存
    stm32_i2c.ops = (void *)&stm32_i2c_bit_ops;
	  stm32_i2c.priv = (void *)&stm32_i2c_bit_ops; //填充private
    rt_i2c_bit_add_bus(&stm32_i2c, "i2c");
    
}