/*--------------------------------------------------------------------------------------------------

                  版权所有 (C), 2015-2020, 北京万维盈创有限公司

 -----------------------------------------------------------------------------------
  文 件 名   : at_para.c
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
 * para串口初始化
 * 初始化device
 * 创建信号量
 *********************************************/
u8_t at_para_trans_init(void)
{
	int err;
	rt_device_t device;
	memset(&para_uart_device,0,sizeof(uart_device));
	device = rt_device_find(PARA_UART_DEVICE);
	
	if(device == RT_NULL){//严重错误，线程退出
		PARA_DEBUG(UART_RELEASE,("uart==>Usart not find\n"));
		return 1;
	}
	para_uart_device.device = device;
	
	para_uart_device.read_nb = netbuf_alloc();
	if(para_uart_device.read_nb == RT_NULL){//严重错误，线程退出
		PARA_DEBUG(UART_RELEASE,("uart==>read nb alloc fail\n"));
		return 1;
	}
	
	para_uart_device.write_nb = netbuf_alloc();
	if(para_uart_device.write_nb == RT_NULL){//严重错误，线程退出
		PARA_DEBUG(UART_RELEASE,("uart==>write nb alloc fail\n"));
		return 1;
	}
//	para_uart_device.read_buf = rt_malloc(PARA_UART_BUFFER_SIZE);
//	if(para_uart_device.read_buf == RT_NULL){//严重错误，线程退出
//		PARA_DEBUG(UART_RELEASE,("uart==>Buffer alloc fail\n"));
//		return 1;
//	}
//	para_uart_device.write_buf = rt_malloc(PARA_UART_BUFFER_SIZE);
//	if(para_uart_device.write_buf == RT_NULL){//严重错误，线程退出
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
	//接管PARA_DEVICE的接收解析
	rt_device_set_rx_indicate(para_uart_device.device, uart_rx_ind);
}
FINSH_FUNCTION_EXPORT_ALIAS(para_uart_enable,Para_out,Para_out)
/********************************ly**********************************/

/*********************************************
 * 线程体
 * 初始化控制结构
 * 轮询信号
 *********************************************/
static void rt_thread_entry_at_para(void *parameter)
{
	int err;
	device_fv_report_init();
	at_para_trans_init();	      //数据数据结构初始化	
	while(1){
		err = rt_sem_take(&para_uart_device.sem_uart_recv, RT_WAITING_FOREVER);
		if (err == RT_EOK){
			para_uart_recv_handle();
		}
	}
}


/*********************************************
 * 初始化at_para处理线程
 * 初始化wifi_ct_ctrl中的uart_wifi
 * 启动线程
 *********************************************/

void start_at_para_process(void)
{
	ALIGN(RT_ALIGN_SIZE)
	static char thread_at_para_stack[1024];	//线程栈
	static struct rt_thread thread_at_para;	//线程指针
//	flash_read(NULL,(uint32_t *)&para,sizeof(struct para_ncom));	//读取参数
	rt_thread_init(&thread_at_para,
                   "at_para",
                   rt_thread_entry_at_para,
                   RT_NULL,
                   &thread_at_para_stack[0],
                   sizeof(thread_at_para_stack),THREAD_RPIORITY_PARA,15);
	rt_thread_startup(&thread_at_para);
	
}
