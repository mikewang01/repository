/**************************************************************************
 *
 * config.h : header file
 * Version : 1.0
 * Compiling Environment : keil5
 * Date : 2016.07.13
 * Author : Liye
 *
 **************************************************************************
 * Confidential and Proprietary unpublished information of Wanweitech
 * Technology Co.Ltd.
 * All rights reserved.
 **************************************************************************
 *
 * File Description : 配置头文件。
 * 
 * Modify Information : No modification.
 *
**************************************************************************/

#ifndef __IPOS_CONFIG_H__
#define __IPOS_CONFIG_H__

//#include "stm32f4xx.h"
//#include "rtconfig.h"
#include "type_def.h"
#include <string.h>
#include "netbuf.h"

#define RT_THREAD_PRIORITY_MIN 	    RT_THREAD_PRIORITY_MAX/4

#define THREAD_RPIORITY_WDOG				RT_THREAD_PRIORITY_MIN			//看门狗线程
#define THREAD_RPIORITY_TMPR_CTRL		RT_THREAD_PRIORITY_MIN+1		//温度控制线程
#define THREAD_RPIORITY_TMPR_CLLCT	RT_THREAD_PRIORITY_MIN+2		//温度采集线程
#define THREAD_RPIORITY_PARA				RT_THREAD_PRIORITY_MIN+3		//参数设置线程
#define THREAD_RPIORITY_EPC					RT_THREAD_PRIORITY_MIN+4		//EPC线程
#define THREAD_RPIORITY_FID					RT_THREAD_PRIORITY_MIN+4		//FID控制线程
#define THREAD_RPIORITY_LCD_CTLUP		RT_THREAD_PRIORITY_MIN+5		//LCD控件更新线程
#define THREAD_RPIORITY_PC					RT_THREAD_PRIORITY_MIN+6		//PC交互线程
#define THREAD_RPIORITY_LCD_CURUP		RT_THREAD_PRIORITY_MIN+6		//LCD曲线更新线程

#endif
