/*
 *
 * Copyright (c) 2016 Simon Schmidt
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
 *	object that the page belongs to (O) or by the lock on the page
 *	queues (P).  [Some fields require that both locks be held to
 *	change that field; holding either lock is sufficient to read.]
 */

/* page queue status bits. (P) */
enum {
	VM_PQ_INACTIVE  = 1,    /* page is in inactive list (P) */
	VM_PQ_ACTIVE    = 2,    /* page is in active list (P) */
	VM_PQ_LAUNDRY   = 4,    /* page is being cleaned now (P)*/
	VM_PQ_FREE      = 8,    /* page is on free list (P) */
	VM_PQ_REFERENCE = 0x10, /* page has been used (P) */
};
/* object status bits. (O) */
enum {
	VM_PAGE_BUSY        = 0x001,	/* page is in transit (O) */
	VM_PAGE_WANTED      = 0x002,	/* someone is waiting for page (O) */
	VM_PAGE_TABLED      = 0x004,	/* page is in VP table (O) */
	VM_PAGE_FICTITIOUS  = 0x008,	/* Physical page doesn't exist (O) */
	VM_PAGE_ISPRIVATE   = 0x010,	/* Page should not be returned to
					 *  the free list (O) */
	VM_PAGE_ABSENT      = 0x020,	/* Data has been requested, but is
					 *  not yet available (O) */
	VM_PAGE_ERROR       = 0x040,	/* Data manager was unable to provide
					 *  data due to error (O) */
	VM_PAGE_DIRTY       = 0x080,	/* Page must be cleaned (O) */
	VM_PAGE_PRECIOUS    = 0x100,	/* Page is precious; data must be
					 *  returned even if clean (O) */
	VM_PAGE_OVERWRITING = 0x200,	/* Request to unlock has been made
					 * without having data. (O)
					 * [See vm_object_overwrite] */
};

struct vm_object;

struct vm_page {
	list_node_s       pagequeue; /* queue info for FIFO queue or free list (P) */
	
	list_node_s       list;      /* all pages in same object (O) */
	
	//struct vm_page *next; /* VP bucket link (O) */
	
	struct vm_object* object;    /* which object am I in (O,P) */
	
	u_intptr_t        offset;    /* offset into that object (O,P) */
	
	u_int32_t         pg_refc;   /* Reference count. */
	
	u_int16_t         page_queue_flags; /* page queue status bits. (P) */
	u_int16_t         object_flags;     /* object status bits. (O) */
	
	
	paddr_t           phys_addr;  /* Physical address of page, passed */
	vm_prot_t         page_lock;  /* Uses prohibited by data manager (O) */
	vm_prot_t         unlock_request; /* Outstanding unlock request (O) */

/* Code from the original. */
	struct kernslice* pg_slice;  /* The kslice, this page is belonging to. */
	kspinlock_t       pg_lock;   /* Modification lock. */
};
typedef struct vm_page *vm_page_t;

/* Code from the original. */
void vm_page_free(struct kernslice* slice, paddr_t addr);

void vm_page_drop(vm_page_t page);


