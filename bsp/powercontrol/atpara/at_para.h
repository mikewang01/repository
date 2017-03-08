/*--------------------------------------------------------------------------------------------------

                  版权所有 (C), 2015-2020, 北京万维盈创有限公司

 -----------------------------------------------------------------------------------
  文 件 名   : at_para.h
  版 本 号   : 初稿
  作    者   : 李烨
  生成日期   : 2016年7月13日
  最近修改   :
  功能描述   : at参数控制头文件
  修改历史   :
  1.日    期   : 2016年7月13日
    作    者   : 李烨
    修改内容   : 创建文件
  

----------------------------------------------------------------------------------------------------*/
#ifndef		__AT_PARA_H__
#define		__AT_PARA_H__

#include "config.h"
#include "usart.h"

#define PARA_UART_BUFFER_SIZE		1024
#define SEM_NAME_PARA_UART		"sem_uart_para"

#define PARA_DEBUG(d,m)		do {if(d) rt_kprintf m ;} while(0)//调试信息

#define PARA_UART_DEVICE		"uart1"

void start_at_para_process(void);



#endif 


