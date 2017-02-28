/*
 *
 * Copyright (c) 2017 Simon Schmidt
 *
 * Mach Operating System
 * Copyright (c) 1993-1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#pragma once
#include <sysarch/paddr.h>
#include <vm/vm_types.h>
#include <sys/kernslice.h>
#include <sys/kspinlock.h>

#include <utils/list.h>

/*
 *	Management of resident (logical) pages.
 *
 *	A small structure is kept for each resident
 *	page, indexed by page number.  Each structure
 *	is an element of several lists:
 *
 *		A hash table bucket used to quickly
 *		perform object/offset lookups
 *
 *		A list of all pages for a given object,
 *		so they can be quickly deactivated at
 *		time of deallocation.
 *
 *		An ordered list of pages due for pageout.
 *
 *	In addition, the structure contains the object
 *	and offset to which this page belongs (for pageout),
 *	and sundry status bits.
 *
 *	Fields in this structure are locked either by the lock on the
 *	object that the page belongs to (O), or by the lock on the page
 *	queues (P), or by it's own lock (L). [Some fields require that
 *      more than one lock is held to change that field; holding either
 *      lock is sufficient to read.]
 *
 *      Lock order is P -> O -> L.
 */

struct vm_object;

struct vm_page {
	list_node_s       pagequeue; /* queue info for FIFO queue or free list (P) */
	
	list_node_s       list;      /* all pages in same object (O) */
	
	struct vm_object* object;    /* which object am I in (O,P) */
	
	u_intptr_t        offset;    /* offset into that object (O,P) */
	
	u_int32_t         pg_refc;   /* Reference count (L). */
	
	u_int16_t
	/* boolean_t */	inactive:1,	/* page is in inactive list (P) */
			active:1,	/* page is in active list (P) */
			laundry:1,	/* page is being cleaned now (P)*/
			free:1,		/* page is on free list (P) */
			reference:1,	/* page has been used (P) */
			:0; /* (force to 'u_int16_t' boundary) */
	
	u_int16_t  /* All fields (O,L). */
	/* boolean_t */	busy:1,		/* page is in transit (O,L) */
			wanted:1,	/* someone is waiting for page (O,L) */
			fictitious:1,	/* Physical page doesn't exist (O,L) */
			is_private:1,	/* Page should not be returned to
					 *  the free list (O,L) */
			absent:1,	/* Data has been requested, but is
					 *  not yet available (O,L) */
			error:1,	/* Data manager was unable to provide
					 *  data due to error (O,L) */
			dirty:1,	/* Page must be cleaned (O,L) */
			precious:1,	/* Page is precious; data must be
					 *  returned even if clean (O,L) */
			overwriting:1,	/* Request to unlock has been made
					 * without having data. (O,L)
					 * [See vm_object_overwrite] */
			:0;
	
	paddr_t           phys_addr;  /* Physical address of page, passed. (valid if fictitious=0) */
	vm_prot_t         page_lock;  /* Uses prohibited by data manager (O) */
	vm_prot_t         unlock_request; /* Outstanding unlock request (O) */

/* Code from the original. */
	struct kernslice* pg_slice;  /* The kslice, this page is belonging to. (L) */
	kspinlock_t       pg_lock;   /* Modification lock. */
};
typedef struct vm_page *vm_page_t;


void      vm_page_free(struct kernslice* slice, paddr_t addr);
void      vm_page_release(vm_page_t page);
void      vm_page_drop(vm_page_t page);
vm_page_t vm_page_grab(struct kernslice* slice);
vm_page_t vm_page_grab_fictitious(struct kernslice* slice);
void      vm_page_zero_fill(vm_page_t page);

