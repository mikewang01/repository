
/*
 FILE NAME: netpage.h
 ORG_CODE:	ubicom
 ORG_DATE:	2002/07/29 20:45:06 
 ORG_VER:		1.5
 MOD_CODE:	@ZKJ
 MOD_DATE:  2015/11/24
 */

#ifndef __NETPAGE_H__
#define __NETPAGE_H__

#include "type_def.h"


//--------MACRO FOR PORTING-----------


#define NETPAGE_MALLOC_MEM  rt_malloc	//
#define NETPAGE_FREE_MEM    rt_free	//

//--------CONFIGURATION---------------
#define NETPAGE_SIZE  0x0080					//但页容量
#define NETPAGE_MASK  0x7F						//页容量掩码
#define NETPAGE_SHIFT 7								//位移位数
#define NETPAGES 		  0x80						//总页数  总容量为：NETPAGES* NETPAGE_SIZE = 0x80*0x80 = 16K
#define NETPAGE_NULL  0x80
#define NETPAGE_LAST (NETPAGES-1) 



//------COMMON DECLARATION------------
typedef u8_t netpage_t;
typedef u8_t netpage_offs_t;

extern u8_t netpage_init(void);
extern netpage_t netpage_get_free(void);
extern netpage_t netpage_get_low_water(void);
extern netpage_t netpage_alloc(void);
extern ref_t netpage_ref(netpage_t page);
extern ref_t netpage_deref(netpage_t page);
extern ref_t netpage_get_refs(netpage_t page);
extern u8_t netpage_lowlevel_init(void);
extern u8_t netpage_read_u8(netpage_t pg, netpage_offs_t offs);
extern u16_t netpage_read_u16(netpage_t pg, netpage_offs_t offs);

extern void netpage_write_u8(netpage_t pg, netpage_offs_t offs, u8_t v);
extern void netpage_write_u16(netpage_t pg, netpage_offs_t offs, u16_t v);
extern void netpage_copy_u16(netpage_t dest_pg, netpage_offs_t dest_offs, netpage_t src_pg, netpage_offs_t src_offs);
extern void netpage_copy(netpage_t dest, netpage_t src);
extern void netpage_fill_u8(netpage_t pg, netpage_offs_t offs, addr_t size, u8_t v);
extern u16_t netpage_crc16(u16_t crc16, netpage_t pg, netpage_offs_t offs, u16_t count);
extern u32_t netpage_crc32(u32_t crc32, netpage_t pg, netpage_offs_t offs, u16_t count);

extern u16_t byteswap16(u16_t v);
extern u32_t byteswap32(u32_t v);
extern u64_t byteswap64(u64_t v);
extern u16_t arch_u16_to_be_u16(u16_t v);
extern u32_t arch_u32_to_le_u32(u32_t v);
extern u64_t arch_u64_to_le_u64(u64_t v);
extern u16_t be_u16_to_arch_u16(u16_t v);
extern u32_t be_u32_to_arch_u32(u32_t v);
extern u64_t be_u64_to_arch_u64(u64_t v);
extern u16_t le_u16_to_arch_u16(u16_t v);
extern u32_t le_u32_to_arch_u32(u32_t v);
extern u64_t le_u64_to_arch_u64(u64_t v);
extern u32_t arch_u32_to_be_u32(u32_t v);
extern u64_t arch_u64_to_be_u64(u64_t v);
extern u16_t arch_u16_to_le_u16(u16_t v);

#endif
