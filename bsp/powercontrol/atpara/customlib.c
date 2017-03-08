/*--------------------------------------------------------------------------------------------------

                  版权所有 (C), 2015-2020, 北京万维盈创有限公司

 -----------------------------------------------------------------------------------
  文 件 名   : customlib.c
  版 本 号   : 初稿
  作    者   : 李烨
  生成日期   : 2016年7月13日
  最近修改   :
  功能描述   : AT指令参数配置C文件
			负责应用层协议的解析、封包，决定是否发送及发送内容
  修改历史   :
  1.日    期   : 2016年7月13日
    作    者   : 李烨
    修改内容   : 创建文件
    
----------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include "customlib.h"

//#define FORCE_DECI_TWO_NUM 1

void float_data_filter_sort(float *buff, int len)//按从小到大顺序
{
	int i;
	int j;
	float tmp;
	for (i=0; i<len; i++) {
		for (j=0; j<len-1-i; j++) {
			if (*(buff+j) > *(buff+j+1)) {
				tmp = *(buff+j);
				*(buff+j) = *(buff+j+1);
				*(buff+j+1) = tmp;
			}
		}
	}
}

/*****************************************************************************
 函 数 名  : get_filte_float_data
 功能描述  : 获取经缓存排序后的数据
		 新数据进入缓存，拷贝当前缓存至排序数组，排序，取中间值
 输入参数  : float r_value  	最新一次采集获取到的数据
		 float *fifo	原始缓存数组
		 float *sort	排序后的数据
		 u8_t *cur_pos	缓存数组当前写入位置
		 float *des		上报数据
		 int len		 缓存数组长度
 输出参数  : void
 返 回 值  : void
 修改历史      :
  1.日    期   : 2016年6月14日
    作    者   : 李烨
    修改内容   : 新生成函数

*****************************************************************************/
void get_filte_float_data(float r_value,float *fifo,float *sort,u8_t *cur_pos,float *des,int len)
{
	fifo[*cur_pos]=r_value;	//新数据进入缓存
	*cur_pos = (*cur_pos +1)%len;	//写入位置pos置位
	memcpy(sort,fifo,len*4);	//拷贝至排序数组
	float_data_filter_sort(sort,len);		//排序
	*des = sort[len/2];	//取中间值
}


const u8_t ASC[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,'A','B','C','D','E','F'};
const u8_t ASCNB[13] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x23,0x2A};

/* ///////////////////////////////////////////////
get_int_value
原型定义：bool_t get_int_value(u16_t *longvalue, u8_t *strSource, u8_t len);
描述：转换字符串为短整数。
参数：
u16_t *longvalue：转换结果（一个整数指针）
u8_t *strSource,：要转换的字符串
u8_t len：字符串的长度
算法：数字字符串转换成数字代码，循环*10 相加
返回值：转换成功的标志，0：成功；1：失败。
////////////////////////////////////////////////// */
bool_t get_int_value(u16_t *intvalue, u8_t *strSource, u8_t len)
{   
   u8_t ch;   
   u16_t  middlevalue=0;
   u8_t  i=0;   
   u16_t value;

   value=0;
   for (i=0;i<len;i++)
   {
		ch = strSource[i];
		if ((ch<48) || (ch>57))
			return 1;
		middlevalue=(u16_t)(ch-48);
		value=value*10+middlevalue;
   }
   (*intvalue)=value;
   return 0;
}


/* ///////////////////////////////////////////////
getlongvalue
原型定义：bool_t getlongvalue (u16_t *longvalue, u8_t *strSource, u8_t len);
描述：转换字符串为长整数。
参数：
u16_t *longvalue：转换结果（一个长整数指针）
u8_t *strSource,：要转换的字符串
u8_t len：字符串的长度
算法：数字字符串转换成数字代码，循环*10 相加
返回值：转换成功的标志，0：成功；1：失败。
////////////////////////////////////////////////// */
bool_t getlongvalue(u32_t *longvalue, u8_t *strSource, u8_t len)
{   
   u8_t ch;   
   u32_t middlevalue=0;
   u8_t i;   
   u32_t value;

   value=0;
   for (i=0;i<len;i++)
   {
		ch = strSource[i];
		if ((ch<48) || (ch>57))
			return 1;
		middlevalue=(u16_t)(ch-48);
		value=value*10+middlevalue;
   }
   (*longvalue)=value;
   return 0;
}




//计算下一个字段的长度
/* ///////////////////////////////////////////////
line_next_f_len
原型定义：extern u8_t line_next_f_len(struct netbuf *nb);
描述：计算缓存区中下一个字段的长度（字段之间用“,”分割）
参数：
struct netbuf *nb：缓存区
返回值：长度。
算法：循环读取比对，找到','时判定长度
////////////////////////////////////////////////// */
u16_t line_next_f_len(struct netbuf *nb,u8_t separator){
	u8_t ch;
	u16_t count,i,len;
	count =netbuf_get_remaining(nb);
	ch=0;
	for (i=0;i<count;i++){
		ch=netbuf_fwd_read_u8(nb);
		if (ch==separator) break;
	}	
	netbuf_retreat_pos(nb, (u16_t)((ch==separator)?(i+1):i));
	len=(ch==separator)?i:count;
	return len;
		
}
/*///////////////////////////////////////////////
line_jump_n_f
原型定义：extern void line_jump_n_f(struct netbuf *nb,u8_t n);
描述：从缓存区的当前位置向前移动N个字段（字段之间用指定分隔符分割）
参数：
struct netbuf *nb：缓存区
u8_t separator：分隔符
u8_t n：多少个字段
返回值：找到分隔符后pos前移
/////////////////////////////////////////////// */
void  line_jump_n_f(struct netbuf *nb,u8_t separator,u8_t n){
	u8_t i,flen;
	flen=0;
	for( i=0;i<n;i++){
		flen=line_next_f_len(nb,separator);
		if (netbuf_get_remaining(nb)>=(u16_t)flen+1) 
			netbuf_advance_pos(nb, (u16_t)(flen+1)); 
		//jump Latitude 3723.2475 ddmm.mmmm
	}		
}

/*******************************
function:  u16_t general_crc16_modbus(u8_t* in_str,u16_t in_len) 
input:     u8_t* in_str       输入字符串
           u16_t in_len       字符串长度
output:    u16_t     CRC码
note:      计算CRC16-modubs
date:      2013/12/27
********************************/
u16_t general_crc16_modbus(u8_t* in_str,u16_t in_len) 
{
        u16_t   temp_crc;
	u16_t   i;
        u16_t   j;
	
	temp_crc=0xFFFF;
	for(i=0;i<in_len;i++)
	{
		temp_crc ^= in_str[i];
		for(j=0;j<8;j++)
		{
			if(temp_crc& 0x01)
			{ 
				temp_crc=(temp_crc>>1) ^ 0xA001;
			}
			else
			{
				temp_crc=temp_crc>>1;
			}	
		}
	}
	return temp_crc;
}


/*****************************************************************************
 函 数 名  : read_nb_modbus_crc
 功能描述  : 读取数据包的modbus CRC
 输入参数  : struct netbuf *nb：收到的modbus数据包
 输出参数  : void
 返 回 值  : u16_t CRC校验值
 修改历史      :
  1.日    期   : 2016年04月11日
    作    者   : 李烨
    修改内容   : 新生成函数

*****************************************************************************/
u16_t read_nb_modbus_crc(struct netbuf *nb)
{
	u16_t read;
	netbuf_set_pos_to_end(nb);
	netbuf_retreat_pos(nb,2);
	read = netbuf_fwd_read_u8(nb);	//读取低字节
	read |= (u16_t)netbuf_fwd_read_u8(nb)<<8;	//加入高字节
	return read;
}

/*****************************************************************************
 函 数 名  : calc_nb_modbus_crc
 功能描述  : 计算数据包的modbus CRC
 输入参数  : struct netbuf *nb：待计算的数据包
 输出参数  : void
 返 回 值  : u16_t CRC校验值
 修改历史      :
  1.日    期   : 2016年04月11日
    作    者   : 李烨
    修改内容   : 新生成函数

*****************************************************************************/
u16_t calc_nb_modbus_crc(struct netbuf *nb)
{
	u16_t i,j;
	u16_t temp_crc = 0xFFFF;
	u16_t len = netbuf_get_extent(nb)-2;	//要计算校验的数据不包括最后两字节
	
	netbuf_set_pos_to_start(nb);
	
	for(i=0;i<len;i++){
		temp_crc ^= netbuf_fwd_read_u8(nb);
        	for(j=0;j<8;j++){
			if(temp_crc& 0x01){ 
				temp_crc=(temp_crc>>1) ^ 0xA001;
			}
			else{
				temp_crc=temp_crc>>1;
			}	
		}
	}
	return temp_crc;
}

