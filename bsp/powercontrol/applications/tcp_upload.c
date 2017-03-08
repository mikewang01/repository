#include <tcp_upload.h>
#include <stdio.h>
#include <rtthread.h>
#define TCP_DEBUGE    1

//静态线程的 线程堆栈
static rt_uint8_t tcp_upload_stack[1024];
//静态线程的 线程控制块
static struct rt_thread tcp_upload_thread;

extern int tcp_up_flag;
extern rt_sem_t sem;
extern char GPS_sendbuf[256];
char sendbuf[256];
extern void TCP_SENDOK(void);
extern void TCP_CMD(void);
int time = 5;

//extern nmea_msg gpsx;
//入口函数
void rt_thread_entry_TCP_Upload(void* parameter)
{
		char GPS_cmd[32];   //上报命令	
		rt_err_t  result;
		int i;
	
		for(i=0;i<=255;i++)
			sendbuf[i] = GPS_sendbuf[i];
	
		while(1)
		{
			/********************************************GPS LBS 上报***************************************************/	
#if 1
			if(tcp_up_flag)
			{

					result = rt_sem_take(sem,1000);   //wait 1s sem
					if(result == RT_EOK)
					{
							rt_kprintf("TCP_UP Take sem success !\r\n");
							for(i=0;i<=255;i++)
								sendbuf[i] = GPS_sendbuf[i];	
							rt_sem_release(sem);							//release sem 
					}
				
					rt_kprintf("GPS upload:\r\n");
					rt_memset(GPS_cmd,0,sizeof(GPS_cmd));
					rt_sprintf(GPS_cmd,"AT+CIPSEND=%d\r\n",rt_strlen(sendbuf));   //GPS_sendbuf为要发送数据的大小
					rt_device_write(GU620_dev,0,GPS_cmd,rt_strlen(GPS_cmd));      //写命令
					TCP_CMD();
					rt_device_write(GU620_dev,0,GPS_sendbuf,rt_strlen(sendbuf));        //发送卫星、经纬度
					rt_thread_delay(RT_TICK_PER_SECOND/5);
					rt_device_write(GU620_dev,0,(rt_uint8_t *)0X1A,1);      //发送结束			
					TCP_SENDOK();
					

					rt_thread_delay(RT_TICK_PER_SECOND * time);
			}
			else
#endif
					rt_thread_delay(RT_TICK_PER_SECOND * 10);
			
		
		}


}

void TCP_Upload(void)
{
	rt_err_t result;
	result = rt_thread_init(&tcp_upload_thread,"tcp_upload_thread",rt_thread_entry_TCP_Upload,RT_NULL,
						(rt_uint8_t*)&tcp_upload_stack[0],sizeof(tcp_upload_stack),9,10);
//	rt_thread_t rt_upload;
//	rt_upload = rt_thread_create("upload",&rt_thread_entry_TCP_Upload,RT_NULL,
//									512,9,10);
	if(result == RT_EOK)
	{
				rt_thread_startup(&tcp_upload_thread);
//				rt_kprintf("Creat TCP_Upload Thread!\r\n");
	}
}
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <rtdevice.h>
void up_time(int num)
{
		time = num;
		rt_kprintf("UP_Time : %d s\r\n",time);
}
FINSH_FUNCTION_EXPORT(up_time, up_time.)

#endif