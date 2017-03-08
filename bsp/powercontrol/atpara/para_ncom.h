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
 * File Description : ncom����������Ķ��塣
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
//	u32_t key;				//flash������ʶ
	float set_sample_tmp;		//�����������¶�
	float set_cbox_tmp;		//���乤���¶�
	float set_checkor_tmp;		//����������¶�
	
	float set_sample_tmp_warn;	//�����������¶�
	float set_cbox_tmp_warn;	//���䱣���¶�
	float set_checkor_tmp_warn;	//����������¶�

	float set_n1_press;		//����1ѹ������xxxPsi
	float set_n2_press;		//����2ѹ������xxxPsi
	float set_n3_press;		//����3ѹ������xxxPsi
		
	float set_h1_press;		//����1ѹ������xxxPsi
	float set_h2_press;		//����2ѹ������xxxPsi
	
	float set_a1_press;		//����1ѹ������xxxPsi
	float set_a2_press;		//����2ѹ������xxxPsi

	u8_t set_FID1_pole;		//FID1����ѡ��
	u8_t set_FID2_pole;		//FID2����ѡ��
	u8_t set_FID1_range;		//FID1����ѡ��
	u8_t set_FID2_range;		//FID2����ѡ��
	
	u8_t set_sample_onoff;		//����������ָʾ
	u8_t set_cbox_onoff;		//���俪��ָʾ
	u8_t set_checkor_onoff;		//���������ָʾ
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