/*****************************************************************************
 函 数 名  : common_modbus_data_valid
 功能描述  : 判断通用modbus指令正确；包括CRC正确，长度满足modbus最低条件
 输入参数  : struct netbuf *nb：收到的modbus数据包
 输出参数  : void
 返 回 值  : void
 修改历史      :
  1.日    期   : 2016年04月11日
    作    者   : 李烨
    修改内容   : 新生成函数

*****************************************************************************/
u8_t common_modbus_data_valid(struct netbuf *nb)
{
	u16_t recv_crc,calc_crc;	//准备计算的校验
	if(netbuf_get_extent(nb)<7) return 0;
	recv_crc = read_nb_modbus_crc(nb);
	calc_crc = calc_nb_modbus_crc(nb);
	if(recv_crc != calc_crc) return 0;
	netbuf_set_pos_to_start(nb);
	return 1;
}
/*///////////////////////////////////////////////
revert_string
原型定义：void revert_string(u8_t *str)
描述：转换字符串顺序
	如："12345"转成"54321"。
参数：
u8_t *str:要转换的字符串
返回值：空
算法：
/////////////////////////////////////////////// */ 
static void revert_string(u8_t *str){	//20101118 add static 
	u8_t strx[20];
	strcpy((char*)strx,(char const*)str);
	u8_t slen=strlen((char const*)strx);
	u8_t i;
	for(i=0;i<slen;i++){
		str[i]=strx[slen-i-1];
	}
} 
/*
int_to_str()
转换整数到字符串
*/  
/*///////////////////////////////////////////////
int_to_str
原型定义：u8_t int_to_str(u32_t srcint,u8_t *des)
描述：转换整数到字符串	
参数：
u32_t srcint：要转换的整数
u8_t *des：转成的字符串
返回值：0转换成功
算法：循环 %10、/10，得到各数位数字，转成字符，合并
/////////////////////////////////////////////// */ 
u8_t int_to_str(u32_t srcint,u8_t *des){
	u8_t j,chx;
	u32_t temp;
	temp=srcint;
	j=0;
	
	/*以下03.11.02增加*/
	if(temp==0){ 
		*(des+j)=ASC[0];
		j++;
	}
	/*以上03.11.02增加*/
	
	while(temp!=0){
		chx=temp % 10;
		temp /= 10;
		*(des+j) = ASC[chx];
		j++;
	};
	*(des+j)=0;
	revert_string(des);
	return 0;	
}

/*///////////////////////////////////////////////
int64_to_str
原型定义：u8_t int64_to_str(u64_t srcint,u8_t *des)
描述：转换整数到字符串	
参数：
u64_t srcint：要转换的整数
u8_t *des：转成的字符串
返回值：0转换成功
算法：循环 %10、/10，得到各数位数字，转成字符，合并
/////////////////////////////////////////////// */ 
u8_t int64_to_str(u64_t srcint,u8_t *des){
	u8_t j,chx;
	u64_t temp;
	temp=srcint;
	j=0;
	
	/*以下03.11.02增加*/
	if(temp==0){ 
		*(des+j)=ASC[0];
		j++;
	}
	/*以上03.11.02增加*/
	
	while(temp!=0){
		chx=temp % 10;
		temp /= 10;
		*(des+j) = ASC[chx];
		j++;
	};
	*(des+j)=0;
	revert_string(des);
	return 0;	
}



/*///////////////////////////////////////////////
int_to_str16
原型定义：u8_t int_to_str16(u32_t srcint,u8_t *s,u8_t len)
描述：转换整数到16进制字符串	
参数：
u32_t srcint：要转换的整数
u8_t *s：转成的字符串
u8_t len：转成的字符串长度
返回值：0转换成功
算法：循环 %16、/16，得到各数位数字，转成字符，合并
/////////////////////////////////////////////// */ 
u8_t int_to_str16(u32_t srcint,u8_t *s,u8_t len){
	u8_t i,chx;
	for (i=0;i<len;i++){
		chx=srcint % 16;
		srcint /= 16;
		s[i] = ASC[chx];
	};
	s[len]=0;
	revert_string(s);
	return 0;
}
/*///////////////////////////////////////////////
str16_to_int
原型定义：u8_t  str16_to_int(u8_t *s,u8_t *srcint,u8_t len)
描述：转换16进制字符串到整数	
参数：
u32_t srcint：要转换的整数
u8_t *s：转成的字符串
u8_t len：字符串长度
返回值：0转换成功
算法：字符转成整数，循环*16相加
/////////////////////////////////////////////// */ 
u8_t str16_to_int(u8_t *s,u8_t *srcint,u8_t len){
   u8_t ch;   
   u8_t  middlevalue=0;
   u8_t  i=0;   

   *srcint=0;
   for (i=0;i<len;i++)
   {
		ch = s[i];
		if (((ch>='0') && (ch<='9'))||((ch>='A') && (ch<='F'))){
			middlevalue=((ch>='0') && (ch<='9'))?(ch-48):(ch-'A'+0x0A);	
			(*srcint)=(*srcint)*16+middlevalue;
		} else 	return 1;
   }
   return 0;
}
/*///////////////////////////////////////////////
s16tr_to_int
原型定义：u8_t  s16tr_to_int(u8_t *s,u8_t *srcint)
描述：转换16进制字符串到整数	
参数：
u32_t srcint：要转换的整数
u8_t *s：转成的字符串
返回值：1转换成功
算法：字符转成整数，循环*16相加
/////////////////////////////////////////////// */ 
bool_t s16tr_to_int(u8_t *src,u16_t *des)
{
	u8_t ch,i=0;
	u8_t  middlevalue=0;
	u16_t value=0;
	u8_t len=strlen((char const*)src);
	for (i=0;i<len;i++){
		ch = src[i];
		if (((ch>='0') && (ch<='9'))||((ch>='A') && (ch<='F'))){
			middlevalue=((ch>='0') && (ch<='9'))?(ch-48):(ch-'A'+0x0A);	
			value=value*16+middlevalue;
		} else 	return 0;
   	}
	*des=value;
	return 1;
}
/*///////////////////////////////////////////////
mac_to_str
原型定义：u8_t mac_to_str(u8_t *mac,u8_t *str)
描述：把内存MAC数组转换成字符串
	如：0x001122334455转换为"001122334455"。
参数：
u8_t *mac：要转换的MAC数组
u8_t *str：转成的字符串
返回值：0转换成功	 
算法：双字节转成16进制字符串，合并
/////////////////////////////////////////////// */ 
u8_t mac_to_str(u8_t *mac,u8_t *str){
	u8_t i;
	for (i=0;i<6;i++){
		int_to_str16(mac[i],str,2);
		str+=2;	
	}
	*(str+1)=0;
	return 0;
}
/*///////////////////////////////////////////////
str_to_mac
原型定义：u8_t str_to_mac(u8_t *str,u8_t *mac)
描述：把MAC字符串转换成内存MAC数组
	如："001122334455"转换为0x001122334455。
参数：
u8_t *str：要转换的字符串
u8_t *mac：转成的MAC数组
返回值：0转换成功	 1转换失败
算法：双字节转成16进制字符串，合并
/////////////////////////////////////////////// */ 
u8_t str_to_mac(u8_t *str,u8_t *mac){
	u8_t i;
	if (strlen((char const*)str)!=12) return 1;
	for (i=0;i<6;i++){
		if (str16_to_int(str,&mac[i],2)) return 1;
		str+=2;	
	}
	return 0;
}
/*///////////////////////////////////////////////
calc_checksum
原型定义：u16_t calc_checksum(struct netbuf *nb,u8_t cstype)
描述：计算数据报文的校验位。
参数：
struct netbuf *nb：数据报的缓存区
u8_t cstype：校验类型，和校验或异或校验
返回值：校验值
算法描述：从报文的第一个字节到最后一个个字节的和或者是异或和。
/////////////////////////////////////////////// */
u16_t calc_checksum(struct netbuf *nb,u8_t cstype){
	u8_t ch;
	u16_t checksum;
	if (!nb) return 0;
	checksum=0;
	netbuf_set_pos_to_start(nb);
	while(netbuf_get_remaining(nb)>0){//2007.10.01
		ch=netbuf_fwd_read_u8(nb);
		if (cstype==ORCHECKSUM){
			checksum^=ch;}
		else {
			checksum+=ch;
		}
	};
	netbuf_set_pos_to_start(nb);
	return checksum;
}



/*///////////////////////////////////////////////
byte_reverse
原型定义：u8_t byte_reverse(u8_t data)
描述：转换数字bit位顺序
	如：00010010b转成01001000b。
参数：
u8_t data:要转换的数据
返回值：转换之后的数据
算法：bit位对调
/////////////////////////////////////////////// */ 
u8_t byte_reverse(u8_t data)
{
	u8_t value=0,i;
	for(i=0;i<8;i++){
		value |= ((data>>i)&1)<<(7-i);
	}
	return value;
}



