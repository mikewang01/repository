 /*
 FILE NAME: netpage.c
 ORG_CODE:	ubicom
 ORG_DATE:	2002/12/19 02:37:50
 ORG_VER:		1.13
 MOD_CODE:	@ZKJ
 MOD_DATE:  2015/11/24
 */
 
#include "netpage.h"

/*
 * Structure of the information stored within the netpage table.  The table indicates
 * how each page is currently being used.
 */
struct netpage {
	union {
//	struct{	//20110809 change to test
		ref_t refs;		/* Number of references to this page if active */
		netpage_t nextfree;	/* Page number of next page on the free list */
	} usage;
};

/*
 * Instance of the netpage code.
 */
struct netpage_instance {
	netpage_t next_free;							/* Next free netpage */
	netpage_t free_pages;							/* Number of free pages */
	netpage_t low_water_mark;					/* Lowest number of pages we've reached */
	struct netpage pginfo[NETPAGES];	/* Array of netpages and their status */
};

struct netpage_instance ni_inst;

#define ni ((struct netpage_instance *)(&ni_inst))

void *netpage_start_addr=NULL;
/*
 * netpage_init()
 */
u8_t netpage_init(void)
{
		netpage_t i;
	
		netpage_start_addr=NETPAGE_MALLOC_MEM(NETPAGE_SIZE*NETPAGES);
		if(!netpage_start_addr){
			return 0;
		}
		
		i = netpage_lowlevel_init();
		ni->next_free = i;
		ni->free_pages = NETPAGES /*- i*/;
		ni->low_water_mark = ni->free_pages;
		ni->pginfo[0].usage.refs = 0;       //20101110 change NETPAGE_NULL to 0
		while (i < NETPAGE_LAST) {        //20101110 change
			ni->pginfo[i].usage.nextfree = i + 1;   //20101110 change
			i++;
		}
		ni->pginfo[NETPAGE_LAST].usage.nextfree = NETPAGE_NULL;
		return 1;
}

/*
 * netpage_alloc()
 *	Get a free netpage.
 */
netpage_t netpage_alloc(void)
{
		netpage_t pg;
		pg = ni->next_free;
		if (pg == NETPAGE_NULL) {
			return pg;
		}
		ni->next_free = ni->pginfo[pg].usage.nextfree;
		ni->pginfo[pg].usage.refs = 1;
		ni->free_pages--;
		if (ni->free_pages < ni->low_water_mark) {
			ni->low_water_mark = ni->free_pages;
		}
		return pg;
}

/*
 * netpage_ref()
 *	Bump up the reference count on a page.
 */
ref_t netpage_ref(netpage_t pg)
{
		ref_t refs;
		ni->pginfo[pg].usage.refs++;
		refs = ni->pginfo[pg].usage.refs;
		return refs;
}

/*
 * netpage_deref()
 *	Drop down the reference count on a page.
 *
 * If our reference count hits zero we free the page back to the pool.
 */
ref_t netpage_deref(netpage_t pg)
{
		ref_t refs;
		ni->pginfo[pg].usage.refs--;
		refs = ni->pginfo[pg].usage.refs;

		if (refs == 0) {
			ni->pginfo[pg].usage.nextfree = ni->next_free;
			ni->next_free = pg;
			ni->free_pages++;
		}
		return refs;
}

/*
 * netpage_get_refs()
 *	Get the number of references for a given page.
 */
ref_t netpage_get_refs(netpage_t pg)
{
		ref_t result;
		if(pg == NETPAGE_NULL) return 0;	//20110505 add
		result = ni->pginfo[pg].usage.refs;
		return result;
}

/*
 * netpage_get_free()
 *	Get the number of free pages.
 */
netpage_t netpage_get_free(void)
{
		netpage_t f;
		f = ni->free_pages;
		return f;
}

/*
 * netpage_get_low_water()
 *	Get the number of free pages at their lowest point.
 */
netpage_t netpage_get_low_water(void)
{
		netpage_t l;
		l = ni->low_water_mark;
		return l;
}

//////////////////////////////////////////////////20101109 add s
u8_t netpage_lowlevel_init(void)
{
		return 0;
}

u8_t netpage_read_u8(netpage_t pg, netpage_offs_t offs)
{
      u32_t read_buf_v;
      u8_t *read_buf;
	
			if(!netpage_start_addr){
				return 0;
			}	
      read_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)pg + offs;
      read_buf=(u8_t *)read_buf_v;
      return (*read_buf);
}

void netpage_write_u8(netpage_t pg, netpage_offs_t offs, u8_t v)
{
			u32_t write_buf_v;
			u8_t *write_buf;
	
			if(!netpage_start_addr){
				return ;
			}
			write_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)pg + offs;
			write_buf=(u8_t *)write_buf_v;
			*write_buf=v;
}


u16_t netpage_read_u16(netpage_t pg, netpage_offs_t offs)
{
      u32_t read_buf_v;
      u8_t *read_buf;
			u16_t value;
	
			if(!netpage_start_addr){
				return 0;
			}
      read_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)pg + offs;
      read_buf=(u8_t *)read_buf_v;
			value=(u16_t)read_buf[0]<<8;
			value+=read_buf[1];
			return value;
}

