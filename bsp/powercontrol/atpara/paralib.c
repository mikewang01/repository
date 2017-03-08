/**************************************************************************
 *
 * paralib.c : implementation file
 * Version : 1.0
 * Compiling Environment : IAR 4.21
 * Date : 2010.11.18
 * Author : Fuqiulian	LiYe
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

#include "customlib.h"
#include "paralib.h"
#include "para_ncom.h"	//2012.04.19 add
//static u8_t get_para_value(struct netbuf *nb,const char *key, void *var);
//get para--------------------------
report_fun_ver_callback report_fv_cb=NULL;

/* ///////////////////////////////////////////////
strtonb
函数原型：u8_t strtonb(struct netbuf *nb, u8_t *str)
描述：字符串写入netbuf。
参数：
struct netbuf *nb：将要写入的nb
u8_t *str：被写入的字符串
返回值：是否写入成功
算法：制造空间，写入数据
////////////////////////////////////////////////// */
u8_t strtonb(struct netbuf *nb, void *str){
	u16_t len;
	if(nb == NULL) return 0;
	len = strlen((const char*)str);	  
  	netbuf_set_pos_to_end(nb);
	if(netbuf_fwd_make_space(nb,len))
	{		   
		if ( !netbuf_fwd_write_str(nb,(char*)str)) {
			return 0;
		}
		netbuf_set_pos_to_start(nb);
	}
	return 1;
}

/* ///////////////////////////////////////////////
memtonb
函数原型：u8_t memtonb(struct netbuf *nb, u8_t *str,u16_t len
描述：字符串写入netbuf。
参数：
struct netbuf *nb：将要写入的nb
u8_t *str：被写入的字符串
返回值：是否写入成功
算法：制造空间，写入数据
////////////////////////////////////////////////// */
u8_t memtonb(struct netbuf *nb, u8_t *str,u16_t len){
	if(nb == NULL) return 0; 
  	netbuf_set_pos_to_end(nb);
	if(netbuf_fwd_make_space(nb,len))
	{	
	  	netbuf_fwd_write_mem(nb,(char*)str,len);
		netbuf_set_pos_to_start(nb);
	}
	return 1;
}


/* ///////////////////////////////////////////////
paratonb
函数原型：u8_t paratonb(struct netbuf *nb,const char *key, void *var)
描述：一个参数字段写入netbuf。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字
void *var：参数值字符串
返回值：是否写入成功
算法：生成完整字段的字符串，再写入nb
////////////////////////////////////////////////// */
u8_t paratonb(struct netbuf *nb,const char *key, void *var){
	char para_str[60];
	para_str[0]=0;
	strcat(para_str,"+");
	strcat(para_str,key);
	strcat(para_str,":");
	strcat(para_str,var);
	strcat(para_str,";");
	strcat(para_str,"\r\nOK\r\n");
	return strtonb(nb,(u8_t*)para_str);	

}
////////////////////////////////////////////////
/* ///////////////////////////////////////////////
get_float_value
函数原型：u8_t get_float_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为浮点的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：将float型参数值转换成字符串，再写入nb
////////////////////////////////////////////////// */
u8_t get_float_value(struct netbuf *nb,const char *key, void *var){	
	u8_t para_str[16];
	float *srcint;
	para_str[0]=0;
	srcint=var;
	float_to_flt_str(*srcint,&para_str[0]);
	strtonb(nb,para_str);
//	paratonb(nb,key,para_str);
	return 1;	
}
/* ///////////////////////////////////////////////
get_u32_value
函数原型：u8_t get_u32_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为无符号长整型的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：将u32_t型参数值转换成字符串，再写入nb
////////////////////////////////////////////////// */
u8_t get_u32_value(struct netbuf *nb,const char *key, void *var){	
	u8_t para_str[16];
	u32_t *srcint;
	para_str[0]=0;
	srcint=var;
	int_to_str(*srcint,&para_str[0]);
	strtonb(nb,para_str);
//	paratonb(nb,key,para_str);
	return 1;	
}
/* ///////////////////////////////////////////////
get_u16_value
函数原型：u8_t get_u16_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为无符号短整型的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：将u16_t型参数值转换成字符串，再写入nb
////////////////////////////////////////////////// */
u8_t get_u16_value(struct netbuf *nb,const char *key, void *var){	
	u8_t para_str[16];
	u16_t *srcint;
	para_str[0]=0;
	srcint=var;
	int_to_str(*srcint,&para_str[0]);
	strtonb(nb,para_str);
//	paratonb(nb,key,para_str);
	return 1;	
}
/* ///////////////////////////////////////////////
get_u8_value
函数原型：u8_t get_u8_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为无符号字符型的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：将u8_t型参数值转换成字符串，再写入nb
////////////////////////////////////////////////// */
u8_t get_u8_value(struct netbuf *nb,const char *key, void *var){	
	u8_t para_str[16];
	u8_t *srcint;
	para_str[0]=0;
	srcint=var;
	int_to_str(*srcint,&para_str[0]);
	strtonb(nb,para_str);
//	paratonb(nb,key,para_str);
	return 1;	
}
/* ///////////////////////////////////////////////
get_poll_mode_value
函数原型：u8_t get_poll_mode_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为'Y'或'y'的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：是'Y'或'y'在nb中写入'1'，否则写'0'
////////////////////////////////////////////////// */
u8_t get_poll_mode_value(struct netbuf *nb,const char *key, void *var){	
	u8_t para_str[2]={0,0};
	u8_t *srcint;
	para_str[0]=0;
	srcint=var;
	if(*srcint=='Y'||*srcint=='y'){
		para_str[0] = '1';
	}else{
		para_str[0] = '0';
	}
	strtonb(nb,para_str);
//	paratonb(nb,key,para_str);
	return 1;	
}


