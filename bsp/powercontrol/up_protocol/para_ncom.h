/*******************************************************************************
File name  : para_ncom.h
Description: 
*******************************************************************************/
#ifndef _PARA_NCOM_H_
#define _PARA_NCOM_H_

struct para_ncom
{
	unsigned char state;	//״̬
	unsigned char isSMS;

	unsigned char conf_over_time;
	unsigned char com_dirct;

	unsigned char alarm_onoff;	//����������
//	unsigned char acc_threshold;	//���ٶ�����

	unsigned char pt;//0:udp;1:tcps;2:tcpc
	unsigned char isdns;

	unsigned char tmode;
	unsigned char heart_beat_scan;	//�������

	unsigned char ischeck;
	unsigned short ccycle;

	unsigned short send_upkg_span;

	unsigned int ripaddr;
	unsigned short rport;

	unsigned char id[21];	//Ӳ��ID

	unsigned short rt_time;	//��������
	char ver_str[8];
};

extern struct para_ncom para;
extern unsigned int param_read(unsigned int nv_offset);
extern void param_write(unsigned int nv_offset, unsigned int reg_offset, unsigned int default_val);

#endif/* _PARA_NCOM_H_ */
