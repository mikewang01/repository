/*************************************************************************
 *
 * customlib.h : header file
 * Version : 1.0
 * Compiling Environment : IAR4.21
 * Date : 2003.11.02
 * Author : Cheng YangJun��Xie YongSheng
 *
 **************************************************************************
 * Confidential and Proprietary unpublished information of warewin
 * Information Technology Co.Ltd.
 * Copyright 2003, warewin Information Technology Co.Ltd.
 * All rights reserved.
 **************************************************************************
 *
 * File Description : ͨ���Զ��庯��������������ú�����������ģ����á�
 * 
 * Modify Information : No modification.
 *
**************************************************************************/
#ifndef __CUSTOMLIB_H__
#define __CUSTOMLIB_H__

#include "config.h"

#define ORCHECKSUM 0	//�����У���
#define PLUSCHECKSUM 1	//�ӷ�����У���

#if !defined(MAX_SMS_NUM)
	#define MAX_SMS_NUM 2
#endif


#define COMMA 0x2c	//����","
#define COMMB ':'

#define DES_AFTER_DATA 1
#define DATA_AFTER_DES 0

#define ALL_STATES 0xF0

void float_data_filter_sort(float *buff, int len);//����С����˳��
void get_filte_float_data(float r_value,float *fifo,float *sort,u8_t *cur_pos,float *des,int len);


/*****************************************************************
* function description:  change a string to a int value
*
* parameter:  *shortvalue - the value of int para
*             *strSource - the string
*             len  - the length of the string 
* return value: 0 - success
*               1 - fail
******************************************************************/
extern bool_t get_int_value(u16_t *longvalue, u8_t *strSource, u8_t len);

/*****************************************************************
* function description:  change a string to a u32_t value
*
* parameter:  *longvalue - the value of u32_t para
*             *strSource - the string
*             len  - the length of the string 
* return value: 0 - success
*               1 - fail
******************************************************************/
extern bool_t getlongvalue(u32_t *longvalue, u8_t *strSource, u8_t len);


/*������һ���ֶεĳ���*/
extern u16_t line_next_f_len(struct netbuf *nb,u8_t separator);
extern void line_jump_n_f(struct netbuf *nb,u8_t separator,u8_t n);


/*
int_to_str()
ת���������ַ���
*/ 
extern u8_t int_to_str(u32_t srcint,u8_t *s);
u8_t int64_to_str(u64_t srcint,u8_t *des);
/*
int_to_str16()
ת���������ַ���
*/ 
extern u8_t int_to_str16(u32_t srcint,u8_t *s,u8_t len);




//�ַ���ת��Ϊ��������16�����ַ���ת��Ϊ10������
u64_t  str_to_u64(const char *s,u8_t base );


//���ڴ���ת��Ϊʮ�������ַ�����ʽ����"ABC"ת��Ϊ"414243"
bool_t mem_to_str(struct netbuf *nbsrc,struct netbuf *nbdes);
void value_mem_to_str(u16_t value,u8_t *str);
//��ʮ�������ַ���ת��Ϊ�ڴ�����ʽ����"414243"ת��Ϊ"ABC"
bool_t str_to_mem(struct netbuf *nbsrc,struct netbuf *nbdes);

//��һ���ַ���д��nb����
void nb_strcat(struct netbuf *nb,u8_t *str);

//��һ���ַ���д��nb����(Unicode��ʽ)---2004.02.09����
void nb_strcat_unicode(struct netbuf *nb,u8_t *str);



void get_an_field(u8_t *temp,struct netbuf *nb,u16_t separator);

u32_t get_an_int_field(struct netbuf *nb,u8_t base,u8_t separator);

u32_t get_an_ip_field(struct netbuf *nb,u8_t separator);


/*
 * itoa()
 *	Convert a number to a string
 */
char *itoaEX(char *obuf, u64_t x, unsigned int base, u8_t issigned);


//��UNICODE unicodes����ת��Ϊһ������ 30 00 30 00 31 00 32 00=>12  2007.01.14
u16_t wordsToInt(u8_t len,u8_t *bytes);

//��һ������ת��ΪUNICODE unicodes���� 12=>30 00 30 00 31 00 32 00 2007.01.14
void intToWords(u16_t value, u8_t len,u8_t *bytes);