/* ///////////////////////////////////////////////
get_dns_value
函数原型：u8_t get_dns_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为IP地址的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：将u32_t型参数值转换成IP地址字符串，再写入nb
////////////////////////////////////////////////// */
//u8_t get_dns_value(struct netbuf *nb,const char *key, void *var){	
//	u32_t *ip;
//	u8_t para_str[16];
//	para_str[0]=0;
//	ip=var;
//	dns_ip_to_dotEX(*ip,&para_str[0]);
//	strtonb(nb,para_str);
////	paratonb(nb,key,para_str);
//	return 1;
//}

/* ///////////////////////////////////////////////
get_mac_value
函数原型：u8_t get_mac_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为MAC地址的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：将参数值转换成MAC地址字符串，再写入nb
////////////////////////////////////////////////// */
//u8_t get_mac_value(struct netbuf *nb,const char *key, void *var){
//	u8_t *mac;
//	u8_t para_str[16];
//	para_str[0]=0;
//	mac=(u8_t*)var;
//	mac_to_str(mac,&para_str[0]);	
//	strtonb(nb,para_str);
////	paratonb(nb,key,para_str);
//	return 1;
//}
/* ///////////////////////////////////////////////
get_str_value
函数原型：u8_t get_str_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为字符串的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：字符串写入nb
////////////////////////////////////////////////// */
u8_t get_str_value(struct netbuf *nb,const char *key, void *var){	
	strtonb(nb,var);
	return 1;
}


/* ///////////////////////////////////////////////
get_resolution_value
函数原型：u8_t get_resolution_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为分辨率的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：从1、2、5、10生成0.1、0.2、0.5、1的字符串，写入nb
////////////////////////////////////////////////// */
u8_t get_resolution_value(struct netbuf *nb,const char *key, void *var)
{	
	u8_t para_str[5]={0,0,0,0,0};
	u8_t value;
	value=*(u8_t*)var;	
	if( value==10 ){
		strcat((char*)para_str,"1");
		strtonb(nb,para_str);	
	}
	if( value==1 || value==2 ||value==5 ){
		para_str[0]='0';
		para_str[1]='.';
		para_str[2]='0'+ value ;
		strtonb(nb,para_str);	
	}		
	return 1;	
}
/* ///////////////////////////////////////////////
get_gps_value
函数原型：u8_t get_gps_value(struct netbuf *nb,const char *key, void *var)
描述：取得值为GPS经纬度的参数值。
参数：
struct netbuf *nb：将要写入的nb
const char *key：参数关键字，无用参数
void *var：参数值存放地址
返回值：是否写入成功
算法：生成符合格式的字符串，写入nb
////////////////////////////////////////////////// */
//u8_t get_gps_value(struct netbuf *nb,const char *key, void *var){	
//	u32_t *gps_value = var;
//	u8_t para_str[20];
////	para_str[0]=0;
//	get_gps_protocol_str(*gps_value,&para_str[0]);
//	strtonb(nb,para_str);
//	return 1;
//}


