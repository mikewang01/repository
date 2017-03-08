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
#include <rthw.h>

#include "uart_nb.h"


/*
 * Serial interrupt routines
 */
int _serial_int_rx_nb(rt_device_t device, struct netbuf *nb, int length)
{
	int size;
	struct rt_serial_rx_fifo* rx_fifo;
	struct rt_serial_device *serial = (struct rt_serial_device *)device;
	RT_ASSERT(serial != RT_NULL);
	size = length; 


	rx_fifo = (struct rt_serial_rx_fifo*) serial->serial_rx;
	RT_ASSERT(rx_fifo != RT_NULL);
	netbuf_set_pos_to_end(nb);
	/* read from software FIFO */
	while (length)
	{
		int ch;
		rt_base_t level;

		/* disable interrupt */
		level = rt_hw_interrupt_disable();
		if (rx_fifo->get_index != rx_fifo->put_index)
		{
			ch = rx_fifo->buffer[rx_fifo->get_index];
			rx_fifo->get_index += 1;
			if (rx_fifo->get_index >= serial->config.bufsz) rx_fifo->get_index = 0;
		}
		else
		{
			/* no data, enable interrupt and break out */
			rt_hw_interrupt_enable(level);
			break;
		}

		/* enable interrupt */
		rt_hw_interrupt_enable(level);

		if(!netbuf_get_remaining(nb)){
			if(!netbuf_fwd_make_space(nb,128))	//一次占用一整个netpage页
				break;
		}
		netbuf_fwd_write_u8(nb,ch & 0xff);
		//        *data = ch & 0xff;
		//        data ++; 
		length --;
	}
	netbuf_set_end_to_pos(nb);
	netbuf_set_pos_to_start(nb);
	return size - length;
}


int _serial_poll_tx_nb(rt_device_t device, struct netbuf *nb, int length)
{
    int size;
	struct rt_serial_device *serial = (struct rt_serial_device *)device;
    RT_ASSERT(serial != RT_NULL);

    size = length;
    while (length && netbuf_get_remaining(nb))
    {
        /*
         * to be polite with serial console add a line feed
         * to the carriage return character
         */
//        if (*data == '\n' && (serial->parent.open_flag & RT_DEVICE_FLAG_STREAM))
//        {
//            serial->ops->putc(serial, '\r');
//        }
//    
        serial->ops->putc(serial,netbuf_fwd_read_u8(nb));
    
//        ++ data;
        -- length;
    }

    return size - length;
}



int _serial_int_tx_nb(rt_device_t device, struct netbuf *nb, int length)
{
    int size;
    struct rt_serial_tx_fifo *tx;
    struct rt_serial_device *serial = (struct rt_serial_device *)device;
    RT_ASSERT(serial != RT_NULL);

    size = length;
    tx = (struct rt_serial_tx_fifo*) serial->serial_tx;
    RT_ASSERT(tx != RT_NULL);

    while (length && netbuf_get_remaining(nb))
    {
        if (serial->ops->putc(serial, netbuf_fwd_read_u8(nb)) == -1)
        {
            rt_completion_wait(&(tx->completion), RT_WAITING_FOREVER);
            continue;
        }

//        data ++; 
	  length --;
    }

    return size - length;
}


