/*******************************************************************************
File name  : upbuff.c
Description: 
*******************************************************************************/
#include "../drivers/uart.h"

#include "upbuff.h"

void uart_tx_buff_add_n(struct uart_port *port, char *buff, unsigned int count)
{
	unsigned int i;

	for (i=0; i<count; i++)
	{
		port->tx_buff[port->tx_len] = *(buff + i);
		port->tx_len++;
	}
}

void uart_tx_buff_add_u32(struct uart_port *port, unsigned int data)
{
	*(unsigned int*)(port->tx_buff + port->tx_len) = data;
	port->tx_len += 4;
}

void uart_tx_buff_add_32u(struct uart_port *port, unsigned int data)
{
	char i;
	char *p = (char *)(&data) + 3;

	for (i =0; i < 4; i++)
	{
		*(port->tx_buff + port->tx_len) = *p--;
		port->tx_len += 1;
	}
}

void uart_tx_buff_add_u16(struct uart_port *port, unsigned int data)
{
	*(unsigned short*)(port->tx_buff + port->tx_len) = data;
	port->tx_len += 2;
}

void uart_tx_buff_add_16u(struct uart_port *port, unsigned int data)
{
	*(unsigned char*)(port->tx_buff + port->tx_len) = (char)(data >> 8);
	port->tx_len += 1;
	*(unsigned char*)(port->tx_buff + port->tx_len) = (char)data;
	port->tx_len += 1;
}

void uart_tx_buff_add_u8(struct uart_port *port, unsigned int data)
{
	*(unsigned char*)(port->tx_buff) = data;
	port->tx_len += 1;
}
