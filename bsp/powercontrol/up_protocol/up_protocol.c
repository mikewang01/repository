/*******************************************************************************
File name  : up_protocol.c
Description: 
*******************************************************************************/
#include "string.h"
//#include "time.h"
#include "stdlib.h"
#include "stdio.h"

//#include "../utils/init.h"
//#include "../utils/crc.h"
//#include "../drivers/uart.h"
//#include "../modbus/mb_def.h"
//#include "../modbus/modbus.h"
#include "up_protocol.h"
#include "upbuff.h"
//#include "../devinfo.h"
#include "para_ncom.h"
//#include "global.h"

struct upbuf nbuff;

char make_msg_body_pkg(struct uart_port *port, unsigned char data_type, struct devinfo *dinfo);
void pkg_head_add(struct uart_port *port);
void pkg_gps_data_add(struct uart_port *port, struct devinfo *dinfo);
static void pkg_base_info_add(struct uart_port *port, struct devinfo *dinfo);
void pkg_radi_val_add(struct uart_port *port,struct devinfo *dinfo);
void pkg_bat_info_add(struct uart_port *port);
void tm_to_str(struct tm *tm_pt, char *s);
static unsigned int get_gps_up_time(struct tm *p_tm);
unsigned char rtc_time_get(struct tm *p_tm);
//功能：数据封包
char make_up_msg_pkg(struct uart_port *port, unsigned char cmd_type, struct devinfo *dinfo)
{
	unsigned short checksum;

	pkg_head_add(port);//添加头部  W H

	if(!make_msg_body_pkg(port, cmd_type, dinfo))  //如果不成功返回0
	{
		return 0;
	}

	checksum = crc_calc(port->tx_buff + 3, port->tx_len - 3);  //计算校验和
	uart_tx_buff_add_16u(port, checksum);		  //校验和
	uart_tx_buff_add_n(port, DATA_TAIL, 2);            //添加包尾
	return 1;
}
//功能：添加数据体
static char make_msg_body_pkg(struct uart_port *port, unsigned char data_type, struct devinfo *dinfo)
{
	unsigned short len;
	unsigned short tmp;
	struct tm rtc_time;

	char tm_str[16] ="20141027110000";//init val for test
	rtc_time_get(&rtc_time);
	tm_to_str(&rtc_time, tm_str);

	uart_tx_buff_add(port, 109);//长度
	if(dinfo->di_status & dinfo->alarm_on_off & 0x0F)
	uart_tx_buff_add(port, 1);
	else
	uart_tx_buff_add(port, 0);//=0定时上报
	uart_tx_buff_add(port, dinfo->alarm_on_off);//布放状态
	uart_tx_buff_add(port, dinfo->di_status);//设备状态
	
	pkg_gps_data_add(port, dinfo);//
	pkg_base_info_add(port, dinfo);
	pkg_radi_val_add(port, dinfo);
	
	uart_tx_buff_add_u16(port, rep_interval/10);
	uart_tx_buff_add(port, alm_interval/10);
	pkg_bat_info_add(port);

	uart_tx_buff_add_u32(port, dinfo->lon_preset);
	uart_tx_buff_add_u32(port, dinfo->lat_preset);
	uart_tx_buff_add_u32(port, dinfo->lon_overflow);
	uart_tx_buff_add_u32(port, dinfo->lat_overflow);

	if (tm_str) {
		len = strlen(tm_str);
		uart_tx_buff_add_n(port, tm_str, len);
	} else {
		return 0;
	}
	uart_tx_buff_add_n(port,para.ver_str,7);

	tmp = mb_dev_holding_reg_rd(MB_REP_ID_H);
	para.id[12] = tmp / 1000 + '0';
	para.id[13] = tmp / 100 % 10 + '0';
	para.id[14] = tmp % 100 / 10 + '0';
	para.id[15] = tmp % 10 + '0';

	tmp = mb_dev_holding_reg_rd(MB_REP_ID_L);
	para.id[16] = tmp / 1000 + '0';
	para.id[17] = tmp / 100 % 10 + '0';
	para.id[18] = tmp % 100 / 10 + '0';
	para.id[19] = tmp % 10 + '0';
	len = 20;
	uart_tx_buff_add_n(port,(char *)para.id, len);
	return 1;
}
//添加包头
static void pkg_head_add(struct uart_port *port)
{
	uart_tx_buff_add(port, 'W');
	uart_tx_buff_add(port, 'H');
}
//添加GPS数据
static void pkg_gps_data_add(struct uart_port *port, struct devinfo *dinfo)
{
	unsigned int up_time;
	FLOAT_UINT32 tmp;

	up_time = get_gps_up_time(&(dinfo->tm_gps));//数据合成一个32位字节时间
	uart_tx_buff_add_u32(port, up_time);
	tmp.float_val = dinfo->longitude;//经纬度
	uart_tx_buff_add_u32(port, tmp.uint32_val);
	tmp.float_val = dinfo->latitude;
	uart_tx_buff_add_u32(port, tmp.uint32_val);
	tmp.float_val = dinfo->velocity;//速度
	uart_tx_buff_add_u32(port, tmp.uint32_val);
	uart_tx_buff_add(port, dinfo->sensor_used);//定位方式
}
//
static void pkg_base_info_add(struct uart_port *port, struct devinfo *dinfo)
{
	int i;

	for (i=0; i<11; i++) {
		uart_tx_buff_add(port, 0);
	}
}

