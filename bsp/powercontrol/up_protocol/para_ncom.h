/*******************************************************************************
File name  : para_ncom.h
Description: 
*******************************************************************************/
#ifndef _PARA_NCOM_H_
#define _PARA_NCOM_H_

struct para_ncom
{
	unsigned char state;	//状态
	unsigned char isSMS;

	unsigned char conf_over_time;
	unsigned char com_dirct;

	unsigned char alarm_onoff;	//布防、撤防
//	unsigned char acc_threshold;	//加速度门限

	unsigned char pt;//0:udp;1:tcps;2:tcpc
	unsigned char isdns;

	unsigned char tmode;
	unsigned char heart_beat_scan;	//心跳间隔

	unsigned char ischeck;
	unsigned short ccycle;

	unsigned short send_upkg_span;

	unsigned int ripaddr;
	unsigned short rport;

	unsigned char id[21];	//硬件ID

	unsigned short rt_time;	//重启周期
	char ver_str[8];
};

extern struct para_ncom para;
extern unsigned int param_read(unsigned int nv_offset);
extern void param_write(unsigned int nv_offset, unsigned int reg_offset, unsigned int default_val);

#endif/* _PARA_NCOM_H_ */
