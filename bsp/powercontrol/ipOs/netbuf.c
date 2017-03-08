/*
 * netbuf.c
 *	Network buffer support.
 *
 * Copyright ?2000-2003 Ubicom Inc. <www.ubicom.com>.  All Rights Reserved. *
 * This file contains confidential information of Ubicom, Inc. and your use of
 * this file is subject to the Ubicom Software License Agreement distributed with
 * this file. If you are uncertain whether you are an authorized user or to report
 * any unauthorized use, please contact Ubicom, Inc. at +1-650-210-1500.
 * Unauthorized reproduction or distribution of this file is subject to civil and
 * criminal penalties.
 *
 * $RCSfile: netbuf.c,v $
 * $Date: 2003/07/03 00:28:29 $
 * $Revision: 1.130 $
 *
 * The network buffer handling is designed to make life easy(ish) when it
 * comes to handling layered communications protocols.
 */
#include <string.h>
#include "netbuf.h"

#define RUNTIME_VERIFY_NETBUF_MAGIC(nb)
#define RUNTIME_FWD_VERIFY_READ_SPACE(nb, sz)
#define RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, sz) 
#define RUNTIME_REV_VERIFY_WRITE_SPACE(nb, sz)

/*
 * netbuf_fwd_make_space()
 */
 
netpage_t first_pg,last_pg,i_pg;

bool_t netbuf_fwd_make_space(struct netbuf *nb, u16_t size)
{
		netpage_t pgidx;
		netpage_t origpg;
		netpage_t last_pg;
		netpage_t i_pg;
		netpage_t first_pg;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		first_pg = nb->pos >> NETPAGE_SHIFT;

		if (netpage_get_refs(nb->pages[first_pg & NETBUF_MASK]) == 1) {
			first_pg++;
		}
		last_pg = (nb->pos + size - 1) >> NETPAGE_SHIFT;
		
		for (i_pg = first_pg; i_pg <= last_pg; i_pg++) {
			pgidx = i_pg & NETBUF_MASK;
			origpg = nb->pages[pgidx];
			if (netpage_get_refs(origpg) != 1) {
				netpage_t newpg = netpage_alloc();
				if (newpg== NETPAGE_NULL) {   //2010 change !newpg
					return FALSE;
				}
				nb->pages[pgidx] = newpg;

				if (origpg != NETPAGE_NULL) {
					netpage_copy(newpg, origpg);
					netpage_deref(origpg);
				}
			}
		}

		if (nb->end < (nb->pos + size)) {
			nb->end = (nb->pos + size);
		}

	return TRUE;
}

/*
 * netbuf_rev_make_space()
 */
bool_t netbuf_rev_make_space(struct netbuf *nb, u16_t size)
{
		netpage_t firstpg;
		netpage_t ipg;
		netpage_t lastpg;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		lastpg = nb->pos >> NETPAGE_SHIFT;

		if ((netpage_get_refs(nb->pages[lastpg & NETBUF_MASK]) == 1) || ((nb->pos & 0xff) == 0)) {
			lastpg--;
		}
		firstpg = (nb->pos - size) >> NETPAGE_SHIFT;
		
		for (ipg = firstpg; ipg <= lastpg; ipg++) {
			netpage_t pgidx = ipg & NETBUF_MASK;
			netpage_t origpg = nb->pages[pgidx];
			if (netpage_get_refs(origpg) != 1) {
				netpage_t newpg = netpage_alloc();
				if (newpg== NETPAGE_NULL) {
					return FALSE;
				}
				nb->pages[pgidx] = newpg;

				if (origpg != NETPAGE_NULL) {
					netpage_copy(newpg, origpg);
					netpage_deref(origpg);
				}
			}
		}
		if (nb->start > (nb->pos - size)) {
			nb->start = (nb->pos - size);
		}
		return TRUE;
}

/*
 * netbuf_fwd_check_space()
 */
bool_t netbuf_fwd_check_space(struct netbuf *nb, u16_t size)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		return ((nb->end - nb->pos) >= size) ? TRUE : FALSE;
}

/*
 * netbuf_rev_check_space()
 */
bool_t netbuf_rev_check_space(struct netbuf *nb, u16_t size)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		return ((nb->pos - nb->start) >= size) ? TRUE : FALSE;
}

/*
 * netbuf_init()
 *	Initialize a netbuf.
 */
void netbuf_init(struct netbuf *nb)
{
		netpage_t *pg;
		u8_t i;
		
		nb->pos = NETBUF_PAGES << NETPAGE_SHIFT;
		nb->start = NETBUF_PAGES << NETPAGE_SHIFT;
		nb->end = NETBUF_PAGES << NETPAGE_SHIFT;
		nb->next = NULL;
		pg = nb->pages;	
	
		for (i = NETBUF_PAGES; i != 0; i--) {
			*pg++ = NETPAGE_NULL;
		}
}

/*
 * netbuf_final()
 *	Finalize a netbuf.
 */
void netbuf_final(struct netbuf *nb)
{
		netpage_t *pg;
		u8_t i;

		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		pg = nb->pages;
	
		for (i = NETBUF_PAGES; i != 0; i--) {
			if (*pg != NETPAGE_NULL) {
				netpage_deref(*pg);
			}
			pg++;
		}
}

/*
 * netbuf_reset()
 *	Reset a netbuf - empty it's pages and start again!
 */
void netbuf_reset(struct netbuf *nb)
{
		netbuf_final(nb);
		netbuf_init(nb);
}