//set para------------------------------
/* ///////////////////////////////////////////////
set_baud_value
函数原型：u8_t set_baud_value( u8_t *line, void *var)
描述：设置波特率。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成整数，能被300整除时对参数变量赋值
////////////////////////////////////////////////// */
u8_t set_baud_value( u8_t *line, void *var){
	u32_t btv;
	u8_t slen;
	slen=strlen((char const*)line);	
	if (getlongvalue(&btv,line,slen))
	{	
		return 0;
	} 
	else 
	{
		if (btv % 300){
			return 0;						
		} 
		else	*(u32_t*)var=btv;
	}		
	return 1;
}
/* ///////////////////////////////////////////////
set_u8_value
函数原型：u8_t set_u8_value( u8_t *line, void *var)
描述：设置类型为无符号字符型的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成整数，对参数变量赋值
////////////////////////////////////////////////// */
u8_t set_u8_value(u8_t *line, void *var){
	u8_t btv;
	u8_t len;
	len=strlen((char const*)line);		
	get_byte_value(&btv,line,len);
	*(u8_t*)var=btv;
	return 1;
}

/* ///////////////////////////////////////////////
set_u16_value
函数原型：u8_t set_u16_value( u8_t *line, void *var)
描述：设置类型为无符号短整型的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成整数，对参数变量赋值
////////////////////////////////////////////////// */
u8_t set_u16_value(u8_t *line, void *var){
	u16_t btv;
	u8_t len;
	len=strlen((char const*)line);		
	get_int_value(&btv,line,len);
	*(u16_t*)var=btv;
	return 1;
}
/* ///////////////////////////////////////////////
set_u32_value
函数原型：u8_t set_u32_value( u8_t *line, void *var)
描述：设置类型为无符号长整型的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成整数，对参数变量赋值
////////////////////////////////////////////////// */
u8_t set_u32_value( u8_t *line, void *var){
	u32_t btv;
	u8_t slen;
	slen=strlen((char const*)line);
	getlongvalue(&btv,line,slen);
//	var=&btv;
	*(u32_t*)var=btv;
	return 1;
}
/* ///////////////////////////////////////////////
set_float_value
函数原型：u8_t set_float_value( u8_t *line, void *var)
描述：设置类型为浮点型的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成浮点数，对参数变量赋值
////////////////////////////////////////////////// */
u8_t set_float_value( u8_t *line, void *var){
	float btv;
	btv = flt_str_to_float(line);
//	var=&btv;
	*(float *)var=btv;
	return 1;
}
/* ///////////////////////////////////////////////
set_dns_value
函数原型：u8_t set_dns_value( u8_t *line, void *var)
描述：设置内容为IP地址的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：IP地址字符串转成无符号长整数，对参数变量赋值
////////////////////////////////////////////////// */
//u8_t set_dns_value( u8_t *line, void *var){
//	u32_t btv;		
//	btv=dns_dot_to_ipEX(line);
//	*(u32_t*)var=btv;
//	return 1;
//}
/* ///////////////////////////////////////////////
set_atoi_value
函数原型：u8_t set_atoi_value( u8_t *line, void *var)
描述：设置类型为无符号短整型的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成整数，对参数变量赋值
////////////////////////////////////////////////// */
//u8_t set_atoi_value( u8_t *line, void *var){	
//	u16_t btv;	
//	btv=atoi((char const*)line);
//	*(u16_t*)var=btv;
//	return	1;
//}

