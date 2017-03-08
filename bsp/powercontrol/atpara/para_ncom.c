/**************************************************************************
 *
 * para_ncom.c : implementation file
 * Version : 1.0
 * Compiling Environment : Ubicom Unity 4.2.2
 * Date : 2003.12.07
 * Author : Yolanda
 *
 **************************************************************************
 * Confidential and Proprietary unpublished information of Warewin
 * Technology Co.Ltd.
 * Copyright 2003, Warewin Technology Co.Ltd.
 * All rights reserved.
 **************************************************************************
 *
 * File Description : 设置参数通讯线类的实现程序，实现参数的设置。
 * 
 * Modify Information : No modification.
 *
**************************************************************************/

#include "config.h"
#include "paralib.h"
#include "finsh.h"
#include "para_ncom.h"	



#define PARA_DEFAULTS { \
	100.0, 						/* 进样器工作温度 */ \
	100.0,					  /*柱箱工作温度 */\
	120.0,					  /*检测器工作温度*/\
	120.0, 						/* 进样器保护温度 */ \
	120.0, 						/*柱箱保护温度 */\
	150.0,					  /*检测器保护温度*/\
	6.0,							/* 氮气1压力设置xxxPsi */ \
	8.0, 							/* 氮气2压力设置xxxPsi*/\
	10.0, 						/* 氮气3压力设置xxxPsi*/\
	15.0,							/* 氢气1压力设置xxxPsi */ \
	15.0, 						/* 氢气2压力设置xxxPsi*/\
	12.0, 						/* 空气1压力设置xxxPsi*/\
	12.0, 						/* 空气2压力设置xxxPsi*/\
	1, 								/* FID1极性选择*/\
	0, 								/* FID2极性选择*/\
	1, 								/* FID1量程选择*/\
	2, 								/* FID2量程选择*/\
	0, 								/* 进样器开关指示*/\
	0, 								/* 柱箱开关指示*/\
	0, 								/* 检测器开关指示*/\
	{'E','M','4','0','0','0','_','Q','X','S','P',0},\
	{'0','1','_','0','7','1','6',0,0,0},\
	{'M','A','I','N','_','V','0','3','_','2','0','1','6','.','0','4','.','2','0',0},\
}                          


//const	u8_t tsfun[12]  ={'E','M','4','0','0','0','-','Q','X','S','P',0};
//const	u8_t tsver[10]  ={'0','1','-','0','7','1','6',0,0,0};


struct para_ncom para = PARA_DEFAULTS;


const struct config_keyword keywords[] = {
	/* keyword      getpara          setpara          variable address  */
	{"SAMWKTMPR",   get_float_value, set_float_value, &(para.set_sample_tmp)},
	{"BOXWKTMPR",   get_float_value, set_float_value, &(para.set_cbox_tmp)},
	{"CHKWKTMPR",   get_float_value, set_float_value, &(para.set_checkor_tmp)},
	
	{"SAMWNTMPR",   get_float_value, set_float_value, &(para.set_sample_tmp_warn)},
	{"BOXWNTTMPR",   get_float_value, set_float_value, &(para.set_cbox_tmp_warn)},
	{"CHKWNTTMPR",   get_float_value, set_float_value, &(para.set_checkor_tmp_warn)},
//
	{"N1RPESSURE",   get_float_value, set_float_value, &(para.set_n1_press)},
	{"N2RPESSURE",   get_float_value, set_float_value, &(para.set_n2_press)},
	{"N3RPESSURE",   get_float_value, set_float_value, &(para.set_n3_press)},
	
	{"H1RPESSURE",   get_float_value, set_float_value, &(para.set_h1_press)},
	{"H2RPESSURE",   get_float_value, set_float_value, &(para.set_h2_press)},
	{"A1RPESSURE",   get_float_value, set_float_value, &(para.set_a1_press)},
	{"A2RPESSURE",   get_float_value, set_float_value, &(para.set_a2_press)},
	
	{"FID1POLE",  	 get_u8_value, set_u8_value, &(para.set_FID1_pole)},
	{"FID2POLE",  	 get_u8_value, set_u8_value, &(para.set_FID2_pole)},
	{"FID1RANGE",    get_u8_value, set_u8_value, &(para.set_FID1_range)},
	{"FID2RANGE",    get_u8_value, set_u8_value, &(para.set_FID2_range)},
	
	{"SAMONOFF",   	 get_u8_value, set_u8_value, &(para.set_sample_onoff)},
	{"BOXONOFF",   	 get_u8_value, set_u8_value, &(para.set_cbox_onoff)},
	{"CHKONOFF",   	 get_u8_value, set_u8_value, &(para.set_checkor_onoff)},
	
	
	{"TSQ",   			 get_para_value, 0,0 },
	{"&W",   				 save_para_value,0,0, },
	{"&R",   				 reset_device,0,0, },
	{"&FINSH",       out_run_info,0,0, },
//	{"&D",   set_all_para_default,0,0 },
	{0,0,0,0 }
};
/**********************************ly********************************/
u8_t out_run_info(struct netbuf *nb,const char *key, void *var)
{
	strtonb(nb,"\r\nOK\r\n");
	finsh_uart_enable();
	return 1;	
}
/**********************************ly********************************/


u8_t save_para_value(struct netbuf *nb,const char *key, void *var){
	strtonb(nb,"\r\nOK\r\n");
//	flash_write(NULL,(uint32_t *)&para,sizeof(struct para_ncom));
	eic_schedule_reboot(0);
	return 1;	
}

u8_t reset_device(struct netbuf *nb,const char *key, void *var){
	strtonb(nb,"\r\nOK\r\n");
//	force_wcount_near_reset();
//	exdog_reset();
	return 1;	
}
//static struct oneshot reboot_timer;
//static bool_t reboot_timer_inited = FALSE;

//2006-3-29
void eic_schedule_reboot(void *app)
{
//	wdog();
//	exdog_reset();
}


static void cllct_fun_version_report(struct netbuf *nb)
{
	netbuf_set_pos_to_end(nb);
	if(netbuf_fwd_make_space(nb,33)){
#if !defined (USE_DEBUG_MODE)			
		netbuf_fwd_write_mem(nb,"+TSFUN=",7);	
#else
		netbuf_fwd_write_mem(nb,"+DSFUN=",7);	
		
#endif		//	
		netbuf_fwd_write_mem(nb,(void *)para.tsfun,strlen((char *)para.tsfun));
		
#if !defined (USE_DEBUG_MODE)			
		netbuf_fwd_write_mem(nb,";+TSVER=",8);	
#else
		netbuf_fwd_write_mem(nb,";+DSVER=",8);	
#endif			
		netbuf_fwd_write_mem(nb,(void *)para.tsver,strlen((char *)para.tsver));
		netbuf_fwd_write_u8(nb,';');
		netbuf_set_end_to_pos(nb);
	}
}

void device_fv_report_init(void)
{
	report_fv_cb = cllct_fun_version_report;
}