/*
 * netbuf_set_initial_offset()
 */
void netbuf_set_initial_offset(struct netbuf *nb, netpage_offs_t offset)
{
		nb->pos = (NETBUF_PAGES << NETPAGE_SHIFT) + offset;
		nb->start = (NETBUF_PAGES << NETPAGE_SHIFT) + offset;
		nb->end = (NETBUF_PAGES << NETPAGE_SHIFT) + offset;
}

/*
 * netbuf_alloc()
 */
struct netbuf *netbuf_alloc(void)
{
		struct netbuf *nb;
		nb = (struct netbuf *)NETPAGE_MALLOC_MEM(sizeof(struct netbuf));
		if (nb) {
			netbuf_init(nb);
		}
		return nb;
}

/*
 * netbuf_alloc_with_rev_space()
 *	Allocates a new netbuf, and makes indicated reverse space
 *	If it allocates a new netbuf, but cannot make reverse space, it frees the netbuf.
 *	It returns a new netbuf with reveres space in case of success or a NULL in case of failure
 */
struct netbuf *netbuf_alloc_with_rev_space(u16_t space)
{
		struct netbuf *nb;
	  nb = netbuf_alloc();
		if (!nb) {
			return NULL;
		}

		if (!netbuf_rev_make_space(nb, space)) {
			netbuf_free(nb);
			return NULL;
		}
		return nb;	
}

/*
 * netbuf_alloc_with_fwd_space()
 *	Allocates a new netbuf, and makes indicated forward space
 *	If it allocates a new netbuf, but cannot make forward space, it frees the netbuf.
 *	It returns a new netbuf with forward space in case of success or a NULL in case of failure
 */
struct netbuf *netbuf_alloc_with_fwd_space(u16_t space)
{
		struct netbuf *nb = netbuf_alloc();
		if (!nb) {
			return NULL;
		}
		if (!netbuf_fwd_make_space(nb, space)) {
			netbuf_free(nb);
			return NULL;
		}
		return nb;	
}

/*
 * netbuf_free()
 *	Callback to handle cleaning up a netbuf.
 */
void netbuf_free(struct netbuf *nb)
{
	  if(nb == NULL) return;
	
		netbuf_final(nb);
		NETPAGE_FREE_MEM(nb);
		nb=NULL;
}

/*
 * netbuf_copy()
 *	Produce a copy of a netbuf.
 */
struct netbuf *netbuf_copy(struct netbuf *newnb, struct netbuf *nb)
{
		netpage_t *pg;
		u8_t i;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		*newnb = *nb;
		newnb->next = NULL;

		pg = newnb->pages;
		for (i = NETBUF_PAGES; i != 0; i--) {
			if (*pg != NETPAGE_NULL) {
				netpage_ref(*pg);
			}
			pg++;
		}
		return newnb;
}

/*
 * netbuf_clone()
 *	Produce a clone (copy) of a netbuf.
 */
struct netbuf *netbuf_clone(struct netbuf *nb)
{
		struct netbuf *newnb;
	
		if(!nb) return NULL;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		newnb = (struct netbuf *)NETPAGE_MALLOC_MEM(sizeof(struct netbuf));
		if (!newnb) {
			return newnb;
		}
		return netbuf_copy(newnb, nb);
}

/*
 * netbuf_populate_netpages()
 *	Fill a netbuf with pages from start to end.
 */
bool_t netbuf_populate_netpages(struct netbuf *nb)
{
		netpage_t pg;
		netpage_t i;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		for (i = (nb->start >> NETPAGE_SHIFT); i <= ((nb->end - 1) >> NETPAGE_SHIFT); i++) {
			pg = nb->pages[i & NETBUF_MASK];
			if (pg == NETPAGE_NULL) {
				pg = netpage_alloc();
				if (!pg) {
					return FALSE;
				}

				nb->pages[i & NETBUF_MASK] = pg;
			}
		}
		return TRUE;
}


/*
 * netbuf_release_unused_pages
 */
void netbuf_release_unused_pages(struct netbuf *nb)
{
		netpage_t pg;
		netpage_t left;
		netpage_t right;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

	  left = (nb->start >> NETPAGE_SHIFT) + NETBUF_PAGES;
		right = ((nb->end - 1) >> NETPAGE_SHIFT);

		for (pg = left - 1; pg > right; pg--) {
			if (nb->pages[pg & NETBUF_MASK]!=NETPAGE_NULL) {	//20110505 add !=NETPAGE_NULL
				netpage_deref(nb->pages[pg & NETBUF_MASK]);
				nb->pages[pg & NETBUF_MASK] = NETPAGE_NULL;	//20110708 change 0 to NETPAGE_NULL
			}
		}
}

/*
 * netbuf_normalize()
 *	Normalize the location of data within a netbuf.
 *
 * When using a netbuf as a buffer, for example in conjunction with a UART,
 * we can end up continually extracting information from the start of the
 * buffer whilst appending new data to the end.  Often, ring buffer structures
 * are used in such cases, however with a netbuf we have a high performance
 * alternative because of our page mapping strategy (we don't want to make a
 * netbuf a true ring buffer in order to keep runtime costs as low as
 * possible).
 *
 * The strategy employed here to normalize our netbuf is to dereference any
 * netpages to the left of the "start" position and, if our start position has
 * moved too far right to move all of our position references left.
 */