/* ///////////////////////////////////////////////
set_mac_value
函数原型：u8_t set_mac_value( u8_t *line, void *var)
描述：设置内容为MAC地址的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：MAC地址字符串转成内存格式，对参数变量赋值
	"AE34FFB5C687"---->0xAE,0x34,0xFF,0xB5,0xC6,0x87
////////////////////////////////////////////////// */
//u8_t set_mac_value( u8_t *line, void *var){
//	u8_t *btv;
//	btv=(u8_t *)var;	
//	str_to_mac(line,btv);
//	return	1;
//}
/* ///////////////////////////////////////////////
set_str_value
函数原型：u8_t set_str_value( u8_t *line, void *var)
描述：设置类型为字符串的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串赋值
////////////////////////////////////////////////// */
u8_t set_str_value( u8_t *line, void *var){	
	*(u8_t*)var=0;
	strcat(var,(char const*)line);
	return 1;
}

/* ///////////////////////////////////////////////
set_hbstr_value
函数原型：u8_t set_hbstr_value( u8_t *line, void *var)
描述：设置类型为字符串的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串赋值
////////////////////////////////////////////////// */
//u8_t set_hbstr_value( u8_t *line, void *var){	
//  	u16_t i;
//  	u16_t j;
//	u16_t len;
//	u8_t tmp;
//	u8_t tmpv1;
//	u8_t tmpv2;	
//	u8_t* c;
//	c = (u8_t*)var;
//	
//	len = strlen((const char*)line);
//	if(len == 0)
//	{
//	  	c[0]=0;
//	}
//	else
//	{
//	  	j=0;
//		j=len/2;
//		if(j>40)j=40;
//	  	for(i=0;i<j;i++)
//		{
//			tmpv1 = 0;
//			tmpv2 = 0;
//			tmp = line[i*2];
//			if(tmp>='0' && tmp <='9')
//			{
//				tmpv1 = tmp - '0';
//			}
//			else if(tmp>='a' && tmp <='f')
//			{
//			  	tmpv1 = tmp - 'a';
//				tmpv1+=10;
//			}
//			else if(tmp>='A' && tmp <='F')
//			{
//			  	tmpv1 = tmp - 'A';
//				tmpv1+=10;
//			}
//			tmp = line[i*2+1];
//			if(tmp>='0' && tmp <='9')
//			{
//				tmpv2 = tmp - '0';
//			}
//			else if(tmp>='a' && tmp <='f')
//			{
//			  	tmpv2 = tmp - 'a';
//				tmpv2+=10;
//			}
//			else if(tmp>='A' && tmp <='F')
//			{
//			  	tmpv2 = tmp - 'A';
//				tmpv2+=10;
//			}
//			tmpv1<<=4;
//			c[i]=tmpv1+tmpv2;
//		}
//	}
//	return 1;
//}