//2004.01.17
/*///////////////////////////////////////////////
str_to_u64
原型定义：u64_t  str_to_u64(const char *s,u8_t base )
描述：转换字符串到long long 整数	
参数：
const char *s：要转换的字符串
u8_t base：数据进制，10或16
返回值：转成的整数
算法：字符转成整数，循环*base相加
/////////////////////////////////////////////// */ 
u64_t  str_to_u64(const char *s,u8_t base )
{
	long long val = 0;
	u8_t ch;
	//bool_t neg = FALSE;

	while (*s == ' ' || *s == '\t') {
		s++;
	}

	if (*s == '-') {
	//	neg = TRUE;
		s++;
	} else if (*s == '+') {
		s++;
	}

	while ((*s >= '0' && *s <= '9')||(*s>='a' && *s<='f')||(*s>='A' && *s<='F')) {
		val *= base;
		ch=(*s >= '0' && *s <= '9')? '0':((*s>='a' && *s<='f')?'a':'A')-10;
		val += *s++ - ch;
	}
	
	/*if (neg) {
		val = -val;
	}*/
	
	return val;
} 
//2004.01.17
/*///////////////////////////////////////////////
str_to_u32
原型定义：u32_t str_to_u32(u8_t *s,u8_t base)
描述：转换字符串到long 整数	
参数：
const char *s：要转换的字符串
u8_t base：数据进制，10或16
返回值：转成的整数
算法：字符转成整数，循环*base相加
/////////////////////////////////////////////// */ 
u32_t str_to_u32(u8_t *s,u8_t base)
{
	u32_t val=0;
	u8_t ch,len=0;
	while(*s ){		//字符串以0x00结尾，自动跳过非数字或字母 字符
		if(len>19) break;		//字符串最长为20
		if((*s >= '0' && *s <= '9')||(*s>='a' && *s<='f')||(*s>='A' && *s<='F')){
			if(base==10 && *s>'9'){	//十进制转换，夹进字母
				s++;
				len++;
				continue;
			}
			val *= base;
			ch=(*s >= '0' && *s <= '9')? '0':((*s>='a' && *s<='f')?'a':'A')-10;
			val += *s - ch;
		}
		s++;
		len++;
	}
	return val;
}

/*以下是2004.01.15增加的函数*/

/*///////////////////////////////////////////////
asc_to_hex
原型定义：u8_t asc_to_hex(u8_t ch)
描述：转换十六进制字符到整数	
	比如'9'转成9  'A'转成10
参数：
u8_t ch：要转换的字符
返回值：转成的整数
算法：减掉与整数的差值
/////////////////////////////////////////////// */ 
static u8_t asc_to_hex(u8_t ch){	////20101118 add static
	u8_t ret;	
	ret=0;
	if ((ch>= 0x30) && (ch <= 0x39)) //ascii0-9，a-f
		    ret=ch - 0x30;
	if ((ch>= 0x41) && (ch <= 0x46))
		    ret = ch- 0x37;	
	return ret; 
}

/*///////////////////////////////////////////////
hex_to_asc
原型定义：u8_t hex_to_asc(u8_t hex,u8_t* str)
描述：转换十六进制为ASC码
参数：
u8_t hex：要转换的HEX
返回值：HEX转换为2个字符
算法： 
/////////////////////////////////////////////// */ 
void hex_to_asc(u8_t hex,u8_t* str)
{
  	u8_t tmp;
	tmp = hex>>4;
	tmp &=0x0F;
	str[0] = ASC[tmp];
	tmp = hex;
	tmp &=0x0F;
	str[1] = ASC[tmp];
	return; 
}

/*///////////////////////////////////////////////
BCD_to_hex
原型定义：u8_t bcd_to_hex(u8_t bcd)
描述：BCD码 转换十六进制
参数：
u8_t BCD：要转换的bcd
返回值：HEX
算法： 
/////////////////////////////////////////////// */ 
u8_t bcd_to_hex(u8_t bcd)
{
  	u8_t tmp1;
	u8_t tmp2;
	tmp1 = bcd>>4;
	tmp2 = bcd&0x0F;
	tmp1 *= 10;
	tmp1 += tmp2;
	return tmp1; 
}

/*///////////////////////////////////////////////
hex_to_BCD
原型定义：u8_t hex_to_bcd(u8_t hex)
描述：转换十六进制为BCD码
参数：
u8_t hex：要转换的HEX
返回值：BCD
算法： 
/////////////////////////////////////////////// */ 
u8_t hex_to_bcd(u8_t hex)
{
  	u8_t tmp1;
	u8_t tmp2;
	tmp1 = hex/10;
	tmp2 = hex%10;
	tmp1 *= 16;
	tmp1 += tmp2;
	return tmp1; 
}

/*///////////////////////////////////////////////
two_asc_to_hex
原型定义：u8_t two_asc_to_hex(u8_t high_ch,u8_t low_ch)
描述：把两个ASCII转换为一个十六进制，ASCII的范围为1-9，A-F，如"3A"转换为16进制0x3A.
参数：
u8_t high_ch：处于高位的ASCII
u8_t low_ch：处于底位的ASCII
返回值：转换的结果位16进制数。
算法：转成整数移位合并
/////////////////////////////////////////////// */
u8_t two_asc_to_hex(u8_t high_ch,u8_t low_ch){

	return ((asc_to_hex(high_ch))<<4)+asc_to_hex(low_ch);
	
}



/*///////////////////////////////////////////////
mem_to_str
原型定义：bool_t mem_to_str(struct netbuf *nbsrc,struct netbuf *nbdes)
描述：把内存中的数据块转成可见的十六进制字符.
参数：
struct netbuf *nbsrc：待转换的数据
struct netbuf *nbdes：转成的数据
返回值：1转换成功   0转换失败。
算法：从原nb中取数，扩展字节后写入新的nb
/////////////////////////////////////////////// */
bool_t mem_to_str(struct netbuf *nbsrc,struct netbuf *nbdes){
	u16_t srclen;
	u8_t ch;
	if (!nbsrc) return FALSE;
	if (!nbdes) return FALSE;
	
	srclen=netbuf_get_extent(nbsrc);
	if(!netbuf_fwd_make_space(nbdes,(u16_t)srclen*2)) return FALSE;
	while(netbuf_fwd_check_space(nbsrc,1)){
		ch=netbuf_fwd_read_u8(nbsrc);
		netbuf_fwd_write_u8(nbdes,ASC[ch>>4]);
		netbuf_fwd_write_u8(nbdes,ASC[ch & 0x0F]);	
	};
	netbuf_set_pos_to_start(nbdes);
	netbuf_set_pos_to_start(nbsrc);
//	if(!netbuf_fwd_make_space(nbdes,1)) return FALSE;
	return TRUE;
	
}
/*///////////////////////////////////////////////
value_mem_to_str
原型定义：void value_mem_to_str(u16_t value,u8_t *str)
描述：把整数的HEX代码转成可见的十六进制字符.
参数：
u16_t value：待转换的数据
u8_t *str：转成的数据
返回值：空
算法：扩展字节后写入数组
/////////////////////////////////////////////// */
void value_mem_to_str(u16_t value,u8_t *str)
{
	u8_t ch = value>>8;
	str[0] = ASC[ch>>4];
	str[1] = ASC[ch & 0x0F];
	ch = value;
	str[2] = ASC[ch>>4];
	str[3] = ASC[ch & 0x0F];
}


/*///////////////////////////////////////////////
str_to_mem
原型定义：bool_t str_to_mem(struct netbuf *nbsrc,struct netbuf *nbdes)
描述：把十六进制字符转成内存中的HEX格式.
参数：
struct netbuf *nbsrc：待转换的数据
struct netbuf *nbdes：转成的数据
返回值：1转换成功   0转换失败。
算法：从原nb中取数，合并字节后写入新的nb
/////////////////////////////////////////////// */
bool_t str_to_mem(struct netbuf *nbsrc,struct netbuf *nbdes)
{
	//检查容量
	u8_t hc,lc,hexc;
	if (!nbsrc) return FALSE;
	if (!nbdes) return FALSE;
	
	netbuf_set_pos_to_end(nbdes);
	if(!netbuf_fwd_make_space(nbdes,netbuf_get_remaining(nbsrc)/2)){
		return FALSE;
	}
	
	while (netbuf_fwd_check_space(nbsrc,2)){//判断容量
	//读取第一个字符
		hc=netbuf_fwd_read_u8(nbsrc);
	//读取第二个字符
		lc=netbuf_fwd_read_u8(nbsrc);
	//合并为一个HEX
		hexc=two_asc_to_hex(hc,lc);
	//写入nbdes
		netbuf_fwd_write_u8(nbdes,hexc);
		
	}//容量>1
	netbuf_set_pos_to_start(nbdes);
	netbuf_set_pos_to_start(nbsrc);
	//容量为1 0，返回
	return TRUE;
	
}
/*以上是2004.01.15增加的函数*/


/*///////////////////////////////////////////////
nb_strcat_unicode
原型定义：void nb_strcat_unicode(struct netbuf *nb,u8_t *str)
描述：将一个字符串以unicode格式写在nb后面-
参数：
struct netbuf *nbsrc：要写入的nb
u8_t *str：待写字符串
返回值：空
算法：在nb尾制造空间、写入字符串，补足unicode码
/////////////////////////////////////////////// */
void nb_strcat_unicode(struct netbuf *nb,u8_t *str)
{
	u16_t k;
	netbuf_set_pos_to_end(nb);
	if(!netbuf_fwd_make_space(nb, strlen((char const*)str)*2)) return;
	for(k=0;k<strlen((char const*)str);k++)
	{
		netbuf_fwd_write_u8(nb,str[k]);
		netbuf_fwd_write_u8(nb,0);
	}
}




