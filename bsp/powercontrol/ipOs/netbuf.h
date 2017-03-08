/*
 * netbuf.h
 *	Network buffer information.
 *
 * Copyright ?2000, 2001, 2002, 2003 Ubicom Inc. <www.ubicom.com>.  All rights reserved.
 *
 * This file contains confidential information of Ubicom, Inc. and your use of
 * this file is subject to the Ubicom Software License Agreement distributed with
 * this file. If you are uncertain whether you are an authorized user or to report
 * any unauthorized use, please contact Ubicom, Inc. at +1-650-210-1500.
 * Unauthorized reproduction or distribution of this file is subject to civil and
 * criminal penalties.
 *
 * $RCSfile: netbuf.h,v $
 * $Date: 2003/07/03 00:28:48 $
 * $Revision: 1.70 $
 */
#ifndef __NETBUF_H__
#define __NETBUF_H__


//#include "type_def.h"
#include "netpage.h"


/*
 * Define the netpage bit mask.
 */
#define NETBUF_PAGES 0x10	//20110106 change 8 to 16
#define NETBUF_MASK (NETBUF_PAGES - 1)

/*
 * Network buffer layout.
 */
struct netbuf {
	struct netbuf *next;		/* Pointer to the next netbuf in a list */
	u16_t pos;			/* Index of the current read/write position */
	u16_t start;			/* Start of the netbuf (index of the first valid octet of data) */
	u16_t end;			/* End of the netbuf (index of the next octet after the last octet of data) */
	netpage_t pages[NETBUF_PAGES];	/* Table of netpages used for this netbuf */
//	u32_t ext;			/* Extension argument - may be used for any purpose but not across layers! */	//20110706 remove
#if defined(DEBUG) && defined(IPOS_DEBUG) && defined(NETBUF_DEBUG)
	u16_t magic;			/* Magic number to check if netbuf is valid */
#endif
};

/*
 * Types.
 */
typedef void (*netbuf_bulk_access_func_t)(u16_t addr, u16_t count);

/*
 * Prototypes.
 */
extern void netbuf_init(struct netbuf *nb);
extern void netbuf_final(struct netbuf *nb);
extern void netbuf_reset(struct netbuf *nb);
extern void netbuf_set_initial_offset(struct netbuf *nb, netpage_offs_t offset);
extern struct netbuf *netbuf_alloc(void);
extern struct netbuf *netbuf_alloc_with_rev_space(u16_t space);
extern struct netbuf *netbuf_alloc_with_fwd_space(u16_t space);
extern void netbuf_free(struct netbuf *nb);
extern struct netbuf *netbuf_copy(struct netbuf *newnb, struct netbuf *nb);
extern struct netbuf *netbuf_clone(struct netbuf *nb);
extern bool_t netbuf_populate_netpages(struct netbuf *nb);
extern netpage_t netpage_alloc_specific(netpage_t page, u8_t required);
extern bool_t netbuf_alloc_initial_sequential_space(struct netbuf *nb, u16_t size);
extern void netbuf_release_unused_pages(struct netbuf *nb);
extern void netbuf_normalize(struct netbuf *nb);
extern void netbuf_validate_pages(struct netbuf *nb);
extern void netbuf_set_start(struct netbuf *nb, u16_t pos);
extern u16_t netbuf_get_start(struct netbuf *nb);
extern void netbuf_set_start_to_pos(struct netbuf *nb);
extern void netbuf_advance_start(struct netbuf *nb, u16_t offs);
extern void netbuf_retreat_start(struct netbuf *nb, u16_t offs);
extern void netbuf_set_end(struct netbuf *nb, u16_t pos);
extern u16_t netbuf_get_end(struct netbuf *nb);
extern void netbuf_set_end_to_pos(struct netbuf *nb);
extern void netbuf_advance_end(struct netbuf *nb, u16_t offs);
extern void netbuf_retreat_end(struct netbuf *nb, u16_t offs);
extern void netbuf_set_pos(struct netbuf *nb, u16_t pos);
extern u16_t netbuf_get_pos(struct netbuf *nb);
extern void netbuf_set_pos_to_end(struct netbuf *nb);
extern void netbuf_set_pos_to_start(struct netbuf *nb);
extern void netbuf_advance_pos(struct netbuf *nb, u16_t offs);
extern void netbuf_retreat_pos(struct netbuf *nb, u16_t offs);
extern u16_t netbuf_get_extent(struct netbuf *nb);
extern u16_t netbuf_get_preceding(struct netbuf *nb);
extern u16_t netbuf_get_remaining(struct netbuf *nb);
extern bool_t netbuf_fwd_make_space(struct netbuf *nb, u16_t size);
extern bool_t netbuf_rev_make_space(struct netbuf *nb, u16_t size);
extern bool_t netbuf_fwd_check_space(struct netbuf *nb, u16_t size);
extern bool_t netbuf_rev_check_space(struct netbuf *nb, u16_t size);
extern u8_t netbuf_fwd_read_u8(struct netbuf *nb);
extern void netbuf_fwd_write_u8(struct netbuf *nb, u8_t v);
extern void netbuf_rev_write_u8(struct netbuf *nb, u8_t v);
extern u16_t netbuf_fwd_read_u16(struct netbuf *nb);
//extern u16_t netbuf_fwd_read_le_u16(struct netbuf *nb);
extern void netbuf_fwd_write_u16(struct netbuf *nb, u16_t v);
//extern void netbuf_fwd_write_le_u16(struct netbuf *nb, u16_t v);
extern void netbuf_rev_write_u16(struct netbuf *nb, u16_t v);
//extern void netbuf_rev_write_le_u16(struct netbuf *nb, u16_t v);
extern u32_t netbuf_fwd_read_u32(struct netbuf *nb);
//extern u32_t netbuf_fwd_read_le_u32(struct netbuf *nb);
extern void netbuf_fwd_write_u32(struct netbuf *nb, u32_t v);
//extern void netbuf_fwd_write_le_u32(struct netbuf *nb, u32_t v);
extern void netbuf_rev_write_u32(struct netbuf *nb, u32_t v);
//extern void netbuf_rev_write_le_u32(struct netbuf *nb, u32_t v);
extern u64_t netbuf_fwd_read_u64(struct netbuf *nb);
//extern u64_t netbuf_fwd_read_le_u64(struct netbuf *nb);
extern void netbuf_fwd_write_u64(struct netbuf *nb, u64_t v);
//extern void netbuf_fwd_write_le_u64(struct netbuf *nb, u64_t v);
extern void netbuf_rev_write_u64(struct netbuf *nb, u64_t v);
//extern void netbuf_rev_write_le_u64(struct netbuf *nb, u64_t v);  
extern addr_t netbuf_fwd_cmp_mem(struct netbuf *nb, void *buf, addr_t size);
extern void netbuf_fwd_read_mem(struct netbuf *nb, void *buf, addr_t size);
extern void netbuf_fwd_read_mem_rev(struct netbuf *nb, void *buf, addr_t size);

