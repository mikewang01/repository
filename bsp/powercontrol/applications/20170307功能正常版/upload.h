#ifndef __UPLOAD_H__
#define __UPLOAD_H__

#include <rtthread.h>
#include "gu620.h"
#define GPS_OK   1
#define GPS_ERR  0

extern int gps_onoff;
extern int TCP_reconnet_flag;
//NMEA 0183 Э����������ݴ�Žṹ��
typedef struct  nmea_msg
{										    
 	u8 svnum;					//�ɼ�������
//	nmea_slmsg slmsg[12];		//���12������
//	nmea_utc_time utc;			//UTCʱ��
	u32 latitude;				//γ�� ������100000��,ʵ��Ҫ����100000
	u8 nshemi;					//��γ/��γ,N:��γ;S:��γ				  
	u32 longitude;			    //���� ������100000��,ʵ��Ҫ����100000
	u8 ewhemi;					//����/����,E:����;W:����
	u8 gpssta;					//GPS״̬:0,δ��λ;1,�ǲ�ֶ�λ;2,��ֶ�λ;6,���ڹ���.				  
 	u8 posslnum;				//���ڶ�λ��������,0~12.
 	u8 possl[12];				//���ڶ�λ�����Ǳ��
	u8 fixmode;					//��λ����:1,û�ж�λ;2,2D��λ;3,3D��λ
	u16 pdop;					//λ�þ������� 0~500,��Ӧʵ��ֵ0~50.0
	u16 hdop;					//ˮƽ�������� 0~500,��Ӧʵ��ֵ0~50.0
	u16 vdop;					//��ֱ�������� 0~500,��Ӧʵ��ֵ0~50.0 
	float LBS_LON;    //��վ��λ����
	float LBS_LAT;    //��վ��λγ��
	int altitude;			 	//���θ߶�,�Ŵ���10��,ʵ�ʳ���10.��λ:0.1m	 
	u16 speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 
}rt_nmea_msg_t; 


extern char GPS_sendbuf[256];
typedef struct nmea
{
	float longitude;  //����
	float latitude;   //γ��

}rt_nmead_t;
struct devinfo
{
	unsigned char busy;
	unsigned char alarm_on_off;	//����״̬ 0 off ; 1 on
	unsigned char alarm_type;	//�������� 1 di; 2 acc
	unsigned int up_time;		//�ϱ���GPSʱ��
	float longitude;			//�ϱ���GPS����
	float latitude;				//�ϱ���GPSγ��
	unsigned char sensor_used;
	unsigned int lon_cmp;
	unsigned int lat_cmp;
	float velocity;				//�ٶ�
	unsigned short lac1;
	unsigned short ci1;
	unsigned short lac2;
	unsigned short ci2;
	unsigned short lac3;
	unsigned short ci3;
	unsigned short lac4;
	unsigned short ci4;
	unsigned short lac5;
	unsigned short ci5;
	char acc_alarm_flag;
	char alarm_confirm_flag;
	char alarm_status_flag;
	char unlocate_flag;
	char is_move;
	char gps_init_ok;

	unsigned char di_status;		//��ǰDI״̬
	unsigned char di_status_pre;	//��һ��DI״̬
	unsigned char gps_count;
	char displace_alarm_flag;

	float rad_acc;
//	struct netbuf *loc_nb;         //������������ʱ�����
//	struct tm tm_gps;
	unsigned int lon_preset;
	unsigned int lat_preset;
	unsigned int lon_overflow;
	unsigned int lat_overflow;
};






void GU620_Upload(void);
extern void TCP_SENDOK(void);
extern void TCP_CMD(void);
#endif

