#include "myled.h"
#include <drivers/pin.h>
#include <rtthread.h>
#include "led.h"

/*指向信号量的指针*/
static rt_sem_t sem = RT_NULL;
void rt_thread_entry_LED_RUN(void* parameter)
{
		while(1)
		{
//				rt_err_t  result;
//				result =rt_sem_take(sem,110);
//			  if(result == RT_EOK)
//				{
						rt_pin_write(pLED,PIN_LOW);
//						rt_hw_led_on(0);
		//				rt_kprintf("RUN on \r\n");
						rt_thread_delay(RT_TICK_PER_SECOND);
						rt_pin_write(pLED,PIN_HIGH);
//						rt_hw_led_off(0);
		//				rt_kprintf("RUN off \r\n");
						rt_thread_delay(RT_TICK_PER_SECOND);
//						rt_sem_release(sem);
//				}
		}

}

void rt_thread_entry_LED2(void* parameter)
{
		rt_err_t  result;
		while(1)
		{
				result = rt_sem_take(sem, 210);
				if(result == -RT_ETIMEOUT)
				{
//						rt_kprintf("****************************\r\n");
//						rt_kprintf("TAKE SEM TIMEOUT \r\n");
//						rt_kprintf("****************************\r\n");
						rt_pin_write(pLED2,PIN_LOW);
				}
				if(result == RT_EOK)
				{
//						rt_kprintf("first sem value is %d \r\n" , sem->value);
//						rt_kprintf("****************************\r\n");
						rt_pin_write(pLED2,PIN_LOW);
		//			rt_kprintf("LED2 turn on \r\n");
						rt_thread_delay(RT_TICK_PER_SECOND/2);
						rt_pin_write(pLED2,PIN_HIGH);
						rt_thread_delay(RT_TICK_PER_SECOND/2);
						rt_sem_release(sem);
//						rt_kprintf("****************************\r\n");
//						rt_kprintf("second sem value is %d \r\n" , sem->value);
//						rt_kprintf("****************************\r\n");
				}
		
		
		}

}

void rt_thread_entry_PowCtr(void* parameter)
{
			rt_uint32_t  num = 0 ;
			while(1)
			{
					rt_pin_write(pPowCtr,PIN_LOW);
					rt_thread_delay(RT_TICK_PER_SECOND*5);
				  num ++;
					if(num == 0);
					else
					{
//						rt_kprintf("PowCtr turn off \r\n");
//						rt_kprintf("****************************\r\n");
				  }
					rt_pin_write(pPowCtr,PIN_HIGH);
//					rt_sem_release(sem);
//					rt_kprintf("PowCtr turn on \r\n");
//					rt_kprintf("****************************\r\n");
//					rt_kprintf("Reboot num : %d \r\n",num);
//				  rt_kprintf("****************************\r\n");
					rt_thread_delay(RT_TICK_PER_SECOND*60);
					
					
			
			}

}

void rt_thread_entry_Fdog(void* parameter)
{
			while(1)
			{
					rt_pin_write(pFdog,PIN_LOW);
//					rt_kprintf("Fdog running \r\n");
					rt_thread_delay(RT_TICK_PER_SECOND);
					rt_pin_write(pFdog,PIN_HIGH);
					rt_thread_delay(RT_TICK_PER_SECOND);
			
			}

}



void LED_Init(void)
{
		stm32_hw_pin_init();
		
		rt_pin_mode(pLED,PIN_MODE_OUTPUT);
		rt_pin_mode(pLED2,PIN_MODE_OUTPUT);
		rt_pin_mode(pPowCtr,PIN_MODE_OUTPUT);
		rt_pin_mode(pFdog,PIN_MODE_OUTPUT);
	
		rt_pin_write(pLED,PIN_HIGH);
	
//		rt_pin_mode(pGU620,PIN_MODE_OUTPUT);    //打开GU620电源
//		rt_pin_write(pGU620,PIN_HIGH);

	
		rt_thread_t   rt_LED_RUN , rt_LED2 , rt_PowCtr , rt_Fdog ;
		/*创建一个信号量，初始值为0*/
		sem = rt_sem_create("sem",1,RT_IPC_FLAG_FIFO);
		
//		rt_LED_RUN = rt_thread_create("LED",
//																	&rt_thread_entry_LED_RUN,RT_NULL,
//																		512,10,10);
//		if(rt_LED_RUN != RT_NULL) 
//			rt_thread_startup(rt_LED_RUN);
		
		rt_LED2 = rt_thread_create("LED2",
																	&rt_thread_entry_LED2,RT_NULL,
																		512,10,10);
		if(rt_LED2 != RT_NULL) 
			rt_thread_startup(rt_LED2);
		
//		rt_PowCtr = rt_thread_create("PowCtr",
//																	&rt_thread_entry_PowCtr,RT_NULL,
//																		512,10,10);
//		if(rt_PowCtr != RT_NULL) 
//			rt_thread_startup(rt_PowCtr);
		
		rt_Fdog = rt_thread_create("Fdog",
																	&rt_thread_entry_Fdog,RT_NULL,
																		512,9,20);
		if(rt_Fdog != RT_NULL) 
			rt_thread_startup(rt_Fdog);
}
