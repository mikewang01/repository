/**************************************************************************
 *
 * para_ncom.h : header file
 * Version : 1.0
 * Compiling Environment : Keil5
 * Date : 2016.07.13
 * Author : LiYe
 *
 **************************************************************************
 * Confidential and Proprietary unpublished information of WanweiTech
 * Technology Co.Ltd.
 * Copyright 2016, Wanwei Technology Co.Ltd.
 * All rights reserved.
 **************************************************************************
 *
 * File Description : ncom参数设置类的定义。
 * 
 * Modify Information : No modification.
 *
**************************************************************************/

#ifndef __PARA_NCOM_H__
#define __PARA_NCOM_H__

#include "config.h"


#define RT_TIME_DEFAULT 48


struct para_ncom
{
//	u32_t key;				//flash操作标识
	float set_sample_tmp;		//进样器工作温度
	float set_cbox_tmp;		//柱箱工作温度
	float set_checkor_tmp;		//检测器工作温度
	
	float set_sample_tmp_warn;	//进样器保护温度
	float set_cbox_tmp_warn;	//柱箱保护温度
	float set_checkor_tmp_warn;	//检测器保护温度

	float set_n1_press;		//氮气1压力设置xxxPsi
	float set_n2_press;		//氮气2压力设置xxxPsi
	float set_n3_press;		//氮气3压力设置xxxPsi
		
	float set_h1_press;		//氢气1压力设置xxxPsi
	float set_h2_press;		//氢气2压力设置xxxPsi
	
	float set_a1_press;		//空气1压力设置xxxPsi
	float set_a2_press;		//空气2压力设置xxxPsi

	u8_t set_FID1_pole;		//FID1极性选择
	u8_t set_FID2_pole;		//FID2极性选择
	u8_t set_FID1_range;		//FID1量程选择
	u8_t set_FID2_range;		//FID2量程选择
	
	u8_t set_sample_onoff;		//进样器开关指示
	u8_t set_cbox_onoff;		//柱箱开关指示
	u8_t set_checkor_onoff;		//检测器开关指示
	u8_t tsfun[12];
	u8_t tsver[10];
	u8_t hdver[20];
};




extern struct para_ncom para;

//extern void reset_para();
extern const struct config_keyword keywords[];
extern u8_t save_para_value(struct netbuf *nb,const char *key, void *var);
extern u8_t reset_device(struct netbuf *nb,const char *key, void *var);
extern void eic_schedule_reboot(void *app);
extern void device_fv_report_init(void);
extern u8_t out_run_info(struct netbuf *nb,const char *key, void *var);

#endif