void netbuf_normalize(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		/*
		 * First go and look for any pages that we don't want to reference any more.
		 */
		netbuf_release_unused_pages(nb);
		/*
		 * Have we gone sufficiently far right that we should be brought back
		 * to the left?
		 */
		if (nb->start > (NETPAGE_SIZE * NETBUF_PAGES * 3 / 2)) {
			nb->start -= NETPAGE_SIZE * NETBUF_PAGES;
			nb->end -= NETPAGE_SIZE * NETBUF_PAGES;
			nb->pos -= NETPAGE_SIZE * NETBUF_PAGES;
		}
}

/*
 * netbuf_validate_pages()
 */
#if (RUNTIME_DEBUG)
void netbuf_validate_pages(struct netbuf *nb)
{
	u8_t i;

	netpage_t *pg = nb->pages;
	for (i = NETBUF_PAGES; i != 0; i--) {
		if (*pg != NETPAGE_NULL) {
			DEBUG_ASSERT(netpage_get_refs(*pg) != 0, "page %d: refs is 0", *pg);
		}

		pg++;
	}
}
#endif

/*
 * netbuf_set_start()
 *	Set the position of the netbuf buffer start pointer.
 */
void netbuf_set_start(struct netbuf *nb, u16_t pos)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->start = pos;
}

/*
 * netbuf_get_start()
 *	Get the position of the netbuf buffer start pointer.
 */
u16_t netbuf_get_start(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		return nb->start;
}

/*
 * netbuf_set_start_to_pos()
 *	Set the position of the netbuf buffer start pointer to that of the pos pointer.
 */
void netbuf_set_start_to_pos(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->start = nb->pos;
}

/*
 * netbuf_advance_start()
 *	Advance the buffer start pointer.
 */
void netbuf_advance_start(struct netbuf *nb, u16_t offs)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->start += offs;
}

/*
 * netbuf_retreat_start()
 *	Retreat (move back) the start pointer.
 */
void netbuf_retreat_start(struct netbuf *nb, u16_t offs)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->start -= offs;
}

/*
 * netbuf_set_end()
 *	Set the position of the netbuf buffer end pointer.
 */
void netbuf_set_end(struct netbuf *nb, u16_t pos)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->end = pos;
}

/*
 * netbuf_get_end()
 *	Get the position of the netbuf buffer end pointer.
 */
u16_t netbuf_get_end(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		return nb->end;
}

/*
 * netbuf_set_end_to_pos()
 *	Set the position of the netbuf buffer end pointer to that of the pos pointer.
 */
void netbuf_set_end_to_pos(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->end = nb->pos;
}

/*
 * netbuf_advance_end()
 *	Advance the buffer end pointer.
 */
void netbuf_advance_end(struct netbuf *nb, u16_t offs)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->end += offs;
}

/*
 * netbuf_retreat_end()
 *	Retreat (move back) the end pointer.
 */
void netbuf_retreat_end(struct netbuf *nb, u16_t offs)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->end -= offs;
}

/*
 * netbuf_set_pos()
 *	Set the read/write position of the netbuf to the specified value.
 */
void netbuf_set_pos(struct netbuf *nb, u16_t pos)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->pos = pos;
}

/*
 * netbuf_get_pos()
 *	Get the current position of the netbuf pointer.
 */
u16_t netbuf_get_pos(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		return nb->pos;
}

/*
 * netbuf_set_pos_to_start()
 *	Set the read/write position to the start of the netbuf.
 */
void netbuf_set_pos_to_start(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->pos = nb->start;
}

/*
 * netbuf_set_pos_to_end()
 *	Set the read/write position to the end of the netbuf.
 */
void netbuf_set_pos_to_end(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->pos = nb->end;
}

/*
 * netbuf_advance_pos()
 *	Advance the position of the netbuf stream to the specified value.
 */
void netbuf_advance_pos(struct netbuf *nb, u16_t offs)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->pos += offs;
}

/*
 * netbuf_retreat_pos()
 *	Retreat the position of the netbuf stream to the specified value.
 */
void netbuf_retreat_pos(struct netbuf *nb, u16_t offs)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->pos -= offs;
}

/*
 * netbuf_get_preceding()
 *	Get the number of bytes before the current netbuf buffer position.
 */
u16_t netbuf_get_preceding(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		if(nb->pos < nb->start){
			return 0;
		}
		return nb->pos - nb->start;
}

/*
 * netbuf_get_remaining()
 *	Get the number of bytes remaining after the current netbuf buffer position.
 */
u16_t netbuf_get_remaining(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		if(!nb || nb->pos > nb->end){
			return 0;
		}
		return nb->end - nb->pos;
}

/*
 * netbuf_get_extent()
 *	Get the number of bytes in the netbuf.
 */
u16_t netbuf_get_extent(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		if(!nb) return 0;
		return nb->end - nb->start;
}

/*
 * fwd_read_u8()
 */
static u8_t fwd_read_u8(struct netbuf *nb)
{
		u8_t v;
		netpage_offs_t offs = nb->pos & NETPAGE_MASK;
		netpage_t pg = nb->pos >> NETPAGE_SHIFT;
		nb->pos++;
	  v = netpage_read_u8(nb->pages[pg & NETBUF_MASK], offs);
		return v;
}

/*
 * fwd_write_u8()
 */