static void pkg_radi_val_add(struct uart_port *port, struct devinfo *dinfo)
{
	FLOAT_UINT32 tmp;

	tmp.float_val = up_limit.float_val;
	uart_tx_buff_add_u32(port, tmp.uint32_val);
	tmp.float_val = dn_limit.float_val;
	uart_tx_buff_add_u32(port, tmp.uint32_val);

	tmp.float_val = dinfo->rad_acc;
       // printf("ss：%f\r\n",dinfo->rad_acc);
	if (dinfo->alarm_on_off & 0x80) 
        {//上报要检测警报状态
		uart_tx_buff_add_u32(port, tmp.uint32_val);
         //       printf("get：%x\r\n",tmp.uint32_val);
	} 
        else
        {
		uart_tx_buff_add_u32(port,0xAC00AC00);
	}
}

extern float battery_vol_get(void);
static void pkg_bat_info_add(struct uart_port *port)
{
	unsigned int vbat;

	vbat = (unsigned int)(battery_vol_get() * 1000);
	uart_tx_buff_add_u16(port, vbat);
}

void tm_to_str(struct tm *tm_pt, char *s)
{
	unsigned int tmp;

	tmp = tm_pt->tm_year + 1970;
	*s++ = tmp / 1000 + 0x30;
	*s++ = tmp / 100 % 10 + 0x30;
	*s++ = tmp % 100 / 10 + 0x30;
	*s++ = tmp % 10 + 0x30;
	tmp = tm_pt->tm_mon + 1;
	*s++ = tmp / 10 + 0x30;
	*s++ = tmp % 10 + 0x30;
	tmp = tm_pt->tm_mday;
	*s++ = tmp / 10 + 0x30;
	*s++ = tmp % 10 + 0x30;
	tmp = tm_pt->tm_hour;
	*s++ = tmp / 10 + 0x30;
	*s++ = tmp % 10 + 0x30;
	tmp = tm_pt->tm_min;
	*s++ = tmp / 10 + 0x30;
	*s++ = tmp % 10 + 0x30;
	tmp = tm_pt->tm_sec;
	*s++ = tmp / 10 + 0x30;
	*s++ = tmp % 10 + 0x30;
}

/************
 up time bit
year  : 6 bit
month : 4 bit
day   : 5 bit
hour  : 5 bit
min   : 6 bit
sec   : 6 bit
************/
unsigned int get_gps_up_time(struct tm *p_tm)
{
	unsigned int ret;

	ret = p_tm->tm_sec + \
		(p_tm->tm_min<<6 & 0x00000FC0) + \
		(p_tm->tm_hour<<12 & 0x0001F000) + \
		(p_tm->tm_mday<<17 & 0x003E0000) + \
		(p_tm->tm_mon<<22 & 0x03C00000) + \
		(p_tm->tm_year<<26 & 0xFC000000);

	return ret;
}