extern void netbuf_fwd_write_mem(struct netbuf *nb, void *buf, addr_t size);
extern void netbuf_rev_write_mem(struct netbuf *nb, void *buf, addr_t size);
extern bool_t netbuf_fwd_read_str(struct netbuf *nb, char *str, addr_t size);
extern u16_t netbuf_fwd_write_str(struct netbuf *nb, char *str);
extern u16_t netbuf_rev_write_str(struct netbuf *nb, char *str);
extern void netbuf_fwd_fill_u8(struct netbuf *nb, addr_t size, u8_t value);
extern void netbuf_rev_fill_u8(struct netbuf *nb, addr_t size, u8_t value);
extern void netbuf_fwd_copy(struct netbuf *nb, struct netbuf *orig, addr_t size);
extern void netbuf_rev_copy(struct netbuf *nb, struct netbuf *orig, addr_t size);
extern u16_t netbuf_crc16(struct netbuf *nb);
extern u32_t netbuf_crc32(struct netbuf *nb);
extern u16_t netbuf_fwd_ipcsum(struct netbuf *nb, u16_t csum, u16_t size);
extern void netbuf_fwd_printf(struct netbuf *nb, const char *fmt, ...);
//extern void netbuf_fwd_vprintf(struct netbuf *nb, const char *fmt, va_list argp);
//extern void netbuf_doprint(struct netbuf *nb, const char *fmt0, va_list argp);
/*extern void netbuf_fwd_write_hex_u8(struct netbuf *nb, u8_t v);
extern void netbuf_fwd_write_hex_u16(struct netbuf *nb, u16_t v);
extern void netbuf_fwd_write_hex_u32(struct netbuf *nb, u32_t v);
extern void netbuf_fwd_write_decimal_u32(struct netbuf *nb, u32_t v);
extern void netbuf_rev_write_decimal_u32(struct netbuf *nb, u32_t v);
extern void netbuf_fwd_write_ip_address(struct netbuf *nb, u8_t ip[4]);       */    
extern void netbuf_attach_tail(struct netbuf **base, struct netbuf *nb);
extern void netbuf_attach_head(struct netbuf **base, struct netbuf *nb);
extern struct netbuf *netbuf_detach_head(struct netbuf **base);
//extern void debug_print_netbuf_header(struct netbuf *nb);
//extern void debug_print_netbuf(struct netbuf *nb);
//extern void debug_print_netbuf_as_text(struct netbuf *nb);                    
/*extern void netbuf_rev_write_prog_mem(struct netbuf *nb, prog_addr_t prog_addr, addr_t size);
extern void netbuf_fwd_write_prog_mem(struct netbuf *nb, prog_addr_t prog_addr, addr_t size);
extern u16_t netbuf_rev_write_prog_str(struct netbuf *nb, prog_addr_t str_addr);
extern u16_t netbuf_fwd_write_prog_str(struct netbuf *nb, prog_addr_t str_addr);    */ //20101110
extern int netbuf_fwd_strncmp(struct netbuf *nb, char *s, addr_t len);
//extern int netbuf_fwd_prog_strncmp(struct netbuf *nb, prog_addr_t str_addr, addr_t len);
extern void netbuf_bulk_access(struct netbuf *nb, netbuf_bulk_access_func_t func, u16_t len);
extern bool_t netbuf_align(struct netbuf *nb, u16_t align);


/*
 * Macros to support conditional printing of debug info.
 */
#define DEBUG_PRINT_NETBUF_HEADER(nb) \
	if (RUNTIME_DEBUG) { \
		debug_print_netbuf_header((nb)); \
	}

#define DEBUG_PRINT_NETBUF(nb) \
	if (RUNTIME_DEBUG) { \
		debug_print_netbuf((nb)); \
	}

#define DEBUG_PRINT_NETBUF_AS_TEXT(nb) \
	if (RUNTIME_DEBUG) { \
		debug_print_netbuf_as_text((nb)); \
	}


#endif
	
