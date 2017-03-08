#include "upload_protocol.h"




char makeup_msg_pkg(struct devinfo *dinfo)
{
	unsigned short checksum;
	
	
	pkg_head_add();        //��Ӱ�ͷ
	
	make_msg_body_pkg();   //��װ������
	
	checksum = crc_calc(); //��������
	
	pkg_checksum_add();	   //��Ӽ����
	
	pkg_tail_add();        //��Ӱ�β

	return UP_PROTOCOL_OK;
}
