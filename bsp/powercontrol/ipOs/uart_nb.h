/*--------------------------------------------------------------------------------------------------

                  版权所有 (C), 2015-2020, 北京万维盈创有限公司

 -----------------------------------------------------------------------------------
  文 件 名   : uart_nb.h
  版 本 号   : 初稿
  作    者   : 李烨
  生成日期   : 2016年7月13日
  最近修改   :
  功能描述   : 串行传输使用netbuf控制头文件
  修改历史   :
  1.日    期   : 2016年7月13日
    作    者   : 李烨
    修改内容   : 创建文件
  

----------------------------------------------------------------------------------------------------*/
#ifndef		__UART_NB_H__
#define		__UART_NB_H__

#include "config.h"
#include <rtdevice.h>

int _serial_int_rx_nb(rt_device_t device, struct netbuf *nb, int length);
int _serial_int_tx_nb(rt_device_t device, struct netbuf *nb, int length);
int _serial_poll_tx_nb(rt_device_t device, struct netbuf *nb, int length);
typedef struct 
{
	rt_device_t device;               //串口的描述符
	struct rt_semaphore sem_uart_recv;//接收信号量
	char * read_buf;									//串口接收缓存区
	char * write_buf;									//发送缓存区
	struct netbuf *read_nb;		
	struct netbuf *write_nb;
}uart_device;

typedef uart_device* uart_device_t;



#endif 

