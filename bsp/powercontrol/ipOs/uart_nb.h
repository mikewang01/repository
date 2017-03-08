/*--------------------------------------------------------------------------------------------------

                  ��Ȩ���� (C), 2015-2020, ������άӯ�����޹�˾

 -----------------------------------------------------------------------------------
  �� �� ��   : uart_nb.h
  �� �� ��   : ����
  ��    ��   : ����
  ��������   : 2016��7��13��
  ����޸�   :
  ��������   : ���д���ʹ��netbuf����ͷ�ļ�
  �޸���ʷ   :
  1.��    ��   : 2016��7��13��
    ��    ��   : ����
    �޸�����   : �����ļ�
  

----------------------------------------------------------------------------------------------------*/
#ifndef		__UART_NB_H__
#define		__UART_NB_H__

#include "config.h"
#include <rtdevice.h>

int _serial_int_rx_nb(rt_device_t device, struct netbuf *nb, int length);
int _serial_int_tx_nb(rt_device_t device, struct netbuf *nb, int length);
int _serial_poll_tx_nb(rt_device_t device, struct netbuf *nb, int length);
typedef struct 
{
	rt_device_t device;               //���ڵ�������
	struct rt_semaphore sem_uart_recv;//�����ź���
	char * read_buf;									//���ڽ��ջ�����
	char * write_buf;									//���ͻ�����
	struct netbuf *read_nb;		
	struct netbuf *write_nb;
}uart_device;

typedef uart_device* uart_device_t;



#endif 