static void fwd_write_u8(struct netbuf *nb, u8_t v)
{
		netpage_offs_t offs = nb->pos & NETPAGE_MASK;
		netpage_t pg = nb->pos >> NETPAGE_SHIFT;
		nb->pos++;
		netpage_write_u8(nb->pages[pg & NETBUF_MASK], offs, v);
}

/*
 * rev_write_u8()
 */
static void rev_write_u8(struct netbuf *nb, u8_t v)
{
		netpage_offs_t offs;
		nb->pos--;
		offs = nb->pos & NETPAGE_MASK;
		netpage_write_u8(nb->pages[(netpage_t)(nb->pos >> NETPAGE_SHIFT) & NETBUF_MASK], offs, v);
}

/*
 * fwd_read_u16()
 */
static u16_t fwd_read_u16(struct netbuf *nb)
{
		u16_t v;
		netpage_offs_t offs = nb->pos & NETPAGE_MASK;
		netpage_t pg = nb->pos >> NETPAGE_SHIFT;
		nb->pos += 2;
		if (offs != (netpage_offs_t)(NETPAGE_SIZE - 1)) {
			v = netpage_read_u16(nb->pages[pg & NETBUF_MASK], offs);
		} else {
			v = (u16_t)netpage_read_u8(nb->pages[pg & NETBUF_MASK], NETPAGE_SIZE - 1) << 8;
			v |= netpage_read_u8(nb->pages[(netpage_t)(pg + 1) & NETBUF_MASK], 0);
		}

		return v;
}

/*
 * fwd_write_u16()
 */
static void fwd_write_u16(struct netbuf *nb, u16_t v)
{
	netpage_offs_t offs = nb->pos & NETPAGE_MASK;
	netpage_t pg = nb->pos >> NETPAGE_SHIFT;
	nb->pos += 2;

	if (offs != (netpage_offs_t)(NETPAGE_SIZE - 1)) {
		netpage_write_u16(nb->pages[pg & NETBUF_MASK], offs, v);
	} else {
		netpage_write_u8(nb->pages[pg & NETBUF_MASK], NETPAGE_SIZE - 1, (u8_t)(v >> 8));
		netpage_write_u8(nb->pages[(netpage_t)(pg + 1) & NETBUF_MASK], 0, (u8_t)(v & 0xff));
	}
}

/*
 * rev_write_u16()
 */
static void rev_write_u16(struct netbuf *nb, u16_t v)
{
		netpage_offs_t offs;
		netpage_t pg;
	
		nb->pos -= 2;
		offs = nb->pos & NETPAGE_MASK;
		pg = nb->pos >> NETPAGE_SHIFT;

		if (offs != (netpage_offs_t)(NETPAGE_SIZE - 1)) {
			netpage_write_u16(nb->pages[pg & NETBUF_MASK], offs, v);
		} else {
			netpage_write_u8(nb->pages[pg & NETBUF_MASK], NETPAGE_SIZE - 1, (u8_t)(v >> 8));
			netpage_write_u8(nb->pages[(netpage_t)(pg + 1) & NETBUF_MASK], 0, (u8_t)(v & 0xff));
		}
}

/*
 * fwd_fill_u8()
 */
static void fwd_fill_u8(struct netbuf *nb, addr_t size, u8_t v)
{
		while (size) {
			netpage_offs_t offs = nb->pos & NETPAGE_MASK;
			addr_t wr_size = NETPAGE_SIZE - offs;
			wr_size = (size > wr_size) ? wr_size : size;
			netpage_fill_u8(nb->pages[(nb->pos >> NETPAGE_SHIFT) & NETBUF_MASK], offs, wr_size, v);
			size -= wr_size;
			netbuf_advance_pos(nb, wr_size);
		}
}     

/*
 * netbuf_fwd_read_u8()
 *	Read a u8_t value forwards from the current netbuf buffer offset.
 */
u8_t netbuf_fwd_read_u8(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_READ_SPACE(nb, 1);

		return fwd_read_u8(nb);
}

/*
 * netbuf_fwd_write_u8()
 *	Write a u8_t value forwards from the current netbuf buffer offset.
 */
void netbuf_fwd_write_u8(struct netbuf *nb, u8_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, 1);
		
		fwd_write_u8(nb, v);
}


/*
 * netbuf_rev_write_u8()
 *	Write a u8_t value backwards from the current netbuf buffer offset.
 */
void netbuf_rev_write_u8(struct netbuf *nb, u8_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_REV_VERIFY_WRITE_SPACE(nb, 1);
	
		rev_write_u8(nb, v);
}

/*
 * netbuf_fwd_read_u16()
 *	Read a u16_t value forwards from the current netbuf buffer offset.
 */
u16_t netbuf_fwd_read_u16(struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_READ_SPACE(nb, 2);

		return fwd_read_u16(nb);
}    


/*
 * netbuf_fwd_write_u16()
 *	Write a u16_t value forwards from the current netbuf buffer offset.
 */
void netbuf_fwd_write_u16(struct netbuf *nb, u16_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, 2);

		fwd_write_u16(nb, v);
}


/*
 * netbuf_rev_write_u16()
 *	Write a u16_t value backwards from the current netbuf buffer offset.
 */
void netbuf_rev_write_u16(struct netbuf *nb, u16_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_REV_VERIFY_WRITE_SPACE(nb, 2);

		rev_write_u16(nb, v);
}


/*
 * netbuf_fwd_read_u32()
 *	Read a u32_t value forwards from the current netbuf buffer offset.
 */
