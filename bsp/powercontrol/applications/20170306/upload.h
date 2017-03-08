#ifndef __UPLOAD_H__
#define __UPLOAD_H__

#include <rtthread.h>
#include "gu620.h"
#define GPS_OK   1
#define GPS_ERR  0

extern int gps_onoff;
extern int TCP_reconnet_flag;
//NMEA 0183 协议解析后数据存放结构体
typedef struct  nmea_msg
{										    
 	u8 svnum;					//可见卫星数
//	nmea_slmsg slmsg[12];		//最多12颗卫星
//	nmea_utc_time utc;			//UTC时间
	u32 latitude;				//纬度 分扩大100000倍,实际要除以100000
	u8 nshemi;					//北纬/南纬,N:北纬;S:南纬				  
	u32 longitude;			    //经度 分扩大100000倍,实际要除以100000
	u8 ewhemi;					//东经/西经,E:东经;W:西经
	u8 gpssta;					//GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.				  
 	u8 posslnum;				//用于定位的卫星数,0~12.
 	u8 possl[12];				//用于定位的卫星编号
	u8 fixmode;					//定位类型:1,没有定位;2,2D定位;3,3D定位
	u16 pdop;					//位置精度因子 0~500,对应实际值0~50.0
	u16 hdop;					//水平精度因子 0~500,对应实际值0~50.0
	u16 vdop;					//垂直精度因子 0~500,对应实际值0~50.0 
	float LBS_LON;    //基站定位经度
	float LBS_LAT;    //基站定位纬度
	int altitude;			 	//海拔高度,放大了10倍,实际除以10.单位:0.1m	 
	u16 speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
}rt_nmea_msg_t; 


extern char GPS_sendbuf[256];
typedef struct nmea
{
	float longitude;  //经度
	float latitude;   //纬度

}rt_nmead_t;
struct devinfo
{
	unsigned char busy;
	unsigned char alarm_on_off;	//布防状态 0 off ; 1 on
	unsigned char alarm_type;	//报警类型 1 di; 2 acc
	unsigned int up_time;		//上报的GPS时间
	float longitude;			//上报的GPS经度
	float latitude;				//上报的GPS纬度
	unsigned char sensor_used;
	unsigned int lon_cmp;
	unsigned int lat_cmp;
	float velocity;				//速度
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

	unsigned char di_status;		//当前DI状态
	unsigned char di_status_pre;	//上一次DI状态
	unsigned char gps_count;
	char displace_alarm_flag;

	float rad_acc;
//	struct netbuf *loc_nb;         //这两个数据暂时不清楚
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