/* ///////////////////////////////////////////////
set_rtc_value
函数原型：u8_t set_str_value( u8_t *line, void *var)
描述：设置类型为字符串的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：设置RTC：格式为:14/07/14 04:15:16
////////////////////////////////////////////////// */
//u8_t set_rtc_value( u8_t *line, void *var)
//{
// #if defined(RX8025SA_TIMER_ENABLED) || defined(DS3231_TIMER_ENABLED)  
//  	struct time_BCD t;
//	u8_t tmp;
//	u8_t tmp2;	
//	//年
//	tmp = line[2];
//	tmp -= 0x30;
//	if(tmp >0x09) return 0;
//	tmp2 = line[3];
//	tmp2 -= 0x30;
//	if(tmp2 >0x09) return 0;
//	t.t_year = tmp*0x10+tmp2;
//	//分割符
//	tmp = line[4];
//	if(tmp !='/') return 0;
//	//月
//	tmp = line[5];
//	tmp -= 0x30;
//	if(tmp >0x09) return 0;
//	tmp2 = line[6];
//	tmp2 -= 0x30;
//	if(tmp2 >0x09) return 0;
//	t.t_month = tmp*0x10+tmp2;
//	//分割符
//	tmp = line[7];
//	if(tmp !='/') return 0;
//	//日
//	tmp = line[8];
//	tmp -= 0x30;
//	if(tmp >0x09) return 0;
//	tmp2 = line[9];
//	tmp2 -= 0x30;
//	if(tmp2 >0x09) return 0;
//	t.t_day = tmp*0x10+tmp2;
//	//分割符
//	tmp = line[10];
//	if(tmp !=' ') return 0;
//	//时
//	tmp = line[11];
//	tmp -= 0x30;
//	if(tmp >0x09) return 0;
//	tmp2 = line[12];
//	tmp2 -= 0x30;
//	if(tmp2 >0x09) return 0;
//	t.t_hour = tmp*0x10+tmp2;
//	//分割符
//	tmp = line[13];
//	if(tmp !=':') return 0;
//	//分
//	tmp = line[14];
//	tmp -= 0x30;
//	if(tmp >0x09) return 0;
//	tmp2 = line[15];
//	tmp2 -= 0x30;
//	if(tmp2 >0x09) return 0;
//	t.t_min = tmp*0x10+tmp2;
//	//分割符
//	tmp = line[16];
//	if(tmp !=':') return 0;
//	//秒
//	tmp = line[17];
//	tmp -= 0x30;
//	if(tmp >0x09) return 0;
//	tmp2 = line[18];
//	tmp2 -= 0x30;
//	if(tmp2 >0x09) return 0;
//	t.t_sec = tmp*0x10+tmp2;
//	//
//  	set_time(&t);
//#endif
//	return 1;
//}
/* ///////////////////////////////////////////////
send_error
函数原型：void send_error(struct netbuf *nb)
描述：在netbuf中写入ERROR字符串。
参数：
struct netbuf *nb：将要写入的nb
返回值：空
算法：
////////////////////////////////////////////////// */
void send_error(struct netbuf *nb)
{
	u8_t ts[10];
	ts[0]=0;
	strcat((char*)ts,"\r\nERROR\r\n");
	strtonb(nb,ts);
}
/* ///////////////////////////////////////////////
send_ok
函数原型：void send_ok(struct netbuf *nb)
描述：在netbuf中写入OK字符串。
参数：
struct netbuf *nb：将要写入的nb
返回值：空
算法：
////////////////////////////////////////////////// */
void send_ok(struct netbuf *nb)
{
	u8_t ts[10];
	ts[0]=0;
	strcat((char*)ts,"\r\nOK\r\n");
	strtonb(nb,ts);
}

/* ///////////////////////////////////////////////
set_com_value
函数原型：u8_t set_com_value( u8_t *line, void *var)
描述：设置内容为串口格式的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：符合格式时进行字符串赋值
////////////////////////////////////////////////// */
u8_t set_com_value(u8_t *line, void *var){
	u8_t slen;
	slen=strlen((char const*)line);		
	if( slen<3 || !(line[0]=='7'||line[0]=='8') || !(line[1]=='N'||line[1]=='O'||line[1]=='E') || !(line[2]=='1'||line[2]=='2'))
	{
		return 0;	
	}
	*(u8_t*)var=0;	
	line[3]=0 ;
	strcat(var,(char const*)line);			
	return 1 ;
}

