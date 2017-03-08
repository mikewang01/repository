/**************************************************************************
 *
 * paralib.h : header file
 * Version : 1.0
 * Compiling Environment : IAR 4.21
 * Date : 2010.11.18
 * Author : Fuqiulian	 LiYe
 *
 **************************************************************************
 * Confidential and Proprietary unpublished information of warewin
 * Information Technology Co.Ltd.
 * Copyright 2003, warewin Information Technology Co.Ltd.
 * All rights reserved.
 **************************************************************************
 *
 * File Description : 数组处理AT参数机制需要的函数库		
 			
**************************************************************************/



struct config_keyword{
	char *keyword;	//AT关键字
	u8_t (*getpara)(struct netbuf *line,const char *key, void *var);	//读取参数函数
	u8_t (*setpara)(u8_t *line, void *var);//配置参数函数
	void *var;	//参数存放位置
//	char *def;
};
//extern struct config_keyword keywords[];//fuqiulian20120219
//extern struct para_ncom para;
typedef void (*report_fun_ver_callback)(struct netbuf *nb);
extern report_fun_ver_callback report_fv_cb;
#if defined (SAVE_ORG_PARA_ENABLED)
	extern void recover_saved_para_handle(void);	//2012.04.18 add
#endif
extern u8_t get_float_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_u32_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_u16_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_u8_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_dns_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_mac_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_str_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_hbstr_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_rtc_value(struct netbuf *nb,const char *key, void *var);
u8_t get_resolution_value(struct netbuf *nb,const char *key, void *var);
u8_t get_mutype_value(struct netbuf *nb,const char *key, void *var);
u8_t get_gps_value(struct netbuf *nb,const char *key, void *var);
u8_t get_poll_mode_value(struct netbuf *nb,const char *key, void *var);

extern u8_t set_baud_value( u8_t *line, void *var);
extern u8_t set_u8_value(u8_t *line, void *var);
extern u8_t set_u16_value(u8_t *line, void *var);
extern u8_t set_u32_value( u8_t *line, void *var);
extern u8_t set_float_value( u8_t *line, void *var);
extern u8_t set_dns_value( u8_t *line, void *var);
extern u8_t set_atoi_value( u8_t *line, void *var);
extern u8_t set_mac_value( u8_t *line, void *var);
extern u8_t set_str_value( u8_t *line, void *var);
extern u8_t set_hbstr_value( u8_t *line, void *var);
extern u8_t set_rtc_value( u8_t *line, void *var);

u8_t set_resolution_value(u8_t *line, void *var);
u8_t set_stime_value(u8_t *line, void *var);
u8_t set_lon_gps_value( u8_t *line, void *var);
u8_t set_lat_gps_value( u8_t *line, void *var);

u8_t set_poll_mode_value( u8_t *line, void *var);

u8_t set_all_para_default(struct netbuf *nb,const char *key, void *var);
extern u8_t get_para_value(struct netbuf *nb,const char *key, void *var);
extern u8_t get_indef_para_value(struct netbuf *nb,const char *key, void *var);

extern void config_read(struct netbuf *nb,struct netbuf *rnb);
u8_t strtonb(struct netbuf *nb, void *str);
u8_t memtonb(struct netbuf *nb, u8_t *str,u16_t len);
u8_t paratonb(struct netbuf *nb,const char *key, void *var);
void send_error(struct netbuf *nb);
void send_ok(struct netbuf *nb);
u8_t set_com_value(u8_t *line, void *var);
u8_t set_mutype_value(u8_t *line, void *var);