void netpage_write_u16(netpage_t pg, netpage_offs_t offs, u16_t v)
{
			u32_t write_buf_v;
      u8_t *write_buf;
	
			if(!netpage_start_addr){
				return ;
			}
			write_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)pg + offs;
      write_buf=(u8_t *)write_buf_v;
			*write_buf++ =(u8_t)(v>>8);
			*write_buf=(u8_t)(v & 0xff);
}

void netpage_copy_u16(netpage_t dest_pg, netpage_offs_t dest_offs, netpage_t src_pg, netpage_offs_t src_offs)
{
			u32_t read_buf_v;
      u8_t *read_buf;
      u32_t write_buf_v;
      u8_t *write_buf;
	
			if(!netpage_start_addr){
				return ;
			}
			read_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)src_pg + src_offs;
      read_buf=(u8_t *)read_buf_v;
      write_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)dest_pg + dest_offs;
      write_buf=(u8_t *)write_buf_v;
			*write_buf++ = *read_buf++;
			*write_buf=*read_buf;
}

void netpage_copy(netpage_t dest, netpage_t src)
{
			u32_t read_buf_v;
      u8_t *read_buf;
      u32_t write_buf_v;
      u8_t *write_buf;
			u16_t i;
	
			if(!netpage_start_addr){
				return ;
			}
			read_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)src;
      read_buf=(u8_t *)read_buf_v;
      write_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)dest;
      write_buf=(u8_t *)write_buf_v;
			
			for(i=0;i<NETPAGE_SIZE;i++){
				write_buf[i]=read_buf[i];
			}
}

u16_t netpage_crc16(u16_t crc16, netpage_t pg, netpage_offs_t offs, u16_t count)
{
			return 0;
}

u32_t netpage_crc32(u32_t crc32, netpage_t pg, netpage_offs_t offs, u16_t count)
{
			return 0;
}

void netpage_fill_u8(netpage_t pg, netpage_offs_t offs, addr_t size, u8_t v)
{
			u32_t write_buf_v;
      u8_t *write_buf;
      u16_t i;
	
      if(!netpage_start_addr){
				return ;
			}
			write_buf_v =(u32_t)netpage_start_addr + NETPAGE_SIZE* (u32_t)pg + offs;
      write_buf=(u8_t *)write_buf_v;
			
			for(i=0;i<size;i++){
				write_buf[i]=v;
			}
}

u16_t byteswap16(u16_t v)
{
			return (((v & 0xff) << 8) | ((v & 0xff00) >> 8));
}

/*
 * byteswap32()
 *	Swap the order of bytes in a 32 bit word
 */
u32_t byteswap32(u32_t v)
{
			return ((u32_t)byteswap16((u16_t)v) << 16) | ((u32_t)byteswap16((u16_t)(v >> 16)));
}

/*
 * byteswap64()
 *	Swap the order of bytes in a 64 bit word
 */
u64_t byteswap64(u64_t v)
{
			return ((u64_t)byteswap32((u32_t)v) << 32) | ((u64_t)byteswap32((u32_t)(v >> 32)));
}

/*
 * arch_u16_to_be_u16()
 *	Return the big-endian form of an architecture-specific 16-bit word.
 */
u16_t arch_u16_to_be_u16(u16_t v)
{
			return v;
}

/*
 * arch_u32_to_be_u32()
 *	Return the big-endian form of an architecture-specific 32-bit word.
 */
u32_t arch_u32_to_be_u32(u32_t v)
{
			return v;
}

/*
 * arch_u64_to_be_u64()
 *	Return the big-endian form of an architecture-specific 64-bit word.
 */
u64_t arch_u64_to_be_u64(u64_t v)
{
			return v;
}

/*
 * arch_u16_to_le_u16()
 *	Return the little-endian form of an architecture-specific 16-bit word.
 */
u16_t arch_u16_to_le_u16(u16_t v)
{
			return byteswap16(v);
}

/*
 * arch_u32_to_le_u32()
 *	Return the little-endian form of an architecture-specific 32-bit word.
 */
u32_t arch_u32_to_le_u32(u32_t v)
{
			return byteswap32(v);
}

/*
 * arch_u64_to_le_u64()
 *	Return the little-endian form of an architecture-specific 64-bit word.
 */
u64_t arch_u64_to_le_u64(u64_t v)
{
			return byteswap64(v);
}

/*
 * be_u16_to_arch_u16()
 *	Return the architecture-specific form of a big-endian 16-bit word.
 */
u16_t be_u16_to_arch_u16(u16_t v)
{
			return v;
}

/*
 * be_u32_to_arch_u32()
 *	Return the architecture-specific form of a big-endian 32-bit word.
 */
u32_t be_u32_to_arch_u32(u32_t v)
{
			return v;
}

/*
 * be_u64_to_arch_u64()
 *	Return the architecture-specific form of a big-endian 64-bit word.
 */
u64_t be_u64_to_arch_u64(u64_t v)
{
			return v;
}

/*
 * le_u16_to_arch_u16()
 *	Return the architecture-specific form of a little-endian 16-bit word.
 */
u16_t le_u16_to_arch_u16(u16_t v)
{
			return byteswap16(v);
}

/*
 * le_u32_to_arch_u32()
 *	Return the architecture-specific form of a little-endian 32-bit word.
 */
u32_t le_u32_to_arch_u32(u32_t v)
{
			return byteswap32(v);
}

/*
 * le_u64_to_arch_u64()
 *	Return the architecture-specific form of a little-endian 64-bit word.
 */
u64_t le_u64_to_arch_u64(u64_t v)
{
			return byteswap64(v);
}