u32_t netbuf_fwd_read_u32(struct netbuf *nb)
{
		u16_t vh;
		u16_t vl;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_READ_SPACE(nb, 4);

		vh = fwd_read_u16(nb);
		vl = fwd_read_u16(nb);

		return ((u32_t)vh << 16) | (u32_t)vl;     
     
}


/*
 * netbuf_fwd_write_u32()
 *	Write a u32_t value forwards from the current netbuf buffer offset.
 */
void netbuf_fwd_write_u32(struct netbuf *nb, u32_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, 4);
	
		fwd_write_u16(nb, (u16_t)((v >> 16) & 0xffff));
		fwd_write_u16(nb, (u16_t)(v & 0xffff));         
     
}

/*
 * netbuf_rev_write_u32()
 *	Write a u32_t value backwards from the current netbuf buffer offset.
 */
void netbuf_rev_write_u32(struct netbuf *nb, u32_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_REV_VERIFY_WRITE_SPACE(nb, 4);

		rev_write_u16(nb, (u16_t)(v & 0xffff));
		rev_write_u16(nb, (u16_t)((v >> 16) & 0xffff));       
}

/*
 * netbuf_rev_write_le_u32()
 *	Write a little-endian u32_t value backwards from the current netbuf buffer offset.
 *
void netbuf_rev_write_le_u32(struct netbuf *nb, u32_t v)
{
	RUNTIME_VERIFY_NETBUF_MAGIC(nb);
	RUNTIME_REV_VERIFY_WRITE_SPACE(nb, 4);

	rev_write_u16(nb, arch_u16_to_le_u16((u16_t)((v >> 16) & 0xffff)));
	rev_write_u16(nb, arch_u16_to_le_u16((u16_t)(v & 0xffff)));
}     */    //20101112 change

/*
 * netbuf_fwd_read_u64()
 *	Read a u64_t value forwards from the current netbuf buffer offset.
 */
u64_t netbuf_fwd_read_u64(struct netbuf *nb)
{
		u16_t va;
		u16_t vb;
		u16_t vc;
		u16_t vd;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_READ_SPACE(nb, 8);

		va = fwd_read_u16(nb);
		vb = fwd_read_u16(nb);
		vc = fwd_read_u16(nb);
		vd = fwd_read_u16(nb);

		return ((u64_t)va << 48) | ((u64_t)vb << 32) | ((u64_t)vc << 16) | (u64_t)vd;       
}

/*
 * netbuf_fwd_read_le_u64()
 *	Read a little-endian u64_t value forwards from the current netbuf buffer offset.
 *
u64_t netbuf_fwd_read_le_u64(struct netbuf *nb)
{
	RUNTIME_VERIFY_NETBUF_MAGIC(nb);
	RUNTIME_FWD_VERIFY_READ_SPACE(nb, 8);

	u16_t vd = le_u16_to_arch_u16(fwd_read_u16(nb));
	u16_t vc = le_u16_to_arch_u16(fwd_read_u16(nb));
	u16_t vb = le_u16_to_arch_u16(fwd_read_u16(nb));
	u16_t va = le_u16_to_arch_u16(fwd_read_u16(nb));

	return ((u64_t)va << 48) | ((u64_t)vb << 32) | ((u64_t)vc << 16) | (u64_t)vd;
}     */    //20101112 change

/*
 * netbuf_fwd_write_u64()
 *	Write a u64_t value forwards from the current netbuf buffer offset.
 */
void netbuf_fwd_write_u64(struct netbuf *nb, u64_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, 8);

		fwd_write_u16(nb, (u16_t)((v >> 48) & 0xffff));
		fwd_write_u16(nb, (u16_t)((v >> 32) & 0xffff));
		fwd_write_u16(nb, (u16_t)((v >> 16) & 0xffff));
		fwd_write_u16(nb, (u16_t)(v & 0xffff));   
     
}

/*
 * netbuf_rev_write_u64()
 *	Write a u64_t value backwards from the current netbuf buffer offset.
 */
void netbuf_rev_write_u64(struct netbuf *nb, u64_t v)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_REV_VERIFY_WRITE_SPACE(nb, 8);

		rev_write_u16(nb, (u16_t)(v & 0xffff));
		rev_write_u16(nb, (u16_t)((v >> 16) & 0xffff));
		rev_write_u16(nb, (u16_t)((v >> 32) & 0xffff));
		rev_write_u16(nb, (u16_t)((v >> 48) & 0xffff));       
}

/*
 * netbuf_rev_write_le_u64()
 *	Write a little-endian u64_t value backwards from the current netbuf buffer offset.
 *
void netbuf_rev_write_le_u64(struct netbuf *nb, u64_t v)
{
	RUNTIME_VERIFY_NETBUF_MAGIC(nb);
	RUNTIME_REV_VERIFY_WRITE_SPACE(nb, 8);

	rev_write_u16(nb, arch_u16_to_le_u16((u16_t)((v >> 48) & 0xffff)));
	rev_write_u16(nb, arch_u16_to_le_u16((u16_t)((v >> 32) & 0xffff)));
	rev_write_u16(nb, arch_u16_to_le_u16((u16_t)((v >> 16) & 0xffff)));
	rev_write_u16(nb, arch_u16_to_le_u16((u16_t)(v & 0xffff)));
}     */    //20101112 change

/*
 * netbuf_fwd_cmp_mem()
 *	Compare a memory buffer forwards from the current netbuf buffer offset.
 */
