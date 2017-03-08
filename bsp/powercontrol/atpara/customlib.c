/*--------------------------------------------------------------------------------------------------

                  ��Ȩ���� (C), 2015-2020, ������άӯ�����޹�˾

 -----------------------------------------------------------------------------------
  �� �� ��   : customlib.c
  �� �� ��   : ����
  ��    ��   : ����
  ��������   : 2016��7��13��
  ����޸�   :
  ��������   : ATָ���������C�ļ�
			����Ӧ�ò�Э��Ľ���������������Ƿ��ͼ���������
  �޸���ʷ   :
  1.��    ��   : 2016��7��13��
    ��    ��   : ����
    �޸�����   : �����ļ�
    
----------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include "customlib.h"

//#define FORCE_DECI_TWO_NUM 1

void float_data_filter_sort(float *buff, int len)//����С����˳��
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
 �� �� ��  : get_filte_float_data
 ��������  : ��ȡ����������������
		 �����ݽ��뻺�棬������ǰ�������������飬����ȡ�м�ֵ
 �������  : float r_value  	����һ�βɼ���ȡ��������
		 float *fifo	ԭʼ��������
		 float *sort	����������
		 u8_t *cur_pos	�������鵱ǰд��λ��
		 float *des		�ϱ�����
		 int len		 �������鳤��
 �������  : void
 �� �� ֵ  : void
 �޸���ʷ      :
  1.��    ��   : 2016��6��14��
    ��    ��   : ����
    �޸�����   : �����ɺ���

*****************************************************************************/
void get_filte_float_data(float r_value,float *fifo,float *sort,u8_t *cur_pos,float *des,int len)
{
	fifo[*cur_pos]=r_value;	//�����ݽ��뻺��
	*cur_pos = (*cur_pos +1)%len;	//д��λ��pos��λ
	memcpy(sort,fifo,len*4);	//��������������
	float_data_filter_sort(sort,len);		//����
	*des = sort[len/2];	//ȡ�м�ֵ
}


const u8_t ASC[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,'A','B','C','D','E','F'};
const u8_t ASCNB[13] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x23,0x2A};

/* ///////////////////////////////////////////////
get_int_value
ԭ�Ͷ��壺bool_t get_int_value(u16_t *longvalue, u8_t *strSource, u8_t len);
������ת���ַ���Ϊ��������
������
u16_t *longvalue��ת�������һ������ָ�룩
u8_t *strSource,��Ҫת�����ַ���
u8_t len���ַ����ĳ���
�㷨�������ַ���ת�������ִ��룬ѭ��*10 ���
����ֵ��ת���ɹ��ı�־��0���ɹ���1��ʧ�ܡ�
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
ԭ�Ͷ��壺bool_t getlongvalue (u16_t *longvalue, u8_t *strSource, u8_t len);
������ת���ַ���Ϊ��������
������
u16_t *longvalue��ת�������һ��������ָ�룩
u8_t *strSource,��Ҫת�����ַ���
u8_t len���ַ����ĳ���
�㷨�������ַ���ת�������ִ��룬ѭ��*10 ���
����ֵ��ת���ɹ��ı�־��0���ɹ���1��ʧ�ܡ�
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




//������һ���ֶεĳ���
/* ///////////////////////////////////////////////
line_next_f_len
ԭ�Ͷ��壺extern u8_t line_next_f_len(struct netbuf *nb);
���������㻺��������һ���ֶεĳ��ȣ��ֶ�֮���á�,���ָ
������
struct netbuf *nb��������
����ֵ�����ȡ�
�㷨��ѭ����ȡ�ȶԣ��ҵ�','ʱ�ж�����
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
ԭ�Ͷ��壺extern void line_jump_n_f(struct netbuf *nb,u8_t n);
�������ӻ������ĵ�ǰλ����ǰ�ƶ�N���ֶΣ��ֶ�֮����ָ���ָ����ָ
������
struct netbuf *nb��������
u8_t separator���ָ���
u8_t n�����ٸ��ֶ�
����ֵ���ҵ��ָ�����posǰ��
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
input:     u8_t* in_str       �����ַ���
           u16_t in_len       �ַ�������
output:    u16_t     CRC��
note:      ����CRC16-modubs
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
 �� �� ��  : read_nb_modbus_crc
 ��������  : ��ȡ���ݰ���modbus CRC
 �������  : struct netbuf *nb���յ���modbus���ݰ�
 �������  : void
 �� �� ֵ  : u16_t CRCУ��ֵ
 �޸���ʷ      :
  1.��    ��   : 2016��04��11��
    ��    ��   : ����
    �޸�����   : �����ɺ���

*****************************************************************************/
u16_t read_nb_modbus_crc(struct netbuf *nb)
{
	u16_t read;
	netbuf_set_pos_to_end(nb);
	netbuf_retreat_pos(nb,2);
	read = netbuf_fwd_read_u8(nb);	//��ȡ���ֽ�
	read |= (u16_t)netbuf_fwd_read_u8(nb)<<8;	//������ֽ�
	return read;
}