//此函数必须小心使用，否则会造成死机
/*///////////////////////////////////////////////
get_an_field
原型定义：void get_an_field(u8_t *temp,struct netbuf *nb,u16_t separator)
描述：从netbuf中获取一个字段
参数：
u8_t *temp：取出的字段写入的位置
struct netbuf *nb：来源netbuf
u16_t separator：字段分隔符
返回值：空
算法：计算分隔符与当前位置距离，读取此长度的数据写入数组中，nb指针前移
/////////////////////////////////////////////// */ 
void get_an_field(u8_t *temp,struct netbuf *nb,u16_t separator){
	u8_t flen;
//	flen=line_next_f_len(nb,',');
	u16_t remain_len;
	remain_len=netbuf_get_remaining(nb);
	if(separator>0xFF){
		if(remain_len>30){
			remain_len=30;	//安全保护,字段长度不能大于30
		}
	}
	flen=line_next_f_len(nb,separator);
//	if (flen<60 && flen>0){
	if (flen<remain_len && flen>0){
		netbuf_fwd_read_mem(nb,temp,flen);
		temp[flen]=0;
	}else temp[0]=0;
	if (netbuf_get_remaining(nb))
			netbuf_advance_pos(nb, (u16_t)(1));

}

/*///////////////////////////////////////////////
get_an_int_field
原型定义：u32_t get_an_int_field(struct netbuf *nb,u8_t base,u8_t separator)
描述：从netbuf中获取一个整数字段并将其转换成整数
参数：
struct netbuf *nb：来源netbuf
u8_t base：转成的数据进制 10或16
u16_t separator：字段分隔符
返回值：转成的整数
算法：取到整数字符串后将其转换成整数
/////////////////////////////////////////////// */ 
u32_t get_an_int_field(struct netbuf *nb,u8_t base,u8_t separator){
	u8_t temp[30];
	temp[0]=0;
	u16_t sep=separator |0xF000;
	get_an_field(temp,nb,sep);
//	return str_to_u64(temp,base);
	return str_to_u32(temp,base);
}




/*///////////////////////////////////////////////
itoaEX
原型定义：char *itoaEX(char *obuf, u64_t x, unsigned int base, u8_t issigned)
描述：转换整数到字符串 与itoa不同之处是十六进制是大写A-F而不是小写a-f
参数：
char *obuf：写入字符串
u64_t x：要转换的整数
unsigned int base：数据进制，10或16
u8_t issigned：是否有符号
返回值：字符串结束位置
算法：整数转成字符，合并
/////////////////////////////////////////////// */ 
char *itoaEX(char *obuf, u64_t x, unsigned int base, u8_t issigned)
{
	char buf[16];
	char *p = buf + 16;
	unsigned int c;
	int sign = 0;

	if ((base == 10) && ((s64_t)x < 0) && issigned) {	//20101201
		*obuf++ = '-';
		x = -(long)x;
		sign++;
	}

	*--p = '\0';
	do {
		c = (x % base);
		if (c < 10) {
			*--p = '0' + c;
		} else {
			*--p = 'A' + (c - 10);
		}
		x /= base;
	} while (x != 0);

	strcpy(obuf, p);

	return obuf + (addr_t)buf + sizeof(buf) - 1 - (addr_t)p;
}

//把UNICODE unicodes数组转换为一个整数 30 00 30 00 31 00 32 00=>12 2007.01.14
/*///////////////////////////////////////////////
wordsToInt
原型定义：u16_t wordsToInt(u8_t len,u8_t *bytes)
描述：把UNICODE unicodes数组转换为一个整数 34 00 33 00 31 00 32 00=>4312
参数：
u8_t len：数字字符个数
u8_t *bytes：待转换数组
返回值：转成的整数
算法：数字字符转成整数，循环*10相加
/////////////////////////////////////////////// */
u16_t wordsToInt(u8_t len,u8_t *bytes){
	u16_t ret=0;
	u8_t i=0;
	u8_t b;
	if (len<=0)  return 0;
	for (i=0;i< len;i++){
		b =*(bytes+2*i);//-0x30;
		if ((b<48) || (b>57)) return 0;
		//if ((b<0) || (b>9)) return 0;
		ret=ret*10+b-0x30;
		b=*(bytes+2*i+1);
		if (b!=0) return 0;	
	}
	return ret;

}
//把一个整数转换为UNICODE unicodes数组 12=>30 00 30 00 31 00 32 00 2007.01.14
/*///////////////////////////////////////////////
intToWords
原型定义：void intToWords(u16_t value, u8_t len,u8_t *bytes)
描述：把一个整数转换为UNICODE unicodes数组 12=>30 00 30 00 31 00 32 00 
参数：
u16_t value：待转换整数
u8_t len：数字字符个数
u8_t *bytes：写入数组
返回值：空
算法：
/////////////////////////////////////////////// */
void intToWords(u16_t value, u8_t len,u8_t *bytes){
		
	u16_t ret=value;
	u8_t i=0;
	u8_t b;
	if (len<=0)  return;
	for (i=0;i<len;i++){
		b=ret%10;
		ret=ret/10;
		*(bytes+2*(len-i)-2)=b+0x30;
		*(bytes+2*(len-i)-1)=0;
	}

}

/*///////////////////////////////////////////////
get_u32_t_from_bytes
原型定义：u32_t get_u32_t_from_bytes(u8_t *bytes)
描述：u32_t 数据字节序调换
参数：
u8_t *bytes：原数据存放地址
返回值：调换后的整数
算法：移位相加
/////////////////////////////////////////////// */
u32_t get_u32_t_from_bytes(u8_t *bytes){
	u32_t res;
	res=bytes[0];
	res =(res<<8)+bytes[1];
	res =(res<<8)+bytes[2];
	res =(res<<8)+bytes[3];
	return res;
}

/*///////////////////////////////////////////////
set_bytes_from_uint
原型定义：void set_bytes_from_uint(u8_t *bytes,u32_t value,u8_t len)
描述：把一个整数调换字节序放入内存中 
参数：
u8_t *bytes：要放入的位置
u32_t value：要写入数据
u8_t len：数字字节数（2或4）
返回值：空
算法：移位赋值
/////////////////////////////////////////////// */
void set_bytes_from_uint(u8_t *bytes,u32_t value,u8_t len)
{
	while(len--){
		bytes[len]= (u8_t)value;
		value >>=8;
	}
}


/*///////////////////////////////////////////////
mem_calc_checksum
原型定义：mem_calc_checksum(u8_t *bytes,u16_t blen,u8_t cstype)
描述：计算数据报文的校验位。
参数：
u8_t *bytes：数据报缓存
u16_t blen：报文长度
u8_t cstype：校验类型  和校验或异或校验
返回值：校验值
算法描述：从报文的第一个字节到最后一个字节的和或者是异或和。
/////////////////////////////////////////////// */
u8_t mem_calc_checksum(u8_t *bytes,u16_t blen,u8_t cstype){
	u8_t ch=0;
	u8_t checksum=0;
	u16_t i=0;
	for(i=0;i<blen;i++){
		ch=bytes[i];
		if (cstype==ORCHECKSUM){
			checksum^=ch;}
		else {
			checksum+=ch;
		}
	};
	return checksum;
}




//类似环保协议

#define DIO_PRO_DA_CONST ",DA=&"