/* ///////////////////////////////////////////////
set_resolution_value
函数原型：u8_t set_resolution_value( u8_t *line, void *var)
描述：设置内容为雨量分辨率的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：0.1、0.2、0.5、1的字符串---->1、2、5、10
////////////////////////////////////////////////// */
u8_t set_resolution_value(u8_t *line, void *var)
{
	u8_t btv;
	u8_t len;
	len=strlen((char const*)line);
	if(len==1){
//		get_byte_value(&btv,line,1);
		btv = line[0]-'0';
		if( btv!=1 )
			return 0;
		*(u8_t*)var=10;	
		return 1;
	}
	if(len==3 && line[0]=='0' && line[1]=='.'){
//		line++;
//		line++;
//		get_byte_value(&btv,line,1);
		btv = line[2]-'0';
		if(btv==1 || btv==2 || btv==5 ){
			*(u8_t*)var=btv;
			return 1;
		}	
	}			
	return 0;
}
/* ///////////////////////////////////////////////
set_stime_value
函数原型：u8_t set_stime_value( u8_t *line, void *var)
描述：设置内容为时刻（时：分格式）的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：符合格式时进行字符串赋值
////////////////////////////////////////////////// */
u8_t set_stime_value(u8_t *line, void *var)
{
	u8_t btv;
	u8_t len;
	u8_t *value;
	value=line;
	len=strlen((char const*)line);
	if(len!=5) return 0;
	*(u8_t*)var=0;
	get_byte_value(&btv,line,2);
	if(btv>23 )
		return 0;
	line++;
	line++;
	if(*line!=':')
		return 0;
	line++;
	get_byte_value(&btv,line,2);
	if(btv>59 )
		return 0;
	strcat(var,(char const*)value);	
	return 1;
}
/* ///////////////////////////////////////////////
set_lon_gps_value
函数原型：u8_t set_lon_gps_value( u8_t *line, void *var)
描述：设置内容为经度的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成特定格式的整数，赋值
////////////////////////////////////////////////// */
//u8_t set_lon_gps_value( u8_t *line, void *var){	
//	u32_t btv;	
//	btv=get_gps_protocol_value(line,0);
//	*(u32_t*)var=btv;
//	return 1;
//}
/* ///////////////////////////////////////////////
set_lat_gps_value
函数原型：u8_t set_lat_gps_value( u8_t *line, void *var)
描述：设置内容为纬度的参数值。
参数：
u8_t *line：将要设置的参数值字符串
void *var：参数值存放地址
返回值：是否设置成功
算法：字符串转成特定格式的整数，赋值
////////////////////////////////////////////////// */
//u8_t set_lat_gps_value( u8_t *line, void *var){	
//	u32_t btv;	
//	btv=get_gps_protocol_value(line,1);
//	*(u32_t*)var=btv;
//	return 1;
//}
/* ///////////////////////////////////////////////
set_all_para_default
函数原型：u8_t set_all_para_default(struct netbuf *nb,const char *key, void *var)
描述：恢复所有参数为默认值。
参数：
struct netbuf *nb：写入回应的nb
const char *key：无效参数
void *var：无效参数
返回值：是否设置成功
算法：写恢复参数的flash标识，从启后系统自动从固定位置中恢复参数
////////////////////////////////////////////////// */
u8_t set_all_para_default(struct netbuf *nb,const char *key, void *var)
{
//	u8_t data =1;
//	WriteFlash(AT_FLAG_ADDR,&data,1,0);//写入AT标志
//	if(nb){
//		strtonb(nb,"ALL_PARA_RECOVER_DEFAULT\r\n");
//	}
	return 1;
}


