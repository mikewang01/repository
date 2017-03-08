#ifndef __GU620_H__
#define __GU620_H__

#include <rtthread.h>
#include <gpio.h>
#include <drivers/pin.h>

/******************************************************/
#include "stm32f10x.h"  //�ȵ��õ�һ���ٵ��õڶ���
#include "core_cm3.h"
/******************************************************/
#include "upload.h"
#define	LOG(debug,message) do {if(debug) rt_kprintf message;}while(0)
#define DEBUG    1
#define RELEASE  1
#define RESP_BUF_LEN    						256
#define	GU620_ERR						     	(-1)			/*ִ�д���*/
#define	GU620_OK								0				/*ִ��ok*/
#define	RETRY_CONUT								2

typedef struct GU620_IP
{
	char ip[20];													/*IP��Ϣ*/
	unsigned short port;											/*�˿���Ϣ*/
}GU620_IP_t;


extern rt_device_t GU620_dev;
extern int gps_flag;
//extern int TCP_reconnet_flag;
//rt_uint8_t GU620_flag = 1;
extern void GP620_rss(void);
extern void GU620_on(void);
int GU620_Thread(void);
extern int sendAtCmdWaitResp(char *atCmdString, rt_uint16_t delay, char *responseString,char count);
extern int TCP_ReConnet(void);
#endif