/*///////////////////////////////////////////////
nb_add_re
原型定义：void nb_add_re(struct netbuf *nbr)
描述：对nb添加回车
参数：
struct netbuf *nbr：需要添加回车的nb
返回值：空
算法描述：制造空间，写入数据
/////////////////////////////////////////////// */
void nb_add_re(struct netbuf *nbr)
{
	netbuf_set_pos_to_end(nbr);
	bool_t make_flag=netbuf_fwd_make_space(nbr,2);
	if(!make_flag){
		netbuf_set_pos_to_start(nbr);
		return;
	}
	netbuf_fwd_write_mem(nbr,"\r\n",2);
	netbuf_set_pos_to_start(nbr);
	return;
}
/*
generate_a_field()  生成“key=value”格式nb，无分号，nb需事先分配
addendchar 是否需要结尾字符‘;’
netbuf 位置放在最后

*/
/*///////////////////////////////////////////////
generate_a_field
原型定义：u8_t generate_a_field(struct netbuf *nbs,u8_t *key_str,u8_t *value_str,u8_t addendchar,u8_t dirhi)
描述：生成“key=value”格式nb，无分号，nb需事先分配
参数：
struct netbuf *nbs：要写入的nb
u8_t *key_str：关键字
u8_t *value_str：数据值
u8_t addendchar：结尾字符
u8_t dirhi：写入字段方向，正向或逆向
返回值：是否生成成功
算法描述：制造空间，写入数据
/////////////////////////////////////////////// */
u8_t generate_a_field(struct netbuf *nbs,u8_t *key_str,u8_t *value_str,u8_t addendchar,u8_t dirhi)
{
	u16_t key_len=strlen((char const*)key_str);
	u16_t value_len=strlen((char const*)value_str);
	u16_t nblen=key_len+value_len+1;
	if (addendchar>0){
		nblen++;	
	}
	
	bool_t make_flag;
	if (dirhi)
		make_flag=netbuf_fwd_make_space(nbs,nblen);
	else{
		make_flag=netbuf_rev_make_space(nbs,nblen);
		netbuf_set_pos_to_start(nbs);
	}
	if(!make_flag){
		return 0;
	}
	netbuf_fwd_write_mem(nbs,key_str,key_len);
	netbuf_fwd_write_u8(nbs,'=');
	netbuf_fwd_write_mem(nbs,value_str,value_len);
	
	if (addendchar>0){
	  	if(netbuf_fwd_make_space(nbs,1)==0) return 0;
		netbuf_fwd_write_u8(nbs,addendchar);
	}
	if (!dirhi){	
		netbuf_set_pos_to_start(nbs);
	}
	return 1;
}
/*///////////////////////////////////////////////
generate_a_int_field
原型定义：void generate_a_int_field(struct netbuf *nb,u8_t *key_str,u32_t value,u8_t addendchar,u8_t dirhi)
描述：生成“key=整数value”格式nb，无分号，nb需事先分配
参数：
struct netbuf *nb：要写入的nb
u8_t *key_str：关键字
u32_t value：整数值
u8_t addendchar：结尾字符
u8_t dirhi：写入字段方向，正向或逆向
返回值：是否生成成功
算法描述：整数转成字符串，调用generate_a_field
/////////////////////////////////////////////// */
void generate_a_int_field(struct netbuf *nb,u8_t *key_str,u32_t value,u8_t addendchar,u8_t dirhi){

	u8_t strv[20];
	memset(strv,0,20);
	int_to_str(value,strv);

	generate_a_field(nb,key_str,strv,addendchar,dirhi);
}
/*
 * find_next_nes()
 */
/*///////////////////////////////////////////////
generate_a_int_field
原型定义：void generate_a_int_field(struct netbuf *nb,u8_t *key_str,u32_t value,u8_t addendchar,u8_t dirhi)
描述：生成“key=整数value”格式nb，无分号，nb需事先分配
参数：
struct netbuf *nb：要写入的nb
u8_t *key_str：关键字
u32_t value：整数值
u8_t addendchar：结尾字符
u8_t dirhi：写入字段方向，正向或逆向
返回值：是否生成成功
算法描述：整数转成字符串，调用generate_a_field
/////////////////////////////////////////////// */
s16_t s16_find_next_nes(u8_t receive_char,struct netbuf *nbr)
{
	s16_t order;
	u8_t find_flat=0;
	u16_t j=0;
	/*接下来查找*/
	u16_t remain=netbuf_get_remaining(nbr);/*执行此句后指针位置*/
	for(;j<remain;j++){
		u8_t tmp=netbuf_fwd_read_u8(nbr);
		if (tmp==receive_char){
			find_flat=1;
			break;
		}
	}
	if (find_flat){
		order=j;
	}
	else order= -1;
	return(order);
}
/*///////////////////////////////////////////////
find_next_nes
原型定义：u16_t find_next_nes(u8_t receive_char,struct netbuf *nbr)
描述：在nb中找到下一个关键字
参数：
u8_t receive_char：要查找的字符
struct netbuf *nbr：来源数据nb
返回值：如果找到，返回与目标字符相隔的字符数，找不到返回0
算法描述：循环查找，记录指针移动的位数
/////////////////////////////////////////////// */
u16_t find_next_nes(u8_t receive_char,struct netbuf *nbr)
{
	u16_t order;
	u8_t find_flat=0;
	u16_t j=0;
	/*接下来查找*/
	u16_t remain=netbuf_get_remaining(nbr);/*执行此句后指针位置*/
	for(;j<remain;j++){
		u8_t tmp=netbuf_fwd_read_u8(nbr);
		if (tmp==receive_char){
			find_flat=1;
			break;
		}
	}
	if(find_flat){
		order=j;
	}
	else order=0;
	return(order);
}	




/*
 * 获取指定字符前的字段，写入新nb；缩减源nb至指定字符后；如未找到将源nb整体内容读入
 * 新nb中，不做缩减
 */
/*///////////////////////////////////////////////
get_a_field_and_remove_it
原型定义：u8_t get_a_field_and_remove_it(struct netbuf *nbsrc,struct netbuf *nbnew,u8_t def_char)
描述：在nb中找到下一个字段并移除
参数：
struct netbuf *nbsrc：来源数据nb
struct netbuf *nbnew：移除的字段将要写入的位置
u8_t def_char：寻找的目标字符
返回值：是否成功找到字段并写入了新的nb
算法描述：找到字符串，写入新nb，截短原nb
/////////////////////////////////////////////// */
u8_t get_a_field_and_remove_it(struct netbuf *nbsrc,struct netbuf *nbnew,u8_t def_char)
{
	netbuf_reset(nbnew);
	if (netbuf_get_extent(nbsrc)==0) return FALSE;
	netbuf_set_pos_to_start(nbsrc);
	u16_t to_char_lenth=find_next_nes(def_char,nbsrc);
	if (to_char_lenth==0) to_char_lenth=netbuf_get_extent(nbsrc);
	

	bool_t make_flag=netbuf_fwd_make_space(nbnew,to_char_lenth);
	if(!make_flag){
		return FALSE;
	}
	netbuf_set_pos_to_start(nbsrc);
	netbuf_fwd_copy(nbnew,nbsrc,to_char_lenth);
	netbuf_set_pos_to_start(nbnew);
	if (to_char_lenth<netbuf_get_extent(nbsrc))
	netbuf_advance_pos(nbsrc,1);
	netbuf_set_start_to_pos(nbsrc);
	if (netbuf_get_extent(nbnew) ) return TRUE;
	return FALSE;
}




    
/*///////////////////////////////////////////////
get_key_and_remove_it
原型定义：u8_t get_key_and_remove_it(struct netbuf *nbsrc,u8_t *key,u8_t max_len)
描述：在nb中找到关键字并移除	
参数：
struct netbuf *nbsrc：来源数据nb
u8_t *key：移除的关键字将要写入的位置
u8_t def_char：寻找的目标字符
u8_t max_len：寻找的最远距离，超过后认为查找失败
返回值：是否成功找到
算法：获取关键字，读入变量代回，缩减源nb至等号后
/////////////////////////////////////////////// */
u8_t get_key_and_remove_it(struct netbuf *nbsrc,u8_t *key,u8_t max_len)
{
	if(!nbsrc){
		return FALSE;
	}
	
	if(netbuf_get_extent(nbsrc)==0){
		return FALSE;
	}
	u16_t cur_pos=netbuf_get_pos(nbsrc);	//2009.01.12 liye
	u16_t equa_mark_lenth= find_next_nes('=',nbsrc);
	if(equa_mark_lenth==0 || equa_mark_lenth>max_len){
		return FALSE;
	}
	//u8_t *send_key=heap_alloc(equa_mark_lenth+1);  //分配内存
	memset(key,0,equa_mark_lenth+1);
//	netbuf_set_pos_to_start(nbsrc);		//2009.01.12 liye
	netbuf_set_pos(nbsrc,cur_pos);		//2009.01.12 liye
	netbuf_fwd_read_mem(nbsrc,key,equa_mark_lenth);
	
	netbuf_advance_pos(nbsrc,1);
	netbuf_set_start_to_pos(nbsrc);
	return(TRUE);
}

/*///////////////////////////////////////////////
add_nch_bef_str_in_nb
原型定义：void add_nch_bef_str_in_nb(struct netbuf *nb,u8_t ch,u8_t tlen,u8_t *str)
描述：在nb写入字符串，并在字符串前补指定长度的数据	
参数：
struct netbuf *nb：要写入的nb
u8_t ch：在写字符串前补的数据
u8_t tlen：字符串与补足数据的总长
u8_t *str：要写入的字符串
返回值：空
算法：制造空间，依次写入
/////////////////////////////////////////////// */
void add_nch_bef_str_in_nb(struct netbuf *nb,u8_t ch,u8_t tlen,u8_t *str){
  	u8_t tmp_len;
	u8_t add_space;
	u8_t i;
	tmp_len = strlen((const char*)str);
	if(tmp_len<tlen){
		add_space=tlen-tmp_len;;
		for(i=0;i<add_space;i++){
			netbuf_fwd_write_u8(nb,ch);
		}
	}
	netbuf_fwd_write_mem(nb,str,tmp_len);
}





