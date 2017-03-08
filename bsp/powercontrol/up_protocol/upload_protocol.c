#include "upload_protocol.h"




char makeup_msg_pkg(struct devinfo *dinfo)
{
	unsigned short checksum;
	
	
	pkg_head_add();        //添加包头
	
	make_msg_body_pkg();   //封装数据体
	
	checksum = crc_calc(); //计算检验和
	
	pkg_checksum_add();	   //添加检验和
	
	pkg_tail_add();        //添加包尾

	return UP_PROTOCOL_OK;
}
