#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include <rtthread.h>
//#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//STM32 FLASH ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 512 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
 

rt_uint16_t STMFLASH_ReadHalfWord(rt_uint32_t faddr);		  //��������  
void STMFLASH_WriteLenByte(rt_uint32_t WriteAddr,rt_uint32_t DataToWrite,rt_uint16_t Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
rt_uint32_t STMFLASH_ReadLenByte(rt_uint32_t ReadAddr,rt_uint16_t Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(rt_uint32_t WriteAddr,rt_uint16_t *pBuffer,rt_uint16_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(rt_uint32_t ReadAddr,rt_uint16_t *pBuffer,rt_uint16_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(rt_uint32_t WriteAddr,rt_uint16_t WriteData);								   
#endif

















