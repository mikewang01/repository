/*******************************************************************************
File name  : para_ncom.c
Description: 
*******************************************************************************/
#include "up_protocol.h"
#include "para_ncom.h"
#include "upbuff.h"

#define RET_OK "\r\nOK\r\n"
extern void flash_read(unsigned char *buff, unsigned int addr, unsigned int count);
extern void mb_dev_holding_reg_wr(unsigned int reg, unsigned int value);
extern unsigned char flash_write(unsigned char *p_buff, unsigned int addr, unsigned int count);
#define PARA_DEFAULTS { \
	204,		/* state */ \
	3,			/* 默认GPRS */ \
	3,			/*确认超时*/	\
	0x03,		/*串口数据方向*/	\
	0x4F,		/*布防撤防默认开启开盖、断电、位移与辐射检测、维护检测*/	\
	0,			/* pt */ \
	0,			/* isdns */ \
	1,			/*default for sms  unused*/ \
	120,		/*默认心跳间隔120秒 */ \
	1,			/* ischeck */ \
	180,		/* ccycle 180秒 */ \
	3,			/*数据上报间隔*/	\
	0x7CCFE27A, /* ripaddr1 */ \
	41890,		/* rport1 */ \
	{'r','s','9','0','0','a','-','2','-','0','0','1','0','0','0','0','0','0','0','0',0}, /* id */ \
	9600,		/*GPS波特率*/\
	{"1511270"},		\
}

struct para_ncom para = PARA_DEFAULTS;

unsigned int param_read(unsigned int nv_offset)
{
	unsigned char buff[4];
	unsigned int tmp=0;
	flash_read(buff, nv_offset, 4);
	tmp = tmp | (*buff) << 8;
	tmp = tmp | *(buff + 1);
	return tmp;
}

void param_write(unsigned int nv_offset, unsigned int reg_offset, unsigned int default_val)
{
	unsigned char buff[4];

	buff[0] = (unsigned char)(default_val >> 8);
	buff[1] = (unsigned char)default_val;
	buff[2] = 0xAA;
	buff[3] = 0xAA;
	mb_dev_holding_reg_wr(reg_offset, default_val);
	flash_write(buff, nv_offset, 4);
}