u32_t get_u32_t_from_bytes(u8_t *bytes);
u32_t get_u16_t_from_bytes(u8_t *bytes);
void set_bytes_from_uint(u8_t *bytes,u32_t value,u8_t len);
u8_t mem_calc_checksum(u8_t *bytes,u16_t blen,u8_t cstype);


u8_t get_key_and_remove_it(struct netbuf *nbsrc,u8_t *key,u8_t max_len);

u8_t get_a_field_and_remove_it(struct netbuf *nbsrc,struct netbuf *nbnew,u8_t def_char);
u8_t generate_a_field(struct netbuf *nbs,u8_t *key_str,u8_t *value_str,u8_t addendchar,u8_t dirhi);
void generate_a_int_field(struct netbuf *nbs,u8_t *key_str,u32_t value,u8_t addendchar,u8_t dirhi);

u16_t find_next_nes(u8_t receive_char,struct netbuf *nbr);
s16_t s16_find_next_nes(u8_t receive_char,struct netbuf *nbr);
void nb_add_re(struct netbuf *nbr);



//�����ַ�ת������
float flt_str_to_float(u8_t *str);
void float_to_flt_str(float num,u8_t *str);
void float_to_str(float num,u8_t *str,u8_t point);


/*///////////////////////////////////////////////
hex_to_asc
ԭ�Ͷ��壺u8_t hex_to_asc(u8_t hex,u8_t* str)
������ת��ʮ������ΪASC��
������
u8_t hex��Ҫת����HEX
����ֵ��HEXת��Ϊ2���ַ�
�㷨�� 
/////////////////////////////////////////////// */ 
void hex_to_asc(u8_t hex,u8_t* str);

u8_t bcd_to_hex(u8_t bcd);
u8_t hex_to_bcd(u8_t hex);

u16_t get_unicode_str_len(void *buffer,u16_t max_len);

//��һ���ڴ�ת���ַ���д��nb���
bool_t mem_to_nb_to_str(void *p,u16_t len,struct netbuf *nb);	//2009.01.12

u32_t get_real_num(u32_t cur_pos,u16_t recv_num,u16_t circle_num);

u16_t crc_checksum(struct netbuf *nb,u16_t len);	////2009.05.06

bool_t num_judge_data_valid(struct netbuf *nb);
bool_t first_judge_data_valid(struct netbuf *nb);

void byte_replen(u8_t *p,u8_t des_len);
void free_heap_str_to_nb(struct netbuf *nb);

//ÿ��һ�ξ���д��ָ���ַ����м��Կո����
bool_t nb_span_add_char(struct netbuf *nb,u16_t span,u8_t des_char,u8_t order);

void one_buffer_to_another(void *buffer_src,void *buffer_des,u16_t lenth);
//u16_t nb_calc_16crc(struct netbuf *nb);
//u32_t nb_calc_32crc(struct netbuf *nb);

void revert_data(u8_t *p_src,u8_t len,u8_t *p_des);
//void nb_add_yanshan_crc(struct netbuf *nb);
//u16_t yanshan_crc_chk(struct netbuf *nb);

void s_str_to_mem(u8_t *src,u8_t src_len,u8_t *des);
void s_mem_to_str(u8_t *src,u8_t src_len,u8_t *des);


u32_t get_gps_protocol_value(u8_t *v_str,u8_t dirct);
void get_gps_protocol_str(u32_t value,u8_t *v_str);

u32_t str_to_u32(u8_t *s,u8_t base);
bool_t get_byte_value(u8_t *intvalue, u8_t *strSource, u8_t len);

u8_t byte_to_3char(u8_t srcint,u8_t *des);
int bytebuf_3char(struct netbuf *nbbuf, struct netbuf *nb);
int char_bytebuf(struct netbuf *nb, struct netbuf *nbbuf);

void byte_seq_arrange(void *src,u8_t len);
u8_t judge_ff_80_error(struct netbuf *nb);
u16_t hj212_crc(struct netbuf *nb);
u16_t calc_checksum_4bytes(struct netbuf *nb,u8_t cstype);
u8_t byte_reverse(u8_t data);


extern u16_t general_crc16_modbus(u8_t* in_str,u16_t in_len);
u16_t calc_nb_modbus_crc(struct netbuf *nb);

u8_t common_modbus_data_valid(struct netbuf *nb);
#endif


