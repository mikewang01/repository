/*******************************************************************************
File name  : up_protocol.h
Description: 
*******************************************************************************/
#ifndef _UP_PROTOCOL_H_
#define _UP_PROTOCOL_H_

//#include "../devinfo.h"
//#include "../drivers/uart.h"

#define DATA_HEAD	"WH"
#define DATA_TAIL	"WT"

extern char make_up_msg_pkg(struct uart_port *port, unsigned char cmd_type, struct devinfo *dinfo);

#endif/* _UP_PROTOCOL_H_ */
