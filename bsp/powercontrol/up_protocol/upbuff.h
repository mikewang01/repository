/*******************************************************************************
File name  : upbuff.h
Description: 
*******************************************************************************/
#ifndef _UPBUF_H_
#define _UPBUF_H_

struct upbuf
{
	char buff[112];
};

extern void uart_tx_buff_add_n(struct uart_port *port, char *buff, unsigned int count);
extern void uart_tx_buff_add_u32(struct uart_port *port, unsigned int data);
extern void uart_tx_buff_add_32u(struct uart_port *port, unsigned int data);
extern void uart_tx_buff_add_u16(struct uart_port *port, unsigned int data);
extern void uart_tx_buff_add_16u(struct uart_port *port, unsigned int data);
extern void uart_tx_buff_add_u8(struct uart_port *port, unsigned int data);

#endif/* _UPBUFF_H_ */