//读取所有参数
/* ///////////////////////////////////////////////
get_para_value
函数原型：u8_t get_para_value(struct netbuf *nb,const char *key, void *var)
描述：生成所有参数字段，写入nb。
参数：
struct netbuf *nb：写入回应的nb
const char *key：无效参数
void *var：无效参数
返回值：是否写入成功
算法：遍历参数表，生成所有字段并写入，加入版本信息
////////////////////////////////////////////////// */
u8_t get_para_value(struct netbuf *nb,const char *key, void *var){
	struct config_keyword *k;
	u16_t i;
	strtonb(nb,"AT");
	for (k = (void *)keywords, i = 0;k->keyword!=0; k++, i++){
//	for (k = keywords, i = 0; i < 22; k++, i++){
		if(k->var==0){
			continue;
		}
		strtonb(nb,"+");
		strtonb(nb,(u8_t*)k->keyword);
		strtonb(nb,"=");
		k->getpara(nb, k->keyword,k->var);
		strtonb(nb,";");
	} 
	if(report_fv_cb){	//写入版本信息
		report_fv_cb(nb);
	}
	strtonb(nb,"\r\nOK\r\n");
	return 1;
}
/* ///////////////////////////////////////////////
get_indef_para_value
函数原型：u8_t get_indef_para_value(struct netbuf *nb,const char *key, void *var)
描述：生成关键字指定部分相同的参数字段，写入nb。
参数：
struct netbuf *nb：写入回应的nb
const char *key：AT关键字
void *var：无效参数
返回值：是否写入成功
算法：遍历参数表，生成关键字指定部分相同的参数字段并写入
////////////////////////////////////////////////// */
u8_t get_indef_para_value(struct netbuf *nb,const char *key, void *var)
{
	struct config_keyword *k;
	u16_t i;
	strtonb(nb,"AT");
	for (k = (void *)keywords, i = 0;k->keyword!=0; k++, i++){
//	for (k = keywords, i = 0; i < 22; k++, i++){
		if(k->var==0){
			continue;
		}
		if(strncmp(k->keyword,key,5)){	//前五个字符不一致，跳过
			continue;
		}
		strtonb(nb,"+");
		strtonb(nb,(u8_t*)k->keyword);
		strtonb(nb,"=");
		k->getpara(nb, k->keyword,k->var);
		strtonb(nb,";");
	} 
	strtonb(nb,"\r\nOK\r\n");
	return 1;
}
////////////////////////////////////////////////////////////////
/* ///////////////////////////////////////////////
find_cmd_tail
函数原型：bool_t find_cmd_tail(struct netbuf *nb)
描述：找到指令尾（最后一个可见字符）。
参数：
struct netbuf *nb：指令来源nb
返回值：是否以0x0D或0x0A结尾
算法：通过2分法找到可见字符与不可见字符的分界，检查其后字符是否为0x0D或0x0A
////////////////////////////////////////////////// */
static bool_t find_cmd_tail(struct netbuf *nb)
{
	u16_t r_start,r_pos,r_end;
	u8_t data=0;//,find_v=0;
	r_start = netbuf_get_start(nb);
	r_end = netbuf_get_end(nb)-1;
	r_pos = (r_start+r_end)/2;
	while(r_pos>r_start){
		netbuf_set_pos(nb,r_pos);
		data=netbuf_fwd_read_u8(nb);
		if(data>=0x20){	//当前可见
			r_start=r_pos;	//可见字符结束位置在后面
		}else{
			r_end=r_pos;
		}
		r_pos = (r_start+r_end)/2;
	}
	///////此时r_pos==r_end
	netbuf_set_pos(nb,r_pos);
	data=netbuf_fwd_read_u8(nb);
	if(data==0x0D||data==0x0A){
		netbuf_set_pos(nb,r_pos);
		netbuf_set_end_to_pos(nb);
		return 1;
	}
	netbuf_set_pos(nb,r_end);
	data=netbuf_fwd_read_u8(nb);
	if(data==0x0D||data==0x0A){
		netbuf_set_pos(nb,r_end);
		netbuf_set_end_to_pos(nb);
		return 1;
	}	//// */	//2014.01.08 remove
	return 0;
}

/* ///////////////////////////////////////////////
config_read
函数原型：void config_read(struct netbuf *nb,struct netbuf *rnb)
描述：参数配置或读取处理。
参数：
struct netbuf *nb：指令来源nb
struct netbuf *rnb：回应nb
返回值：空
算法：判定AT及回车头尾正确后，遍历参数表找到关键字相同的节点，根据后续为？或 =进行查询或配置
////////////////////////////////////////////////// */
void config_read(struct netbuf *nb,struct netbuf *rnb)
{
//	u16_t tail_pos,nb_len;
	u8_t str_head[2];
//截取AT两个字符
	netbuf_set_pos_to_start(nb);
	netbuf_set_pos_to_start(rnb);
	netbuf_fwd_read_mem(nb,str_head,2);
	if(memcmp("AT",str_head,2)!=0){
		send_error(rnb);
		return;
	}
//去掉回车
	netbuf_set_start_to_pos(nb);
	bool_t find_return=0;
	find_return = find_cmd_tail(nb);
//无回车	
	if(!find_return){
		send_error(rnb);
		return;
	}
//如果只有AT回车返回OK		
	if(!netbuf_get_preceding(nb)){	
		send_ok(rnb);
		return;
	}			
//如果只有AT回车返回OK	
/*	if(tail_pos==0)	{
		send_ok(rnb);
		return;
	}	*/	//2012.12.11 remove
//截取每个关键字	
	netbuf_set_pos_to_start(nb);
	u8_t cmd[60];
	u8_t *vcmd,*value;
	u8_t ancmdlen=line_next_f_len(nb,';');	
	u16_t klen,vlen,i;
	struct config_keyword *k;	
	while (ancmdlen>0)
	{
		if(ancmdlen<60)	
		{
			netbuf_fwd_read_mem(nb,cmd,ancmdlen);
			cmd[ancmdlen]=0;
			vcmd=cmd;
			if (cmd[0]=='+')
			{
				vcmd=&cmd[1];
			}			
			vlen=strlen((char const*)vcmd);
			k = (void *)keywords;	
			for (i = 0; k->keyword!=0; i++) 
			{
				klen=strlen(k->keyword);		
				if (memcmp(k->keyword,vcmd,klen)==0) 
				{
					if (klen==vlen)
					{	//AT+TSQ的专门处理
						k->getpara(rnb, k->keyword,k->var);
						break;
					}					
					if (*(vcmd+klen)=='='){
						value=vcmd+klen+1;
						if (k->setpara!=0 && k->setpara(value, k->var)==1){
							strtonb(rnb,"+");
							strtonb(rnb,(u8_t*)k->keyword);
							send_ok(rnb);
						}else {
							send_error(rnb);
						}
						break;	
					}						
					if (*(vcmd+klen)=='?'){
						strtonb(rnb,"AT");	//2012.04.13 add
						strtonb(rnb,"+");
						strtonb(rnb,(u8_t*)k->keyword);
						strtonb(rnb,"=");		//2012.04.13 change : to =
						k->getpara(rnb,k->keyword, k->var);
						send_ok(rnb);
						break;						
					}								
					send_error(rnb);
					break;	
				}
				k++;
			}
		}
		if (netbuf_get_remaining(nb)) 
			netbuf_advance_pos(nb,1);
		ancmdlen=line_next_f_len(nb,';');					
	}	
}