addr_t netbuf_fwd_cmp_mem(struct netbuf *nb, void *buf, addr_t size)
{
		u16_t *p;
		u16_t words;
		u16_t n;
		u8_t misbyte;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_READ_SPACE(nb, size);

		p = (u16_t *)buf;
		words = size >> 1;

		while (words) {
			n = be_u16_to_arch_u16(fwd_read_u16(nb));
			if (*p++ != n) {
				misbyte = 0;
				if (*(u8_t *)p != (u8_t)n) misbyte = 1;
				return misbyte + (words >> 1) + (size & 1);
			}
			words--;
		}

		if (size & 1) {
			if (*(u8_t *)p != fwd_read_u8(nb)) {
				return 1;
			}
		}
		return 0;
}

/*
 * netbuf_fwd_read_mem()
 *	Read a memory buffer forwards from the current netbuf buffer offset.
 */
void netbuf_fwd_read_mem(struct netbuf *nb, void *buf, addr_t size)
{
		u8_t *p;
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_READ_SPACE(nb, size);
		
		p=(u8_t *)buf;
		while(size--){
					*p++ = fwd_read_u8(nb);
				}
}

/*
 * netbuf_fwd_read_mem_rev()
 *	Read a memory buffer forwards from the current netbuf buffer offset.
 */
void netbuf_fwd_read_mem_rev(struct netbuf *nb, void *buf, addr_t size)
{
		u8_t *p;
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_READ_SPACE(nb, size);
		
		p=(u8_t *)buf;
		while(size--){
					*(p+size) = fwd_read_u8(nb);
				}
}


/*
 * netbuf_fwd_write_mem()
 *	Write a memory buffer forwards from the current netbuf buffer offset.
 */
void netbuf_fwd_write_mem(struct netbuf *nb,void *buf, addr_t size)
{
		u8_t *p;
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, size);
      
		p=(u8_t *)buf;
		while(size--){
			fwd_write_u8(nb, *p++);
		}           
}

/*
 * netbuf_rev_write_mem()
 *	Write a memory block backwards from the current netbuf buffer offset.
 */
void netbuf_rev_write_mem(struct netbuf *nb,void *buf, addr_t size)
{
		u8_t *p;
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_REV_VERIFY_WRITE_SPACE(nb, size);
				
		p=(u8_t *)((u8_t *)buf + size);
		while(size--){
			rev_write_u8(nb, *--p);
		}
}

/*
 * netbuf_fwd_read_str()
 *	Read a string from the current netbuf buffer offset and store it in memory.
 */
bool_t netbuf_fwd_read_str(struct netbuf *nb, char *str, addr_t size)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		while (size) {
			if (nb->pos + 1 > nb->end) {
				return FALSE;
			}
			if ((*str++ = (char)fwd_read_u8(nb)) == 0) {
				return TRUE;
			}
			size--;
		}
		return FALSE;
}

/*
 * netbuf_fwd_write_str()
 *	Write a string forwards from the current netbuf buffer offset.
 */
u16_t netbuf_fwd_write_str(struct netbuf *nb,char *str)
{
		u16_t strsz;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		strsz = strlen(str);
		if (!strsz) {
			return 0;
		}

		if (!netbuf_fwd_make_space(nb, strsz)) {
			return 0;
		}
		netbuf_fwd_write_mem(nb, str, strsz);
		return strsz;
}

/*
 * netbuf_rev_write_str()
 *	Write a string backwards from the current netbuf buffer offset.
 */
u16_t netbuf_rev_write_str(struct netbuf *nb,char *str)
{
		u16_t strsz;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		strsz = strlen(str);
		if (!strsz) {
			return 0;
		}

		if (!netbuf_rev_make_space(nb, strsz)) {
			return 0;
		}

		netbuf_rev_write_mem(nb, str, strsz);
		return strsz;
}

/*
 * netbuf_fwd_fill_u8()
 *	Fill a memory block forwards from the current netbuf buffer offset with the given value.
 */
void netbuf_fwd_fill_u8(struct netbuf *nb, addr_t size, u8_t value)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, size);

		fwd_fill_u8(nb, size, value);
}

/*
 * netbuf_rev_fill_u8()
 *	Fill a memory block backwards from the current netbuf buffer offset with the given value.
 */
void netbuf_rev_fill_u8(struct netbuf *nb, addr_t size, u8_t value)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_REV_VERIFY_WRITE_SPACE(nb, size);

		netbuf_retreat_pos(nb, size);
		fwd_fill_u8(nb, size, value);
		netbuf_retreat_pos(nb, size);
}

/*
 * netbuf_fwd_copy()
 *	Copy a forwards-heading netbuf to another forwards-heading one, updating
 *	both buffer positions.
 */