//浮点字符转换函数
/*///////////////////////////////////////////////
flt_str_to_float
原型定义：float flt_str_to_float(u8_t *str)
描述：转换浮点字符串到浮点数	
参数：
u8_t *str：要转换的字符串
返回值：转成的浮点数
算法：以小数点分割，分别获取到整数部分和小数部分，换算成整数
	小数部分循环/10直到小于1，加入整数部分得到最终值
/////////////////////////////////////////////// */ 
float flt_str_to_float(u8_t *str)
{

	u8_t n10=0;
	u16_t str_lenth=strlen((char const*)str);
	u8_t  fstr[20];	//20101118 change str_lenth+1 to 20
	memset(fstr,0,str_lenth+1);
	u8_t i;
	u8_t ch;
	u8_t j=0;
	u8_t dotpos=str_lenth;
	for(i=0;i<str_lenth;i++){		
		ch=str[i];
		if (!((ch>='0' && ch<='9') || ch=='.' || ch=='-'))
			return 0; 
		if(ch=='.'){
			str[i]=' ';
			dotpos=i;
			//break;
		}else{
			fstr[j]=ch;
			j++;	
		}
	}

	//memcpy(int_part_str,str,i);
	
	s32_t value=atol((char const*)fstr);
	float f= value;
	if(str_lenth<=dotpos)
		n10=0;
	else
		n10=str_lenth-dotpos-1;
		
	for (i=0;i<n10;i++){
		f=f/10;
	}
	return f;
	
}
////浮点数转字符串，转换超过s32_t上限的数字会出错
/*///////////////////////////////////////////////
float_to_flt_str
原型定义：void float_to_flt_str(float num,u8_t *str)
描述：转换浮点数到浮点字符串	
参数：
float num：要转换的浮点数
u8_t *str：转成的字符串
返回值：空
算法：以小数点分割，分别获取到整数部分和小数部分，换算成整数
	小数部分循环/10直到小于1，加入整数部分得到最终值
/////////////////////////////////////////////// */ 
void float_to_flt_str(float num,u8_t *str)
{
	if(num<0){
		strcat((char*)str,"-");		
		num *= -1;	//转成正数
	}
	u8_t tmp[13];
	memset(tmp,0,13);
	u32_t int_part = (u32_t)num;	//取得整数部分
	u64_t multy_time=(u64_t)num;
	u64_t multy_num = multy_time;
	while(num>multy_num){
		num*=10;	//放大原始值至
		multy_time*=10;
		multy_num=(u64_t)num;
		if(multy_time == multy_num){	//小数点后没有有效数字
			tmp[11]++;	//小数点后0计数自增
		}		
#if defined(FORCE_DECI_TWO_NUM)		
		if(tmp[12]>=2){  	
			tmp[12]=20;
# else		
		if(tmp[12]>=10){	
			
#endif		
			break;
		}	
		tmp[12]++;	//小数位数计数
//		if(multy_num>0xFFFFFFFFFFFF){
//			break;
//		}
	}
	u32_t deci_part = multy_num -multy_time;
#if defined(FORCE_DECI_TWO_NUM)	
	u8_t tail_num =0;
	if(tmp[12]==20){	//需要舍掉最后一个数字
//	if(deci_part>=100){
		tail_num = deci_part%10;
		deci_part -= tail_num;	//舍掉尾数
		if(tail_num>4){
			deci_part +=10;	//四舍五入
			if(deci_part==10){
				tmp[11]--;
			}
		}
		if(deci_part>=100){	//进位引起小数位数减少
			if(tmp[11]){
				tmp[11]--;
			}
		}
		deci_part /=10;	//消掉最后一位小数
/*		if(deci_part>=10){	//0.009 不能查0
			deci_part /=10;	//0.098 ->查9
		}
		if(tail_num>4){
			deci_part ++;
			if(deci_part>=10){
				if(tmp[11]){
					tmp[11]--;
				}
//				deci_part /=10;
			}
		}	*/
		
		if(deci_part>=100){	//小数进位引起整数增加
			int_part++;
			deci_part=0;
		}	
		if(!deci_part){
			tail_num =20;
		}
	}
#endif	
	int_to_str(int_part,tmp);
	strcat((char*)str,(char const*)tmp);
#if defined(FORCE_DECI_TWO_NUM)	
	if(tail_num==20){
		strcat((char*)str,".");
		strcat((char*)str,"00");
	}
#endif
	if(deci_part){
		
		memset(tmp,0,11);
		int_to_str(deci_part,tmp);
		strcat((char*)str,".");
		while(tmp[11]--){
			strcat((char*)str,"0");
		}
		strcat((char*)str,(char const*)tmp);
	}
//	return 1;
}


////浮点数转字符串，转换超过s32_t上限的数字会出错
/*///////////////////////////////////////////////
float_to_str
原型定义：void float_to_str(float num,u8_t *str,u8_t point)
描述：转换浮点数到浮点字符串	
参数：
float num：要转换的浮点数
u8_t *str：转成的字符串
u8_t point 需要保留的小数点位
返回值：空
算法：
/////////////////////////////////////////////// */ 
void float_to_str(float num,u8_t *str,u8_t point)
{
    s32_t v_int;		//整数部分
    s32_t v_dot;		//小数部分
    float v_int_float;		//整数部分浮点形式
    float v_dot_float;		//小数部分浮点形式
    float v_num;
    u8_t i;
    
    u8_t s_int[20];
    u8_t s_dot[20];
    
    memset(s_int,0,20);
    memset(s_dot,0,20);
    
    v_num = num;
    if(v_num<0)
    {
      	v_num *=-1.0;
    }
    v_int = (s32_t)v_num;
    
    v_int_float = (float)v_int;
    v_dot_float = v_num-v_int_float;
    
    v_dot_float += 1.0f;		//用于转换迭代
    
    for(i=0;i<point;i++)
    {
      v_dot_float *=10;
    }
    v_dot = (s32_t)v_dot_float;
    
    int_to_str(v_int,s_int);
    if(point !=0)
    {
	int_to_str(v_dot,s_dot);
	if(s_dot[1]!=0)
	{
		s_dot[0]='.';		//转换迭代
	}
	else
	{
	  	s_dot[0]=0;		//转换迭代
	}
    }
    if(num <0)
    {
      	strcpy((char*)str,(const char*)"-");
    }
    strcat((char*)str,(const char*)s_int);
    strcat((char*)str,(const char*)s_dot);
}



/*
//2007.09.29 add three functions
u8_t get_area(u16_t point, u16_t lowa,u16_t upa ){
	if (point<lowa) return 1;
	if (point>upa) return 3;
	return 2;
}
bool_t isatsamearea(u16_t point1,u16_t point2, u16_t lowa,u16_t upa ){
	if( get_area(point1,  lowa, upa )==get_area(point2,  lowa, upa )) return TRUE;
	return FALSE;
}
*/
//2008.10.30 add  liye
/*///////////////////////////////////////////////
get_unicode_str_len
原型定义：u16_t get_unicode_str_len(void *buffer,u16_t max_len)
描述：计算unicode字符串的长度
	比如31003200330034000000 则返回8
参数：
void *buffer：要计算长度的字符串位置
u16_t max_len：最大限定长度
返回值：字符串长度
算法：找到连续两个0000时认为字符串终止，返回指针移动的位数
/////////////////////////////////////////////// */ 
u16_t get_unicode_str_len(void *buffer,u16_t max_len)
{
	u16_t len,tmp;
	u8_t *p=buffer;
	len=0;
	while(1){
		tmp=*p+*(p+1);
		if(tmp==0){
			break;
		}
		p=p+2;
		len=len+2;
		if(len>=max_len){
			len=max_len;
			break;
		}
	}
	return len;
	
}

/*///////////////////////////////////////////////
mem_to_nb_to_str
原型定义：bool_t mem_to_nb_to_str(void *p,u16_t len,struct netbuf *nb)
描述：把内存中的数据块转成可见的十六进制字符并写入nb.
参数：
void *p：待转换的数据
u16_t len：待转换的数据长度
返回值：1转换成功   0转换失败。
算法：写入nb，调用mem_to_str生成扩展nb，用扩展nb内容替换原nb内容
/////////////////////////////////////////////// */
bool_t mem_to_nb_to_str(void *p,u16_t len,struct netbuf *nb)
{
	struct netbuf *nb_tmp=netbuf_alloc();
	if(!nb_tmp){
		return 0;
	} 
	if(!netbuf_fwd_make_space(nb_tmp,len)){
		netbuf_free(nb_tmp);
		return 0;
	}
	netbuf_fwd_write_mem(nb_tmp,p,len);
	netbuf_set_pos_to_start(nb_tmp);
	struct netbuf *nbs=netbuf_alloc();
	if(!nbs){
		netbuf_free(nb_tmp);
		return 0;
	}
	if(!mem_to_str(nb_tmp,nbs)){
		netbuf_free(nb_tmp);
		netbuf_free(nbs);
		return 0;
	}
	u16_t lenth=netbuf_get_extent(nbs); 
	netbuf_set_pos_to_end(nb);
	if(!netbuf_fwd_make_space(nb,lenth)){
		netbuf_free(nb_tmp);
		netbuf_free(nbs);
		return 0;
	}
	netbuf_fwd_copy(nb,nbs,lenth);
	netbuf_set_pos_to_start(nb);
	netbuf_free(nb_tmp);
	netbuf_free(nbs);
	return 1;
}

////////////查询历史上不存在的数据(已被覆盖或还未产生)时返回最新存储的数据，否则返回查询处数据
/*///////////////////////////////////////////////
get_real_num
原型定义：u32_t get_real_num(u32_t cur_pos,u16_t recv_num,u16_t circle_num)
描述：从要求读取号码获取逻辑现存号码.
参数：
u32_t cur_pos：当前逻辑最大号码
u16_t recv_num：要读取的号码
u16_t circle_num：物理号码范围（物理上可存放多少数据）
返回值：现存逻辑号码。
算法：已覆盖的数据或尚未产生的数据返回当前最新号，否则返回读取号码
/////////////////////////////////////////////// */
u32_t get_real_num(u32_t cur_pos,u16_t recv_num,u16_t circle_num)
{
	if(cur_pos<=recv_num){
		return cur_pos-1;		//还未产生的数据
	}
//	return (cur_pos-1-recv_num)/circle_num*circle_num+recv_num;	//2011.12.28 add -1	//返回的是实际对应位置
	////已被覆盖的数据返回cur_pos-1 否则recv_num
	return (cur_pos-1-recv_num >= circle_num)? cur_pos-1 : recv_num;
}

/*///////////////////////////////////////////////
num_judge_data_valid
原型定义：bool_t num_judge_data_valid(struct netbuf *nb)
描述：判断收到的数据包是否有效.
参数：
struct netbuf *nb：要判断的数据包
返回值：1 有效  0 无效。
算法：数据包中FF个数超过总数据三分之二认为无效，否有效
/////////////////////////////////////////////// */
/////////////////////////数据有效性判断函数
bool_t num_judge_data_valid(struct netbuf *nb)
{
	u16_t num,len;
	u8_t tmp;
	num=0;
	len=netbuf_get_extent(nb);
	netbuf_set_pos_to_start(nb);
	while(netbuf_get_remaining(nb)){
		tmp=netbuf_fwd_read_u8(nb);
		if(tmp==0xFF){
			num++;
		}
	}
	netbuf_set_pos_to_start(nb);
	if(num*3>len*2){
		return 0;
	}
	return 1;	
}

/*///////////////////////////////////////////////
first_judge_data_valid
原型定义：bool_t first_judge_data_valid(struct netbuf *nb)
描述：通过数据包第一个字节是否为FF 判断数据包是否有效.
参数：
struct netbuf *nb：要判断的数据包
返回值：1 有效  0 无效。
算法：
/////////////////////////////////////////////// */
bool_t first_judge_data_valid(struct netbuf *nb)
{
	netbuf_set_pos_to_start(nb);
	if(netbuf_fwd_read_u8(nb)==0xFF){
		return 0;
	}else{
		netbuf_set_pos_to_start(nb);
		return 1;
	}
}

////字节补偿
/*///////////////////////////////////////////////
byte_replen
原型定义：void byte_replen(u8_t *p,u8_t des_len)
描述：在字符串前以'0'补足长度
参数：
u8_t *p：要补足的字符串
u8_t des_len：补足长度
返回值：空
算法：写入netbuf，补足后再读出
/////////////////////////////////////////////// */
void byte_replen(u8_t *p,u8_t des_len)
{
	if(strlen((char const*)p)>=des_len) return;
	struct netbuf *nb=netbuf_alloc();
	if(!nb) return;
	if(!netbuf_rev_make_space(nb,des_len)){
		netbuf_free(nb);
		return;
	}
	netbuf_rev_write_mem(nb,p,strlen((char const*)p));
	while(netbuf_get_preceding(nb)){
		netbuf_rev_write_u8(nb,'0');
	}
	netbuf_fwd_read_mem(nb,p,des_len);
	p[des_len]=0;
	netbuf_free(nb);
}
//////////////////////////内存检测函数


/*///////////////////////////////////////////////
nb_span_add_char
原型定义：bool_t nb_span_add_char(struct netbuf *nb,u16_t span,u8_t des_char,u8_t order)
描述：在nb中以指定间隔插入数据
参数：
struct netbuf *nb：要插入数据的nb
u16_t span：间隔
u8_t des_char：插入数据
u8_t order：是否在头上插入第一个数据 0 插入
返回值：是否插入成功
算法：制造空间，将原nb数据和插入数据写入新nb，再写回原nb
/////////////////////////////////////////////// */
bool_t nb_span_add_char(struct netbuf *nb,u16_t span,u8_t des_char,u8_t order)
{
	struct netbuf *nbs=netbuf_alloc();
	if(!nbs){
		return 0;
	}
	u16_t srclen=netbuf_get_extent(nb);
	u16_t num;
	if(order){
		num=(srclen%span)?(srclen/span):(srclen/span-1);
	}else{
		num=(srclen%span)?(srclen/span+1):(srclen/span);
	}
	if(!num){
		return 0;
	}
	u16_t len=srclen+num;
	if(!netbuf_fwd_make_space(nbs,len)){
		netbuf_free(nbs);
		return 0;
	}
	netbuf_set_pos_to_start(nb);
	while(srclen>span){
		if(order){
			netbuf_fwd_copy(nbs,nb,span);
			netbuf_fwd_write_u8(nbs,des_char);
		}else{
			netbuf_fwd_write_u8(nbs,des_char);
			netbuf_fwd_copy(nbs,nb,span);
		}
		srclen-=span;
	}
	if(order){
		netbuf_fwd_copy(nbs,nb,srclen);
	}else{
		netbuf_fwd_write_u8(nbs,des_char);
		netbuf_fwd_copy(nbs,nb,srclen);
	}
	netbuf_reset(nb);
	netbuf_set_pos_to_start(nbs);
	if(!netbuf_fwd_make_space(nb,len)){
		netbuf_free(nbs);
		return 0;
	}
	netbuf_fwd_copy(nb,nbs,len);
	netbuf_set_pos_to_start(nb);
	return 1;
}




/*///////////////////////////////////////////////
revert_data
原型定义：revert_data(u8_t *p_src,u8_t len,u8_t *p_des)
描述：指定长度的内存块翻转写入另一段内存
	如："12345"转成"54321"。
参数：
u8_t *p_src：源内存块
u8_t len：翻转长度
u8_t *p_des：目的内存块
返回值：空
算法：
/////////////////////////////////////////////// */ 
void revert_data(u8_t *p_src,u8_t len,u8_t *p_des)
{
	u8_t i=0;
	while(len-i){
		p_des[len-1-i]=p_src[i];
		i++;
	}
}


/*///////////////////////////////////////////////
s_str_to_mem
原型定义：void s_str_to_mem(u8_t *src,u8_t src_len,u8_t *des)
描述：内存操作，把指定长度的十六进制字符转成内存中的HEX格式.
参数：
u8_t *src：待转换的数据
u8_t *des：转成的数据
返回值：空
算法：限定长度内循环操作，合并源内存块字节后写入新的内存块
/////////////////////////////////////////////// */
void s_str_to_mem(u8_t *src,u8_t src_len,u8_t *des)
{
	u8_t i,j=0;
	for(i=0;i<src_len;i+=2){
		des[j]=two_asc_to_hex(src[i],src[i+1]);
		j++;
	}
}
/*///////////////////////////////////////////////
s_str_to_mem
原型定义：void s_str_to_mem(u8_t *src,u8_t src_len,u8_t *des)
描述：内存操作，把指定长度的内存中的HEX数据转成十六进制字符.
参数：
u8_t *src：待转换的数据
u8_t *des：转成的数据
返回值：空
算法：限定长度内循环操作，拆分源内存块字节后写入新的内存块
/////////////////////////////////////////////// */
void s_mem_to_str(u8_t *src,u8_t src_len,u8_t *des)
{
	u8_t ch,i,j=0;
	for(i=0;i<src_len;i++){
		ch=src[i];
		des[j]=ASC[ch>>4];
		des[j+1]=ASC[ch & 0x0F];
		j+=2;
	}
}



//////////////////////////////////2011.12.13 add s
/////////////116°18.007402'
/*///////////////////////////////////////////////
get_gps_protocol_value
原型定义：u32_t get_gps_protocol_value(u8_t *v_str,u8_t dirct)
描述：从经纬度字符串生成特定协议的u32_t数据.
参数：
u8_t *v_str：待转换字符串
u8_t dirct：方向标识
返回值：转成的long整数
算法：按协议要求移位合并
/////////////////////////////////////////////// */
u32_t get_gps_protocol_value(u8_t *v_str,u8_t dirct)
{
	u8_t i,len=0;
	u32_t value=0;//,value_t=0;
	u8_t tmp[10];
	len=strlen((char const*)v_str);
	if(len>20){
		return 0xFFFFFFFF;
	}
	if(v_str[len-1]!=0x27){
		return 0xFFFFFFFF;
	}
	v_str[len-1]=0;
	for(i=0;i<len;i++){
		if(v_str[i]==0xA1 && v_str[i+1]==0xE3){
			break;
		}
	}
	if(i==len){
		return 0xFFFFFFFF;
	}
	memset(tmp,0,10);
	strncpy((char*)tmp,(char const*)v_str,i);
	value=atoi((char const*)tmp);	//度进入
	len -= i+2;	//剩余字符串长度
	v_str += i+2;	//字符串头前移
	for(i=0;i<len;i++){
		if(v_str[i]=='.'){	
			break;
		}
	}
	if(i==len){
		return 0xFFFFFFFF;
	}
	memset(tmp,0,10);
	strncpy((char*)tmp,(char const*)v_str,i);
	value+= atoi((char const*)tmp)<<8;	//分进入
	len -= i+1;
	v_str += i+1;	//字符串头前移
	for(i=0;i<len;i++){
		if(v_str[i]!='0'){
			break;
		}
	}
	if(i<len){
		value+=(u32_t)i <<29;
		v_str += i;	//字符串头前移至有效数字起始处
	}
	len=strlen((char const*)v_str);
	if(len>4){	//有效数字个数多于4个
		v_str[4]=0;
	}
//	value_t = atoi(v_str);
//	while(value_t>0x3FFF){
//		value_t/=10;
//	}
	value+= (u32_t)(atoi((char const*)v_str)&0x3FFF)<<14;	//分小数进入
//	value+= (value_t&0x3FFF)<<14;
	if(dirct){
		value+= 0x10000000;
	}
	return value;
}
/*///////////////////////////////////////////////
get_gps_protocol_str
原型定义：void get_gps_protocol_str(u32_t value,u8_t *v_str)
描述：从特定协议的u32_t数据生成经纬度字符串.
参数：
u32_t value：待转换整数
u8_t *v_str：转成的经纬度字符串
返回值：空
算法：移位分解成各个字段
/////////////////////////////////////////////// */
void get_gps_protocol_str(u32_t value,u8_t *v_str)
{
	v_str[0]=0;
	u16_t tmp_v=0;
	u8_t tmp[10];
	memset(tmp,0,10);
	tmp_v=value&0xFF;	//取出度
	value>>=8;		//移除度
	int_to_str(tmp_v,tmp);
	strcat((char*)v_str,(char const*)tmp);	//写入度
	strcat((char*)v_str,"°");
	tmp_v=value&0x3F;	//取出分
	value>>=6;		//移除分
	memset(tmp,0,10);
	int_to_str(tmp_v,tmp);	
	strcat((char*)v_str,(char const*)tmp);	//写入分
	strcat((char*)v_str,".");
	tmp_v=value&0x3FFF;	//取出分小数
	int_to_str(tmp_v,tmp);	
	value>>=15;		//移除分小数和方位标志
	while(value--){
		strcat((char*)v_str,"0");
	}
	strcat((char*)v_str,(char const*)tmp);	//写入分小数
	strcat((char*)v_str,"'");
}



/*///////////////////////////////////////////////
get_byte_value
原型定义：bool_t get_byte_value(u8_t *intvalue, u8_t *strSource, u8_t len)
描述：将指定长度的数字字符串转成整数.
参数：
u8_t *intvalue：转换后整数放入位置
u8_t *strSource：待转换字符串
u8_t len：要转换的长度
返回值：是否转换成功  0 成功  1失败
算法：减差值得到中值，循环*10相加
/////////////////////////////////////////////// */
bool_t get_byte_value(u8_t *intvalue, u8_t *strSource, u8_t len)
{   
   u8_t ch;   
   u8_t  middlevalue=0;
   u8_t  i=0;   
   u8_t value;

   value=0;
   for (i=0;i<len;i++)
   {
		ch = strSource[i];
		if ((ch<48) || (ch>57))
			return 1;
		middlevalue=(u16_t)(ch-48);
		value=value*10+middlevalue;
   }
   (*intvalue)=value;
   return 0;
}


/*///////////////////////////////////////////////
byte_to_3char
原型定义：u8_t byte_to_3char(u8_t srcint,u8_t *des)
描述：将一个u8_t 数据转成三位数字字符串.  比如0xFF=>"255"
参数：
u8_t srcint：待转换整数
u8_t *des：转成的字符串
返回值：0代表转换成功
算法：取得个位、十位、百位数值，转换成字符，写入数组
/////////////////////////////////////////////// */
u8_t byte_to_3char(u8_t srcint,u8_t *des)
{
	u8_t i,chx;
	u32_t temp;
	temp=srcint;
	i=3;
	while(i!=0){
		i--;
		chx=temp % 10;
		temp /= 10;
		*(des+i) = ASC[chx];
	};
	return 0;	
}

/*///////////////////////////////////////////////
bytebuf_3char
原型定义：int bytebuf_3char(struct netbuf *nbbuf, struct netbuf *nb)
描述：将一个netbuf中的每个数据转成三位数字字符串写入另一个netbuf
参数：
struct netbuf *nbbuf：读数netbuf
struct netbuf *nb:写字符串netbuf
返回值：0代表转换成功
算法：每个数据转换，写入
/////////////////////////////////////////////// */
int bytebuf_3char(struct netbuf *nbbuf, struct netbuf *nb)
{
	u8_t temp;
	u8_t des[3];
	u8_t i;
	u16_t len;
	len=netbuf_get_remaining(nbbuf);
	if(len>500)	
		len=500;
	if ( netbuf_fwd_make_space(nb,3*len)) {
		for(i=0;i<len;i++){
			temp=netbuf_fwd_read_u8(nbbuf);
			byte_to_3char(temp,des);
			netbuf_fwd_write_mem(nb,des,3);		
		}
		netbuf_set_pos_to_start(nb);
		netbuf_set_pos_to_start(nbbuf);
		return 0;
	}
	return 1;
}

/*///////////////////////////////////////////////
char_bytebuf
原型定义：int char_bytebuf(struct netbuf *nb, struct netbuf *nbbuf)
描述：将一个netbuf中的每三位数字字符串转成单字节数据写入另一个netbuf
	bytebuf_3char的逆运算
参数：
struct netbuf *nb：读字符串netbuf
struct netbuf *nbbuf:写数netbuf
返回值：0代表转换成功  1代表转换失败
算法：每个数据转换，写入
/////////////////////////////////////////////// */
int char_bytebuf(struct netbuf *nb, struct netbuf *nbbuf)
{
	u8_t temp;
	u8_t des[3];
	u8_t i;
	u16_t len;
	len=netbuf_get_remaining(nb);
	if(len%3!=0 )
		return 1;
	len=len/3;
	if ( netbuf_fwd_make_space(nbbuf,len)) {
		for(i=0;i<len;i++){
			netbuf_fwd_read_mem(nb,des,3);
			get_byte_value(&temp,des,3);
			netbuf_fwd_write_u8(nbbuf,temp);			
		}
		netbuf_set_pos_to_start(nb);
		netbuf_set_pos_to_start(nbbuf);
		return 0;
	}
	return 1;
}

/*///////////////////////////////////////////////
byte_seq_arrange
原型定义：void byte_seq_arrange(void *src,u8_t len)
描述：将指定长度的内存块中数据字节倒序
参数：
void *src：数据内存块
u8_t len：倒序长度
返回值：0代表转换成功
算法：
/////////////////////////////////////////////// */

void byte_seq_arrange(void *src,u8_t len)
{
#if defined (LITTLE_ENDIAN_ENABLED)	
	if(len>20) return;
	u8_t *byte = src;
	u8_t tmp[20],i=0;
	memset(tmp,0,20);
	memcpy(tmp,byte,len);
	while(i<len){
		byte[i]=tmp[len-i-1];
		i++;
	}
#endif	
}
/*///////////////////////////////////////////////
judge_ff_80_error
原型定义：u8_t judge_ff_80_error(struct netbuf *nb)
描述：根据数据包头10个字节中FF和80个数判断收到的数据包是否有效.
参数：
struct netbuf *nb：要判断的数据包
返回值：0有效  FF 或 80 无效。
算法：数据包前10个字节中FF或80个数超过三分之二认为无效，否有效
/////////////////////////////////////////////// */

u8_t judge_ff_80_error(struct netbuf *nb)
{
	u8_t i=0,tmp,find_ff=0,find_80=0;//,sub_ff=0,sub_80=0;
	u16_t len=netbuf_get_extent(nb);
	if(len>10){
		len=10;
	}
	for(;i<len;i++){
		tmp=netbuf_fwd_read_u8(nb);
		if(tmp==0xFF){
			find_ff ++;
		}
		if(tmp==0x80){
			find_80 ++;
		}
	}
	netbuf_set_pos_to_start(nb);
	if(find_ff ==len){
		return 0xFF;
	}
	if(find_80 ==len){
		return 0x80;
	}
	find_ff = len/(len-find_ff);
	find_80 = len/(len-find_80);
	if(find_ff >=3){		//错误数据占总数据量三分之二以上
		return 0xFF;
	}
	if(find_80 >=3){		//错误数据占总数据量三分之二以上
		return 0x80;
	}
	return 0;
}




