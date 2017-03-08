/*--------------------------------------------------------------------------------------------------

                  ��Ȩ���� (C), 2015-2020, ������άӯ�����޹�˾

 -----------------------------------------------------------------------------------
  �� �� ��   : at_para.c
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
#include "config.h"
#include "finsh.h"
#include "uart_nb.h"
#include "para_ncom.h"
#include "paralib.h"
#include "at_para.h"

uart_device para_uart_device;

#define	UART_RELEASE		(1)


void para_recv_explain(void)
{
//	u16_t lenth;
	struct netbuf *nb = para_uart_device.write_nb;
	netbuf_reset(nb);
	
	config_read(para_uart_device.read_nb,nb);
	netbuf_set_pos_to_start(nb);
	_serial_poll_tx_nb(para_uart_device.device,nb,netbuf_get_extent(nb));
	netbuf_reset(nb);
}

void para_uart_recv_handle(void)
{
	int len = 0,tol_len = 0;
	netbuf_reset(para_uart_device.read_nb);
	do
	{
		len = _serial_int_rx_nb(para_uart_device.device, para_uart_device.read_nb, PARA_UART_BUFFER_SIZE - tol_len);
		if(len < 1)
			break;
		tol_len += len;
		rt_thread_delay(4);
	}while(len > 0 );
	if(tol_len == 0)
		return ;
	
	para_recv_explain();
	netbuf_reset(para_uart_device.read_nb);
}


static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
	if(para_uart_device.sem_uart_recv.value == 0 && size > 0)
	{
		rt_sem_release(&(para_uart_device.sem_uart_recv));
	}
	return RT_EOK;
}
/*********************************************
 * para���ڳ�ʼ��
 * ��ʼ��device
 * �����ź���
 *********************************************/
u8_t at_para_trans_init(void)
{
	int err;
	rt_device_t device;
	memset(&para_uart_device,0,sizeof(uart_device));
	device = rt_device_find(PARA_UART_DEVICE);
	
	if(device == RT_NULL){//���ش����߳��˳�
		PARA_DEBUG(UART_RELEASE,("uart==>Usart not find\n"));
		return 1;
	}
	para_uart_device.device = device;
	
	para_uart_device.read_nb = netbuf_alloc();
	if(para_uart_device.read_nb == RT_NULL){//���ش����߳��˳�
		PARA_DEBUG(UART_RELEASE,("uart==>read nb alloc fail\n"));
		return 1;
	}
	
	para_uart_device.write_nb = netbuf_alloc();
	if(para_uart_device.write_nb == RT_NULL){//���ش����߳��˳�
		PARA_DEBUG(UART_RELEASE,("uart==>write nb alloc fail\n"));
		return 1;
	}
//	para_uart_device.read_buf = rt_malloc(PARA_UART_BUFFER_SIZE);
//	if(para_uart_device.read_buf == RT_NULL){//���ش����߳��˳�
//		PARA_DEBUG(UART_RELEASE,("uart==>Buffer alloc fail\n"));
//		return 1;
//	}
//	para_uart_device.write_buf = rt_malloc(PARA_UART_BUFFER_SIZE);
//	if(para_uart_device.write_buf == RT_NULL){//���ش����߳��˳�
//		PARA_DEBUG(UART_RELEASE,("uart==>Buffer alloc fail\n"));
//		return 1;
//	}	
	
	err = rt_device_open(device, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
	if(err != RT_EOK)
	{
		PARA_DEBUG(UART_RELEASE,("uart==>Data Usart Open error %d\n",err));
		return 1;
	}
	err = rt_sem_init(&(para_uart_device.sem_uart_recv), SEM_NAME_PARA_UART, 0, RT_IPC_FLAG_FIFO);
	if (err != RT_EOK)
	{
		PARA_DEBUG(UART_RELEASE,("uart==>init sem error %d\n",err));
		return 1;
	}
//	rt_device_set_rx_indicate(para_uart_device.device, uart_rx_ind);
	
	return 0;
}


/********************************ly**********************************/
void para_uart_enable(void)
{
	//�ӹ�PARA_DEVICE�Ľ��ս���
	rt_device_set_rx_indicate(para_uart_device.device, uart_rx_ind);
}
FINSH_FUNCTION_EXPORT_ALIAS(para_uart_enable,Para_out,Para_out)
/********************************ly**********************************/

/*********************************************
 * �߳���
 * ��ʼ�����ƽṹ
 * ��ѯ�ź�
 *********************************************/
static void rt_thread_entry_at_para(void *parameter)
{
	int err;
	device_fv_report_init();
	at_para_trans_init();	      //�������ݽṹ��ʼ��	
	while(1){
		err = rt_sem_take(&para_uart_device.sem_uart_recv, RT_WAITING_FOREVER);
		if (err == RT_EOK){
			para_uart_recv_handle();
		}
	}
}


/*********************************************
 * ��ʼ��at_para�����߳�
 * ��ʼ��wifi_ct_ctrl�е�uart_wifi
 * �����߳�
 *********************************************/

void start_at_para_process(void)
{
	ALIGN(RT_ALIGN_SIZE)
	static char thread_at_para_stack[1024];	//�߳�ջ
	static struct rt_thread thread_at_para;	//�߳�ָ��
//	flash_read(NULL,(uint32_t *)&para,sizeof(struct para_ncom));	//��ȡ����
	rt_thread_init(&thread_at_para,
                   "at_para",
                   rt_thread_entry_at_para,
                   RT_NULL,
                   &thread_at_para_stack[0],
                   sizeof(thread_at_para_stack),THREAD_RPIORITY_PARA,15);
	rt_thread_startup(&thread_at_para);
	
}