void netbuf_fwd_copy(struct netbuf *nb, struct netbuf *orig, addr_t size)
{
		netpage_offs_t orig_offs;
		netpage_offs_t nb_offs;
		u16_t words;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_FWD_VERIFY_WRITE_SPACE(nb, size);
		RUNTIME_FWD_VERIFY_READ_SPACE(orig, size);

	  orig_offs = orig->pos & NETPAGE_MASK;
		nb_offs = nb->pos & NETPAGE_MASK;

		words = size >> 1;

		if (!(nb_offs & 0x01) && !(orig_offs & 0x01)) {
			netpage_t orig_pg = orig->pos >> NETPAGE_SHIFT;
			netpage_t orig_netpg = orig->pages[orig_pg & NETBUF_MASK];
			netpage_t nb_pg = nb->pos >> NETPAGE_SHIFT;
			netpage_t nb_netpg = nb->pages[nb_pg & NETBUF_MASK];

			while (words) {
				netpage_copy_u16(nb_netpg, nb_offs, orig_netpg, orig_offs);

				orig_offs = (orig_offs + 2) & NETPAGE_MASK;
				if (orig_offs == 0) {
					orig_pg++;
					orig_netpg = orig->pages[orig_pg & NETBUF_MASK];
				}

				nb_offs = (nb_offs + 2) & NETPAGE_MASK;
				if (nb_offs == 0) {
					nb_pg++;
					nb_netpg = nb->pages[nb_pg & NETBUF_MASK];
				}

				words--;
			}

			orig->pos += (size & 0xfffe);
			nb->pos += (size & 0xfffe);
		} else {
			while (words) {
				fwd_write_u16(nb, fwd_read_u16(orig));
				words--;
			}
		}

		if (size & 1) {
			fwd_write_u8(nb, fwd_read_u8(orig));
		}
}

/*
 * netbuf_rev_copy()
 *	Copy a forwards-heading netbuf to a backwards-heading one, updating both
 *	buffer positions.
 *
 * Note that the copied block is copied as an atomic entity and thus it is not
 * reversed even though it is being written in different directions.
 */
void netbuf_rev_copy(struct netbuf *nb, struct netbuf *orig, addr_t size)
{
		u16_t words;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		RUNTIME_REV_VERIFY_WRITE_SPACE(nb, size);
		RUNTIME_FWD_VERIFY_READ_SPACE(orig, size);

		netbuf_retreat_pos(nb, size);
		words = size >> 1;

		while (words) {
			fwd_write_u16(nb, fwd_read_u16(orig));
			words--;
		}

		if (size & 1) {
			fwd_write_u8(nb, fwd_read_u8(orig));
		}
		netbuf_retreat_pos(nb, size);
}

/*
 * netbuf_crc16()
 *	Calculate the CCITT CRC16 for the entire netbuf.
 */
u16_t netbuf_crc16(struct netbuf *nb)
{
		u16_t size;
		u16_t result;
		u16_t sz;
		netpage_offs_t offs;
		netpage_t idx;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		result = 0xffff;

		offs = (netpage_offs_t)(nb->start & NETPAGE_MASK);
		idx = nb->start >> NETPAGE_SHIFT;
		size = nb->end - nb->start;

		while (size) {
			

			if (size + offs > NETPAGE_SIZE) {
				sz = NETPAGE_SIZE - offs;
			} else {
				sz = size;
			}
			size -= sz;

			result = netpage_crc16(result, nb->pages[idx & NETBUF_MASK], offs, sz);
			offs = 0;
			idx++;
		}
		
		return ~result;
}

/*
 * netbuf_crc32()
 *	Calculate the Ethernet CRC32 for the entire netbuf.
 */
u32_t netbuf_crc32(struct netbuf *nb)
{
		u32_t result;
		u16_t size;
		netpage_offs_t offs;
		netpage_t idx;
	
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		result = 0xffffffff;

	  offs = (netpage_offs_t)(nb->start & NETPAGE_MASK);
		idx = nb->start >> NETPAGE_SHIFT;
		size = nb->end - nb->start;

		while (size) {
			u16_t sz;

			if (size + offs > NETPAGE_SIZE) {
				sz = NETPAGE_SIZE - offs;
			} else {
				sz = size;
			}
			size -= sz;

			result = netpage_crc32(result, nb->pages[idx & NETBUF_MASK], offs, sz);
			offs = 0;
			idx++;
		}
		return ~result;
}

/*
 * netbuf_fwd_ipcsum()
 *	Calculate the IP checksum for an identified region within a netbuf.
 *
 * The checksum is calculated from the current netbuf buffer offset for "size"
 * bytes.  The result is calculated based on a previous partial checksum that
 * is also passed to this function.
 *
u16_t netbuf_fwd_ipcsum(struct netbuf *nb, u16_t csum, u16_t size)
{
	RUNTIME_VERIFY_NETBUF_MAGIC(nb);
	RUNTIME_FWD_VERIFY_READ_SPACE(nb, size);

	 *
	 * We rely on a characteristic of the IP checksum algorithm where the
	 * result of checksumming a block of data is the same as the byte-swapped
	 * result of checksumming the odd-byte aligned (and zero padded) version
	 * of the same block of data.
	 *
	if (nb->pos & 1) {
		csum = byteswap16(csum);
	}

	netpage_offs_t offs = (netpage_offs_t)(nb->pos & NETPAGE_MASK);
	netpage_t idx = nb->pos >> NETPAGE_SHIFT;

	while (size) {
		u16_t sz;

		if (size + offs > NETPAGE_SIZE) {
			sz = NETPAGE_SIZE - offs;
		} else {
			sz = size;
		}
		size -= sz;

		csum = netpage_ipcsum(csum, nb->pages[idx & NETBUF_MASK], offs, sz);
		offs = 0;
		idx++;
	}

	if (nb->pos & 1) {
		csum = byteswap16(csum);
	}

	return csum ^ 0xffff;
}           */ //20101110

/*
 * netbuf_attach_tail()
 *	Attach a netbuf to the tail of a list of netbufs.
 */