/*****************************************************************************
 �� �� ��  : calc_nb_modbus_crc
 ��������  : �������ݰ���modbus CRC
 �������  : struct netbuf *nb������������ݰ�
 �������  : void
 �� �� ֵ  : u16_t CRCУ��ֵ
 �޸���ʷ      :
  1.��    ��   : 2016��04��11��
    ��    ��   : ����
    �޸�����   : �����ɺ���

*****************************************************************************/
u16_t calc_nb_modbus_crc(struct netbuf *nb)
{
	u16_t i,j;
	u16_t temp_crc = 0xFFFF;
	u16_t len = netbuf_get_extent(nb)-2;	//Ҫ����У������ݲ�����������ֽ�
	
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
 �� �� ��  : common_modbus_data_valid
 ��������  : �ж�ͨ��modbusָ����ȷ������CRC��ȷ����������modbus�������
 �������  : struct netbuf *nb���յ���modbus���ݰ�
 �������  : void
 �� �� ֵ  : void
 �޸���ʷ      :
  1.��    ��   : 2016��04��11��
    ��    ��   : ����
    �޸�����   : �����ɺ���

*****************************************************************************/
u8_t common_modbus_data_valid(struct netbuf *nb)
{
	u16_t recv_crc,calc_crc;	//׼�������У��
	if(netbuf_get_extent(nb)<7) return 0;
	recv_crc = read_nb_modbus_crc(nb);
	calc_crc = calc_nb_modbus_crc(nb);
	if(recv_crc != calc_crc) return 0;
	netbuf_set_pos_to_start(nb);
	return 1;
}
/*///////////////////////////////////////////////
revert_string
ԭ�Ͷ��壺void revert_string(u8_t *str)
������ת���ַ���˳��
	�磺"12345"ת��"54321"��
������
u8_t *str:Ҫת�����ַ���
����ֵ����
�㷨��
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
ת���������ַ���
*/  
/*///////////////////////////////////////////////
int_to_str
ԭ�Ͷ��壺u8_t int_to_str(u32_t srcint,u8_t *des)
������ת���������ַ���	
������
u32_t srcint��Ҫת��������
u8_t *des��ת�ɵ��ַ���
����ֵ��0ת���ɹ�
�㷨��ѭ�� %10��/10���õ�����λ���֣�ת���ַ����ϲ�
/////////////////////////////////////////////// */ 
u8_t int_to_str(u32_t srcint,u8_t *des){
	u8_t j,chx;
	u32_t temp;
	temp=srcint;
	j=0;
	
	/*����03.11.02����*/
	if(temp==0){ 
		*(des+j)=ASC[0];
		j++;
	}
	/*����03.11.02����*/
	
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
ԭ�Ͷ��壺u8_t int64_to_str(u64_t srcint,u8_t *des)
������ת���������ַ���	
������
u64_t srcint��Ҫת��������
u8_t *des��ת�ɵ��ַ���
����ֵ��0ת���ɹ�
�㷨��ѭ�� %10��/10���õ�����λ���֣�ת���ַ����ϲ�
/////////////////////////////////////////////// */ 
u8_t int64_to_str(u64_t srcint,u8_t *des){
	u8_t j,chx;
	u64_t temp;
	temp=srcint;
	j=0;
	
	/*����03.11.02����*/
	if(temp==0){ 
		*(des+j)=ASC[0];
		j++;
	}
	/*����03.11.02����*/
	
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
ԭ�Ͷ��壺u8_t int_to_str16(u32_t srcint,u8_t *s,u8_t len)
������ת��������16�����ַ���	
������
u32_t srcint��Ҫת��������
u8_t *s��ת�ɵ��ַ���
u8_t len��ת�ɵ��ַ�������
����ֵ��0ת���ɹ�
�㷨��ѭ�� %16��/16���õ�����λ���֣�ת���ַ����ϲ�
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
ԭ�Ͷ��壺u8_t  str16_to_int(u8_t *s,u8_t *srcint,u8_t len)
������ת��16�����ַ���������	
������
u32_t srcint��Ҫת��������
u8_t *s��ת�ɵ��ַ���
u8_t len���ַ�������
����ֵ��0ת���ɹ�
�㷨���ַ�ת��������ѭ��*16���
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
ԭ�Ͷ��壺u8_t  s16tr_to_int(u8_t *s,u8_t *srcint)
������ת��16�����ַ���������	
������
u32_t srcint��Ҫת��������
u8_t *s��ת�ɵ��ַ���
����ֵ��1ת���ɹ�
�㷨���ַ�ת��������ѭ��*16���
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
ԭ�Ͷ��壺u8_t mac_to_str(u8_t *mac,u8_t *str)
���������ڴ�MAC����ת�����ַ���
	�磺0x001122334455ת��Ϊ"001122334455"��
������
u8_t *mac��Ҫת����MAC����
u8_t *str��ת�ɵ��ַ���
����ֵ��0ת���ɹ�	 
�㷨��˫�ֽ�ת��16�����ַ������ϲ�
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
ԭ�Ͷ��壺u8_t str_to_mac(u8_t *str,u8_t *mac)
��������MAC�ַ���ת�����ڴ�MAC����
	�磺"001122334455"ת��Ϊ0x001122334455��
������
u8_t *str��Ҫת�����ַ���
u8_t *mac��ת�ɵ�MAC����
����ֵ��0ת���ɹ�	 1ת��ʧ��
�㷨��˫�ֽ�ת��16�����ַ������ϲ�
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
ԭ�Ͷ��壺u16_t calc_checksum(struct netbuf *nb,u8_t cstype)
�������������ݱ��ĵ�У��λ��
������
struct netbuf *nb�����ݱ��Ļ�����
u8_t cstype��У�����ͣ���У������У��
����ֵ��У��ֵ
�㷨�������ӱ��ĵĵ�һ���ֽڵ����һ�����ֽڵĺͻ��������͡�
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
ԭ�Ͷ��壺u8_t byte_reverse(u8_t data)
������ת������bitλ˳��
	�磺00010010bת��01001000b��
������
u8_t data:Ҫת��������
����ֵ��ת��֮�������
�㷨��bitλ�Ե�
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
ԭ�Ͷ��壺u64_t  str_to_u64(const char *s,u8_t base )
������ת���ַ�����long long ����	
������
const char *s��Ҫת�����ַ���
u8_t base�����ݽ��ƣ�10��16
����ֵ��ת�ɵ�����
�㷨���ַ�ת��������ѭ��*base���
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
ԭ�Ͷ��壺u32_t str_to_u32(u8_t *s,u8_t base)
������ת���ַ�����long ����	
������
const char *s��Ҫת�����ַ���
u8_t base�����ݽ��ƣ�10��16
����ֵ��ת�ɵ�����
�㷨���ַ�ת��������ѭ��*base���
/////////////////////////////////////////////// */ 
u32_t str_to_u32(u8_t *s,u8_t base)
{
	u32_t val=0;
	u8_t ch,len=0;
	while(*s ){		//�ַ�����0x00��β���Զ����������ֻ���ĸ �ַ�
		if(len>19) break;		//�ַ����Ϊ20
		if((*s >= '0' && *s <= '9')||(*s>='a' && *s<='f')||(*s>='A' && *s<='F')){
			if(base==10 && *s>'9'){	//ʮ����ת�����н���ĸ
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

/*������2004.01.15���ӵĺ���*/

/*///////////////////////////////////////////////
asc_to_hex
ԭ�Ͷ��壺u8_t asc_to_hex(u8_t ch)
������ת��ʮ�������ַ�������	
	����'9'ת��9  'A'ת��10
������
u8_t ch��Ҫת�����ַ�
����ֵ��ת�ɵ�����
�㷨�������������Ĳ�ֵ
/////////////////////////////////////////////// */ 
static u8_t asc_to_hex(u8_t ch){	////20101118 add static
	u8_t ret;	
	ret=0;
	if ((ch>= 0x30) && (ch <= 0x39)) //ascii0-9��a-f
		    ret=ch - 0x30;
	if ((ch>= 0x41) && (ch <= 0x46))
		    ret = ch- 0x37;	
	return ret; 
}

/*///////////////////////////////////////////////
hex_to_asc
ԭ�Ͷ��壺u8_t hex_to_asc(u8_t hex,u8_t* str)
������ת��ʮ������ΪASC��
������
u8_t hex��Ҫת����HEX
����ֵ��HEXת��Ϊ2���ַ�
�㷨�� 
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
ԭ�Ͷ��壺u8_t bcd_to_hex(u8_t bcd)
������BCD�� ת��ʮ������
������
u8_t BCD��Ҫת����bcd
����ֵ��HEX
�㷨�� 
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
ԭ�Ͷ��壺u8_t hex_to_bcd(u8_t hex)
������ת��ʮ������ΪBCD��
������
u8_t hex��Ҫת����HEX
����ֵ��BCD
�㷨�� 
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
ԭ�Ͷ��壺u8_t two_asc_to_hex(u8_t high_ch,u8_t low_ch)
������������ASCIIת��Ϊһ��ʮ�����ƣ�ASCII�ķ�ΧΪ1-9��A-F����"3A"ת��Ϊ16����0x3A.
������
u8_t high_ch�����ڸ�λ��ASCII
u8_t low_ch�����ڵ�λ��ASCII
����ֵ��ת���Ľ��λ16��������
�㷨��ת��������λ�ϲ�
/////////////////////////////////////////////// */
u8_t two_asc_to_hex(u8_t high_ch,u8_t low_ch){

	return ((asc_to_hex(high_ch))<<4)+asc_to_hex(low_ch);
	
}



/*///////////////////////////////////////////////
mem_to_str
ԭ�Ͷ��壺bool_t mem_to_str(struct netbuf *nbsrc,struct netbuf *nbdes)
���������ڴ��е����ݿ�ת�ɿɼ���ʮ�������ַ�.
������
struct netbuf *nbsrc����ת��������
struct netbuf *nbdes��ת�ɵ�����
����ֵ��1ת���ɹ�   0ת��ʧ�ܡ�
�㷨����ԭnb��ȡ������չ�ֽں�д���µ�nb
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
ԭ�Ͷ��壺void value_mem_to_str(u16_t value,u8_t *str)
��������������HEX����ת�ɿɼ���ʮ�������ַ�.
������
u16_t value����ת��������
u8_t *str��ת�ɵ�����
����ֵ����
�㷨����չ�ֽں�д������
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
ԭ�Ͷ��壺bool_t str_to_mem(struct netbuf *nbsrc,struct netbuf *nbdes)
��������ʮ�������ַ�ת���ڴ��е�HEX��ʽ.
������
struct netbuf *nbsrc����ת��������
struct netbuf *nbdes��ת�ɵ�����
����ֵ��1ת���ɹ�   0ת��ʧ�ܡ�
�㷨����ԭnb��ȡ�����ϲ��ֽں�д���µ�nb
/////////////////////////////////////////////// */
bool_t str_to_mem(struct netbuf *nbsrc,struct netbuf *nbdes)
{
	//�������
	u8_t hc,lc,hexc;
	if (!nbsrc) return FALSE;
	if (!nbdes) return FALSE;
	
	netbuf_set_pos_to_end(nbdes);
	if(!netbuf_fwd_make_space(nbdes,netbuf_get_remaining(nbsrc)/2)){
		return FALSE;
	}
	
	while (netbuf_fwd_check_space(nbsrc,2)){//�ж�����
	//��ȡ��һ���ַ�
		hc=netbuf_fwd_read_u8(nbsrc);
	//��ȡ�ڶ����ַ�
		lc=netbuf_fwd_read_u8(nbsrc);
	//�ϲ�Ϊһ��HEX
		hexc=two_asc_to_hex(hc,lc);
	//д��nbdes
		netbuf_fwd_write_u8(nbdes,hexc);
		
	}//����>1
	netbuf_set_pos_to_start(nbdes);
	netbuf_set_pos_to_start(nbsrc);
	//����Ϊ1 0������
	return TRUE;
	
}
/*������2004.01.15���ӵĺ���*/


/*///////////////////////////////////////////////
nb_strcat_unicode
ԭ�Ͷ��壺void nb_strcat_unicode(struct netbuf *nb,u8_t *str)
��������һ���ַ�����unicode��ʽд��nb����-
������
struct netbuf *nbsrc��Ҫд���nb
u8_t *str����д�ַ���
����ֵ����
�㷨����nbβ����ռ䡢д���ַ���������unicode��
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




//�˺�������С��ʹ�ã�������������
/*///////////////////////////////////////////////
get_an_field
ԭ�Ͷ��壺void get_an_field(u8_t *temp,struct netbuf *nb,u16_t separator)
��������netbuf�л�ȡһ���ֶ�
������
u8_t *temp��ȡ�����ֶ�д���λ��
struct netbuf *nb����Դnetbuf
u16_t separator���ֶηָ���
����ֵ����
�㷨������ָ����뵱ǰλ�þ��룬��ȡ�˳��ȵ�����д�������У�nbָ��ǰ��
/////////////////////////////////////////////// */ 
void get_an_field(u8_t *temp,struct netbuf *nb,u16_t separator){
	u8_t flen;
//	flen=line_next_f_len(nb,',');
	u16_t remain_len;
	remain_len=netbuf_get_remaining(nb);
	if(separator>0xFF){
		if(remain_len>30){
			remain_len=30;	//��ȫ����,�ֶγ��Ȳ��ܴ���30
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
ԭ�Ͷ��壺u32_t get_an_int_field(struct netbuf *nb,u8_t base,u8_t separator)
��������netbuf�л�ȡһ�������ֶβ�����ת��������
������
struct netbuf *nb����Դnetbuf
u8_t base��ת�ɵ����ݽ��� 10��16
u16_t separator���ֶηָ���
����ֵ��ת�ɵ�����
�㷨��ȡ�������ַ�������ת��������
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
ԭ�Ͷ��壺char *itoaEX(char *obuf, u64_t x, unsigned int base, u8_t issigned)
������ת���������ַ��� ��itoa��֮ͬ����ʮ�������Ǵ�дA-F������Сдa-f
������
char *obuf��д���ַ���
u64_t x��Ҫת��������
unsigned int base�����ݽ��ƣ�10��16
u8_t issigned���Ƿ��з���
����ֵ���ַ�������λ��
�㷨������ת���ַ����ϲ�
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

//��UNICODE unicodes����ת��Ϊһ������ 30 00 30 00 31 00 32 00=>12 2007.01.14
/*///////////////////////////////////////////////
wordsToInt
ԭ�Ͷ��壺u16_t wordsToInt(u8_t len,u8_t *bytes)
��������UNICODE unicodes����ת��Ϊһ������ 34 00 33 00 31 00 32 00=>4312
������
u8_t len�������ַ�����
u8_t *bytes����ת������
����ֵ��ת�ɵ�����
�㷨�������ַ�ת��������ѭ��*10���
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
//��һ������ת��ΪUNICODE unicodes���� 12=>30 00 30 00 31 00 32 00 2007.01.14
/*///////////////////////////////////////////////
intToWords
ԭ�Ͷ��壺void intToWords(u16_t value, u8_t len,u8_t *bytes)
��������һ������ת��ΪUNICODE unicodes���� 12=>30 00 30 00 31 00 32 00 
������
u16_t value����ת������
u8_t len�������ַ�����
u8_t *bytes��д������
����ֵ����
�㷨��
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
ԭ�Ͷ��壺u32_t get_u32_t_from_bytes(u8_t *bytes)
������u32_t �����ֽ������
������
u8_t *bytes��ԭ���ݴ�ŵ�ַ
����ֵ�������������
�㷨����λ���
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
ԭ�Ͷ��壺void set_bytes_from_uint(u8_t *bytes,u32_t value,u8_t len)
��������һ�����������ֽ�������ڴ��� 
������
u8_t *bytes��Ҫ�����λ��
u32_t value��Ҫд������
u8_t len�������ֽ�����2��4��
����ֵ����
�㷨����λ��ֵ
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
ԭ�Ͷ��壺mem_calc_checksum(u8_t *bytes,u16_t blen,u8_t cstype)
�������������ݱ��ĵ�У��λ��
������
u8_t *bytes�����ݱ�����
u16_t blen�����ĳ���
u8_t cstype��У������  ��У������У��
����ֵ��У��ֵ
�㷨�������ӱ��ĵĵ�һ���ֽڵ����һ���ֽڵĺͻ��������͡�
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




//���ƻ���Э��

#define DIO_PRO_DA_CONST ",DA=&"



/*///////////////////////////////////////////////
nb_add_re
ԭ�Ͷ��壺void nb_add_re(struct netbuf *nbr)
��������nb��ӻس�
������
struct netbuf *nbr����Ҫ��ӻس���nb
����ֵ����
�㷨����������ռ䣬д������
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
generate_a_field()  ���ɡ�key=value����ʽnb���޷ֺţ�nb�����ȷ���
addendchar �Ƿ���Ҫ��β�ַ���;��
netbuf λ�÷������

*/
/*///////////////////////////////////////////////
generate_a_field
ԭ�Ͷ��壺u8_t generate_a_field(struct netbuf *nbs,u8_t *key_str,u8_t *value_str,u8_t addendchar,u8_t dirhi)
���������ɡ�key=value����ʽnb���޷ֺţ�nb�����ȷ���
������
struct netbuf *nbs��Ҫд���nb
u8_t *key_str���ؼ���
u8_t *value_str������ֵ
u8_t addendchar����β�ַ�
u8_t dirhi��д���ֶη������������
����ֵ���Ƿ����ɳɹ�
�㷨����������ռ䣬д������
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
ԭ�Ͷ��壺void generate_a_int_field(struct netbuf *nb,u8_t *key_str,u32_t value,u8_t addendchar,u8_t dirhi)
���������ɡ�key=����value����ʽnb���޷ֺţ�nb�����ȷ���
������
struct netbuf *nb��Ҫд���nb
u8_t *key_str���ؼ���
u32_t value������ֵ
u8_t addendchar����β�ַ�
u8_t dirhi��д���ֶη������������
����ֵ���Ƿ����ɳɹ�
�㷨����������ת���ַ���������generate_a_field
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
ԭ�Ͷ��壺void generate_a_int_field(struct netbuf *nb,u8_t *key_str,u32_t value,u8_t addendchar,u8_t dirhi)
���������ɡ�key=����value����ʽnb���޷ֺţ�nb�����ȷ���
������
struct netbuf *nb��Ҫд���nb
u8_t *key_str���ؼ���
u32_t value������ֵ
u8_t addendchar����β�ַ�
u8_t dirhi��д���ֶη������������
����ֵ���Ƿ����ɳɹ�
�㷨����������ת���ַ���������generate_a_field
/////////////////////////////////////////////// */
s16_t s16_find_next_nes(u8_t receive_char,struct netbuf *nbr)
{
	s16_t order;
	u8_t find_flat=0;
	u16_t j=0;
	/*����������*/
	u16_t remain=netbuf_get_remaining(nbr);/*ִ�д˾��ָ��λ��*/
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
ԭ�Ͷ��壺u16_t find_next_nes(u8_t receive_char,struct netbuf *nbr)
��������nb���ҵ���һ���ؼ���
������
u8_t receive_char��Ҫ���ҵ��ַ�
struct netbuf *nbr����Դ����nb
����ֵ������ҵ���������Ŀ���ַ�������ַ������Ҳ�������0
�㷨������ѭ�����ң���¼ָ���ƶ���λ��
/////////////////////////////////////////////// */
u16_t find_next_nes(u8_t receive_char,struct netbuf *nbr)
{
	u16_t order;
	u8_t find_flat=0;
	u16_t j=0;
	/*����������*/
	u16_t remain=netbuf_get_remaining(nbr);/*ִ�д˾��ָ��λ��*/
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
 * ��ȡָ���ַ�ǰ���ֶΣ�д����nb������Դnb��ָ���ַ�����δ�ҵ���Դnb�������ݶ���
 * ��nb�У���������
 */
/*///////////////////////////////////////////////
get_a_field_and_remove_it
ԭ�Ͷ��壺u8_t get_a_field_and_remove_it(struct netbuf *nbsrc,struct netbuf *nbnew,u8_t def_char)
��������nb���ҵ���һ���ֶβ��Ƴ�
������
struct netbuf *nbsrc����Դ����nb
struct netbuf *nbnew���Ƴ����ֶν�Ҫд���λ��
u8_t def_char��Ѱ�ҵ�Ŀ���ַ�
����ֵ���Ƿ�ɹ��ҵ��ֶβ�д�����µ�nb
�㷨�������ҵ��ַ�����д����nb���ض�ԭnb
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
ԭ�Ͷ��壺u8_t get_key_and_remove_it(struct netbuf *nbsrc,u8_t *key,u8_t max_len)
��������nb���ҵ��ؼ��ֲ��Ƴ�	
������
struct netbuf *nbsrc����Դ����nb
u8_t *key���Ƴ��Ĺؼ��ֽ�Ҫд���λ��
u8_t def_char��Ѱ�ҵ�Ŀ���ַ�
u8_t max_len��Ѱ�ҵ���Զ���룬��������Ϊ����ʧ��
����ֵ���Ƿ�ɹ��ҵ�
�㷨����ȡ�ؼ��֣�����������أ�����Դnb���Ⱥź�
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
	//u8_t *send_key=heap_alloc(equa_mark_lenth+1);  //�����ڴ�
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
ԭ�Ͷ��壺void add_nch_bef_str_in_nb(struct netbuf *nb,u8_t ch,u8_t tlen,u8_t *str)
��������nbд���ַ����������ַ���ǰ��ָ�����ȵ�����	
������
struct netbuf *nb��Ҫд���nb
u8_t ch����д�ַ���ǰ��������
u8_t tlen���ַ����벹�����ݵ��ܳ�
u8_t *str��Ҫд����ַ���
����ֵ����
�㷨������ռ䣬����д��
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





//�����ַ�ת������
/*///////////////////////////////////////////////
flt_str_to_float
ԭ�Ͷ��壺float flt_str_to_float(u8_t *str)
������ת�������ַ�����������	
������
u8_t *str��Ҫת�����ַ���
����ֵ��ת�ɵĸ�����
�㷨����С����ָ�ֱ��ȡ���������ֺ�С�����֣����������
	С������ѭ��/10ֱ��С��1�������������ֵõ�����ֵ
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
////������ת�ַ�����ת������s32_t���޵����ֻ����
/*///////////////////////////////////////////////
float_to_flt_str
ԭ�Ͷ��壺void float_to_flt_str(float num,u8_t *str)
������ת���������������ַ���	
������
float num��Ҫת���ĸ�����
u8_t *str��ת�ɵ��ַ���
����ֵ����
�㷨����С����ָ�ֱ��ȡ���������ֺ�С�����֣����������
	С������ѭ��/10ֱ��С��1�������������ֵõ�����ֵ
/////////////////////////////////////////////// */ 
void float_to_flt_str(float num,u8_t *str)
{
	if(num<0){
		strcat((char*)str,"-");		
		num *= -1;	//ת������
	}
	u8_t tmp[13];
	memset(tmp,0,13);
	u32_t int_part = (u32_t)num;	//ȡ����������
	u64_t multy_time=(u64_t)num;
	u64_t multy_num = multy_time;
	while(num>multy_num){
		num*=10;	//�Ŵ�ԭʼֵ��
		multy_time*=10;
		multy_num=(u64_t)num;
		if(multy_time == multy_num){	//С�����û����Ч����
			tmp[11]++;	//С�����0��������
		}		
#if defined(FORCE_DECI_TWO_NUM)		
		if(tmp[12]>=2){  	
			tmp[12]=20;
# else		
		if(tmp[12]>=10){	
			
#endif		
			break;
		}	
		tmp[12]++;	//С��λ������
//		if(multy_num>0xFFFFFFFFFFFF){
//			break;
//		}
	}
	u32_t deci_part = multy_num -multy_time;
#if defined(FORCE_DECI_TWO_NUM)	
	u8_t tail_num =0;
	if(tmp[12]==20){	//��Ҫ������һ������
//	if(deci_part>=100){
		tail_num = deci_part%10;
		deci_part -= tail_num;	//���β��
		if(tail_num>4){
			deci_part +=10;	//��������
			if(deci_part==10){
				tmp[11]--;
			}
		}
		if(deci_part>=100){	//��λ����С��λ������
			if(tmp[11]){
				tmp[11]--;
			}
		}
		deci_part /=10;	//�������һλС��
/*		if(deci_part>=10){	//0.009 ���ܲ�0
			deci_part /=10;	//0.098 ->��9
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
		
		if(deci_part>=100){	//С����λ������������
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


////������ת�ַ�����ת������s32_t���޵����ֻ����
/*///////////////////////////////////////////////
float_to_str
ԭ�Ͷ��壺void float_to_str(float num,u8_t *str,u8_t point)
������ת���������������ַ���	
������
float num��Ҫת���ĸ�����
u8_t *str��ת�ɵ��ַ���
u8_t point ��Ҫ������С����λ
����ֵ����
�㷨��
/////////////////////////////////////////////// */ 
void float_to_str(float num,u8_t *str,u8_t point)
{
    s32_t v_int;		//��������
    s32_t v_dot;		//С������
    float v_int_float;		//�������ָ�����ʽ
    float v_dot_float;		//С�����ָ�����ʽ
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
    
    v_dot_float += 1.0f;		//����ת������
    
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
		s_dot[0]='.';		//ת������
	}
	else
	{
	  	s_dot[0]=0;		//ת������
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
ԭ�Ͷ��壺u16_t get_unicode_str_len(void *buffer,u16_t max_len)
����������unicode�ַ����ĳ���
	����31003200330034000000 �򷵻�8
������
void *buffer��Ҫ���㳤�ȵ��ַ���λ��
u16_t max_len������޶�����
����ֵ���ַ�������
�㷨���ҵ���������0000ʱ��Ϊ�ַ�����ֹ������ָ���ƶ���λ��
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
ԭ�Ͷ��壺bool_t mem_to_nb_to_str(void *p,u16_t len,struct netbuf *nb)
���������ڴ��е����ݿ�ת�ɿɼ���ʮ�������ַ���д��nb.
������
void *p����ת��������
u16_t len����ת�������ݳ���
����ֵ��1ת���ɹ�   0ת��ʧ�ܡ�
�㷨��д��nb������mem_to_str������չnb������չnb�����滻ԭnb����
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

////////////��ѯ��ʷ�ϲ����ڵ�����(�ѱ����ǻ�δ����)ʱ�������´洢�����ݣ����򷵻ز�ѯ������
/*///////////////////////////////////////////////
get_real_num
ԭ�Ͷ��壺u32_t get_real_num(u32_t cur_pos,u16_t recv_num,u16_t circle_num)
��������Ҫ���ȡ�����ȡ�߼��ִ����.
������
u32_t cur_pos����ǰ�߼�������
u16_t recv_num��Ҫ��ȡ�ĺ���
u16_t circle_num��������뷶Χ�������Ͽɴ�Ŷ������ݣ�
����ֵ���ִ��߼����롣
�㷨���Ѹ��ǵ����ݻ���δ���������ݷ��ص�ǰ���ºţ����򷵻ض�ȡ����
/////////////////////////////////////////////// */
u32_t get_real_num(u32_t cur_pos,u16_t recv_num,u16_t circle_num)
{
	if(cur_pos<=recv_num){
		return cur_pos-1;		//��δ����������
	}
//	return (cur_pos-1-recv_num)/circle_num*circle_num+recv_num;	//2011.12.28 add -1	//���ص���ʵ�ʶ�Ӧλ��
	////�ѱ����ǵ����ݷ���cur_pos-1 ����recv_num
	return (cur_pos-1-recv_num >= circle_num)? cur_pos-1 : recv_num;
}

/*///////////////////////////////////////////////
num_judge_data_valid
ԭ�Ͷ��壺bool_t num_judge_data_valid(struct netbuf *nb)
�������ж��յ������ݰ��Ƿ���Ч.
������
struct netbuf *nb��Ҫ�жϵ����ݰ�
����ֵ��1 ��Ч  0 ��Ч��
�㷨�����ݰ���FF������������������֮����Ϊ��Ч������Ч
/////////////////////////////////////////////// */
/////////////////////////������Ч���жϺ���
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
ԭ�Ͷ��壺bool_t first_judge_data_valid(struct netbuf *nb)
������ͨ�����ݰ���һ���ֽ��Ƿ�ΪFF �ж����ݰ��Ƿ���Ч.
������
struct netbuf *nb��Ҫ�жϵ����ݰ�
����ֵ��1 ��Ч  0 ��Ч��
�㷨��
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

////�ֽڲ���
/*///////////////////////////////////////////////
byte_replen
ԭ�Ͷ��壺void byte_replen(u8_t *p,u8_t des_len)
���������ַ���ǰ��'0'���㳤��
������
u8_t *p��Ҫ������ַ���
u8_t des_len�����㳤��
����ֵ����
�㷨��д��netbuf��������ٶ���
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
//////////////////////////�ڴ��⺯��


/*///////////////////////////////////////////////
nb_span_add_char
ԭ�Ͷ��壺bool_t nb_span_add_char(struct netbuf *nb,u16_t span,u8_t des_char,u8_t order)
��������nb����ָ�������������
������
struct netbuf *nb��Ҫ�������ݵ�nb
u16_t span�����
u8_t des_char����������
u8_t order���Ƿ���ͷ�ϲ����һ������ 0 ����
����ֵ���Ƿ����ɹ�
�㷨������ռ䣬��ԭnb���ݺͲ�������д����nb����д��ԭnb
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
ԭ�Ͷ��壺revert_data(u8_t *p_src,u8_t len,u8_t *p_des)
������ָ�����ȵ��ڴ�鷭תд����һ���ڴ�
	�磺"12345"ת��"54321"��
������
u8_t *p_src��Դ�ڴ��
u8_t len����ת����
u8_t *p_des��Ŀ���ڴ��
����ֵ����
�㷨��
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
ԭ�Ͷ��壺void s_str_to_mem(u8_t *src,u8_t src_len,u8_t *des)
�������ڴ��������ָ�����ȵ�ʮ�������ַ�ת���ڴ��е�HEX��ʽ.
������
u8_t *src����ת��������
u8_t *des��ת�ɵ�����
����ֵ����
�㷨���޶�������ѭ���������ϲ�Դ�ڴ���ֽں�д���µ��ڴ��
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
ԭ�Ͷ��壺void s_str_to_mem(u8_t *src,u8_t src_len,u8_t *des)
�������ڴ��������ָ�����ȵ��ڴ��е�HEX����ת��ʮ�������ַ�.
������
u8_t *src����ת��������
u8_t *des��ת�ɵ�����
����ֵ����
�㷨���޶�������ѭ�����������Դ�ڴ���ֽں�д���µ��ڴ��
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
/////////////116��18.007402'
/*///////////////////////////////////////////////
get_gps_protocol_value
ԭ�Ͷ��壺u32_t get_gps_protocol_value(u8_t *v_str,u8_t dirct)
�������Ӿ�γ���ַ��������ض�Э���u32_t����.
������
u8_t *v_str����ת���ַ���
u8_t dirct�������ʶ
����ֵ��ת�ɵ�long����
�㷨����Э��Ҫ����λ�ϲ�
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
	value=atoi((char const*)tmp);	//�Ƚ���
	len -= i+2;	//ʣ���ַ�������
	v_str += i+2;	//�ַ���ͷǰ��
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
	value+= atoi((char const*)tmp)<<8;	//�ֽ���
	len -= i+1;
	v_str += i+1;	//�ַ���ͷǰ��
	for(i=0;i<len;i++){
		if(v_str[i]!='0'){
			break;
		}
	}
	if(i<len){
		value+=(u32_t)i <<29;
		v_str += i;	//�ַ���ͷǰ������Ч������ʼ��
	}
	len=strlen((char const*)v_str);
	if(len>4){	//��Ч���ָ�������4��
		v_str[4]=0;
	}
//	value_t = atoi(v_str);
//	while(value_t>0x3FFF){
//		value_t/=10;
//	}
	value+= (u32_t)(atoi((char const*)v_str)&0x3FFF)<<14;	//��С������
//	value+= (value_t&0x3FFF)<<14;
	if(dirct){
		value+= 0x10000000;
	}
	return value;
}
/*///////////////////////////////////////////////
get_gps_protocol_str
ԭ�Ͷ��壺void get_gps_protocol_str(u32_t value,u8_t *v_str)
���������ض�Э���u32_t�������ɾ�γ���ַ���.
������
u32_t value����ת������
u8_t *v_str��ת�ɵľ�γ���ַ���
����ֵ����
�㷨����λ�ֽ�ɸ����ֶ�
/////////////////////////////////////////////// */
void get_gps_protocol_str(u32_t value,u8_t *v_str)
{
	v_str[0]=0;
	u16_t tmp_v=0;
	u8_t tmp[10];
	memset(tmp,0,10);
	tmp_v=value&0xFF;	//ȡ����
	value>>=8;		//�Ƴ���
	int_to_str(tmp_v,tmp);
	strcat((char*)v_str,(char const*)tmp);	//д���
	strcat((char*)v_str,"��");
	tmp_v=value&0x3F;	//ȡ����
	value>>=6;		//�Ƴ���
	memset(tmp,0,10);
	int_to_str(tmp_v,tmp);	
	strcat((char*)v_str,(char const*)tmp);	//д���
	strcat((char*)v_str,".");
	tmp_v=value&0x3FFF;	//ȡ����С��
	int_to_str(tmp_v,tmp);	
	value>>=15;		//�Ƴ���С���ͷ�λ��־
	while(value--){
		strcat((char*)v_str,"0");
	}
	strcat((char*)v_str,(char const*)tmp);	//д���С��
	strcat((char*)v_str,"'");
}



/*///////////////////////////////////////////////
get_byte_value
ԭ�Ͷ��壺bool_t get_byte_value(u8_t *intvalue, u8_t *strSource, u8_t len)
��������ָ�����ȵ������ַ���ת������.
������
u8_t *intvalue��ת������������λ��
u8_t *strSource����ת���ַ���
u8_t len��Ҫת���ĳ���
����ֵ���Ƿ�ת���ɹ�  0 �ɹ�  1ʧ��
�㷨������ֵ�õ���ֵ��ѭ��*10���
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
ԭ�Ͷ��壺u8_t byte_to_3char(u8_t srcint,u8_t *des)
��������һ��u8_t ����ת����λ�����ַ���.  ����0xFF=>"255"
������
u8_t srcint����ת������
u8_t *des��ת�ɵ��ַ���
����ֵ��0����ת���ɹ�
�㷨��ȡ�ø�λ��ʮλ����λ��ֵ��ת�����ַ���д������
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
ԭ�Ͷ��壺int bytebuf_3char(struct netbuf *nbbuf, struct netbuf *nb)
��������һ��netbuf�е�ÿ������ת����λ�����ַ���д����һ��netbuf
������
struct netbuf *nbbuf������netbuf
struct netbuf *nb:д�ַ���netbuf
����ֵ��0����ת���ɹ�
�㷨��ÿ������ת����д��
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
ԭ�Ͷ��壺int char_bytebuf(struct netbuf *nb, struct netbuf *nbbuf)
��������һ��netbuf�е�ÿ��λ�����ַ���ת�ɵ��ֽ�����д����һ��netbuf
	bytebuf_3char��������
������
struct netbuf *nb�����ַ���netbuf
struct netbuf *nbbuf:д��netbuf
����ֵ��0����ת���ɹ�  1����ת��ʧ��
�㷨��ÿ������ת����д��
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
ԭ�Ͷ��壺void byte_seq_arrange(void *src,u8_t len)
��������ָ�����ȵ��ڴ���������ֽڵ���
������
void *src�������ڴ��
u8_t len�����򳤶�
����ֵ��0����ת���ɹ�
�㷨��
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
ԭ�Ͷ��壺u8_t judge_ff_80_error(struct netbuf *nb)
�������������ݰ�ͷ10���ֽ���FF��80�����ж��յ������ݰ��Ƿ���Ч.
������
struct netbuf *nb��Ҫ�жϵ����ݰ�
����ֵ��0��Ч  FF �� 80 ��Ч��
�㷨�����ݰ�ǰ10���ֽ���FF��80������������֮����Ϊ��Ч������Ч
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
	if(find_ff >=3){		//��������ռ������������֮������
		return 0xFF;
	}
	if(find_80 >=3){		//��������ռ������������֮������
		return 0x80;
	}
	return 0;
}