#if defined(SAVE_ORG_PARA_ENABLED)	
/* ///////////////////////////////////////////////
recover_saved_para_handle
函数原型：void recover_saved_para_handle(void)
描述：恢复存储的默认参数。
参数：空
返回值：空
算法：从固定位置读取AT指令字符串，调用config_read配置相关参数
////////////////////////////////////////////////// */
void recover_saved_para_handle(void)
{
	u16_t save_para_len=0;
	u8_t WriteBuf[2];
	WriteBuf[0] = ReadFlash(AT_PARA_LEN_ADDR);	
	save_para_len = WriteBuf[0];	//先获取高字节
	save_para_len <<=8;	
	WriteBuf[1] = ReadFlash(AT_PARA_LEN_ADDR+1);
	save_para_len += WriteBuf[1]  ;	
	if(save_para_len == 0xFFFF){		//没有保存默认参数，保存
		save_org_para_handle(0);
	}
//========连续判断5次读取是否为FF，否则返回===============	
	if(ReadFlash(AT_FLAG_ADDR)==0xFF){
		return;
	}
	if(ReadFlash(AT_FLAG_ADDR)==0xFF){
		return;
	}
	if(ReadFlash(AT_FLAG_ADDR)==0xFF){
		return;
	}
	if(ReadFlash(AT_FLAG_ADDR)==0xFF){
		return;
	}
	if(ReadFlash(AT_FLAG_ADDR)==0xFF){
		return;
	}
	struct netbuf *nb=netbuf_alloc();
	if(!nb) return;
	struct netbuf *nbt=netbuf_alloc();
	if(!nbt) {
		netbuf_free(nb);
		return;
	}
	
	if(!netbuf_fwd_make_space(nb,save_para_len)){
		netbuf_free(nb);
		netbuf_free(nbt);
		return;
	}
	u16_t i=0;
	u8_t data=0;
	while(i<save_para_len){
		if(i%256==0x00){
			wdog();
		}
		data = ReadFlash(AT_PARA_SAVE_ADDR+i);
		netbuf_fwd_write_u8(nb,data);
		i++;
	}
	config_read(nb,nbt);	//用存储的AT指令设置参数
	netbuf_free(nb);
	netbuf_free(nbt);
	flash_erase(NULL,sizeof(struct para_ncom));
	flash_write(NULL,&para,sizeof(struct para_ncom));
	EraseFlash(AT_PARA_LEN_ADDR);		//2012.08.21 change
	WriteFlash(AT_PARA_LEN_ADDR,WriteBuf,2,0);	//写入AT指令长度标志	//2012.08.01 change
}



#endif