void netbuf_attach_tail(struct netbuf **base, struct netbuf *nb)
{
		struct netbuf **pprev = base;
		struct netbuf *p = *base;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);

		while (p) {
			pprev = &p->next;
			p = p->next;
		}
		*pprev = nb;
		nb->next = NULL;
}

/*
 * Netbuf_attach_head()
 *
 * Providing the routine enables the next pointer to be consider private
 * to the netbuf package.  This routine insert the netbuf at the head of
 * the list.
 */
void netbuf_attach_head(struct netbuf **base, struct netbuf *nb)
{
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		nb->next = *base;
		*base = nb;
}

/*
 * netbuf_detach_head()
 *	Detach the netbuf at the head of a list of netbufs.
 */
struct netbuf *netbuf_detach_head(struct netbuf **base)
{
		struct netbuf *nb = *base;
		if (nb) {
			*base = nb->next;
			RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		}
		return nb;
}

/*
 * netbuf_fwd_strncmp()
 *	Compares a string in the netbuf with a string in data memory
 *	up to a maximum of "count" characters.
 *
 * The behaviour of this function is rather like that of strncmp() but with a small
 * difference - netbuf strings don't need to be zero-terminated because they're
 * implicitly terminated by the netbuf's extent.
 */
int netbuf_fwd_strncmp(struct netbuf *nb, char *s, addr_t count)
{
		u16_t pos;
		s8_t res;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
		
		pos = netbuf_get_pos(nb);
		res = 0;

		while (count--) {
			u8_t nbch;
			if (!netbuf_fwd_check_space(nb, 1)) {
				nbch = 0;
			} else {
				nbch = netbuf_fwd_read_u8(nb);
			}

			res = (s8_t)(nbch - *s++);
			if ((res != 0) || (!nbch)) {
				break;
			}
		}
		
		netbuf_set_pos(nb, pos);	

		return res;
}

/*
 * netbuf_fwd_prog_strncmp()
 *	Compares a string in the netbuf with a string located at the program memory
 *	up to a maximum of "count" characters.
 *
 * The behaviour of this function is rather like that of strncmp() but with a small
 * difference - netbuf strings don't need to be zero-terminated because they're
 * implicitly terminated by the netbuf's extent.
 *
int netbuf_fwd_prog_strncmp(struct netbuf *nb, prog_addr_t str_addr, addr_t count)
{
	RUNTIME_VERIFY_NETBUF_MAGIC(nb);
	
	u16_t pos = netbuf_get_pos(nb);
	s8_t res = 0;

	while (count--) {
		u8_t nbch;
		if (!netbuf_fwd_check_space(nb, 1)) {
			nbch = 0;
		} else {
			nbch = netbuf_fwd_read_u8(nb);
		}

		res = (s8_t)(nbch - progmem_read_u8(str_addr));
		if ((res != 0) || (!nbch)) {
			break;
		}

		str_addr++;
	}
	
	netbuf_set_pos(nb, pos);	

	return res;
}     */    //20101110

/*
 * netbuf_bulk_access()
 */
void netbuf_bulk_access(struct netbuf *nb, netbuf_bulk_access_func_t func, u16_t len)
{
		netpage_t idx;
		netpage_offs_t offs;
		u16_t addr;
		u16_t count;
	
		RUNTIME_VERIFY_NETBUF_MAGIC(nb);
	
		offs = (netpage_offs_t)(nb->pos & NETPAGE_MASK);
		idx = nb->pos >> NETPAGE_SHIFT;
		while (len) {
			if (len + offs > NETPAGE_SIZE) {
				count = NETPAGE_SIZE - offs;
			} else {
				count = len;
			}
			len -= count;
			addr = (nb->pages[idx & NETBUF_MASK] << NETPAGE_SHIFT) | offs;
			func(addr, count);
			offs = 0;
			idx++;
		}
}

/*
 * netbuf_align()
 *
#if defined(IP2K) && defined(NETBUF_ACCELERATED_FUNCTIONS)
bool_t netbuf_align(struct netbuf *nb, u16_t align) __attribute__((section(".pram.netbuf_align")));
#endif
bool_t netbuf_align(struct netbuf *nb, u16_t align)
{
	RUNTIME_VERIFY_NETBUF_MAGIC(nb);
	
	netpage_offs_t shift = nb->start & (align-1);
	if (shift == 0) {
		return TRUE;
	}

	netpage_t firstpg = nb->start >> NETPAGE_SHIFT;
	netpage_t lastpg = (nb->end - 1) >> NETPAGE_SHIFT;

	netpage_t ipg = firstpg;
	while (1) {
		netpage_t pg = nb->pages[ipg & NETBUF_MASK];
		if (netpage_get_refs(pg) == 1) {
			netpage_copy_align(pg, 0, pg, shift, NETPAGE_SIZE - shift);
		} else {
			netpage_t newpg = netpage_alloc();
			if (!newpg) {
				return FALSE;
			}
			netpage_copy_align(newpg, 0, pg, shift, NETPAGE_SIZE - shift);
			netpage_deref(pg);
			nb->pages[ipg & NETBUF_MASK] = newpg;
			pg = newpg;
		}
		if (ipg >= lastpg) {
			break;
		}
		ipg++;
		netpage_copy_align(pg, NETPAGE_SIZE - shift, nb->pages[ipg & NETBUF_MASK], 0, shift);
	}

	nb->start -= shift;
	nb->end -= shift;
	nb->pos -= shift;

	return TRUE;
}     */    //20101110



