/*
 * Copyright (c) 2017 Simon Schmidt
 *
 * Mach Operating System
 * Copyright (c) 1993-1987 Carnegie Mellon University
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
#include <vm/vm_types.h>
#include <sys/kspinlock.h>

#include <utils/list.h>


/*
 *	Types defined:
 *
 *	vm_object_t		Virtual memory object.
 *
 */
struct vm_object {
	list_node_s     memq;      /* Resident memory */
	list_node_s     parents;   /* List of parent mappings */
	u_int32_t       size;      /*Object size (only valid if internal) */
	
	u_int16_t       ref_count; /* Number of references */
	u_int16_t       resident_page_count; /* number of resident pages */

	//struct vm_object *copy; /* Object that should receive a copy of my changed pages */
	//struct vm_object *shadow; /* My shadow */
	
	u_intptr_t      shadow_offset; /* Offset into shadow */
	
	//struct ipc_port		*pager;		/* Where to get data */
	//vm_offset_t		paging_offset;	/* Offset into memory object */
	//pager_request_t		pager_request;	/* Where data comes back */
	//struct ipc_port		*pager_name;	/* How to identify region */

	u_int32_t	copy_strategy;	/* How to handle data copy */

	u_int32_t       absent_count;	/* The number of pages that
					 * have been requested but
					 * not filled.  That is, the
					 * number of pages for which
					 * the "absent" attribute is
					 * asserted.
					 */
	
	u_int32_t       all_wanted;	/* Bit array of "want to be
					 * awakened" notations.  See
					 * VM_OBJECT_EVENT_* items
					 * below
					 */
	
	
	unsigned int
				paging_in_progress:16,
						/* The memory object ports are
						 * being used (e.g., for pagein
						 * or pageout) -- don't change any
						 * of these fields (i.e., don't
						 * collapse, destroy or terminate)
						 */
	/* boolean_t */		pager_created:1,/* Has pager ever been created? */
	/* boolean_t */		pager_initialized:1,/* Are fields ready to use? */
	/* boolean_t */		pager_ready:1,	/* Will manager take requests? */

	/* boolean_t */		can_persist:1,	/* The kernel may keep the data
						 * for this object (and rights to
						 * the memory object) after all
						 * address map references are
						 * deallocated?
						 */
	/* boolean_t */		internal:1,	/* Created by the kernel (and
						 * therefore, managed by the
						 * default memory manger)
						 */
	/* boolean_t */		temporary:1,	/* Permanent objects may be changed
						 * externally by the memory manager,
						 * and changes made in memory must
						 * be reflected back to the memory
						 * manager.  Temporary objects lack
						 * both of these characteristics.
						 */
	/* boolean_t */		alive:1,	/* Not yet terminated (debug) */
	/* boolean_t */		lock_in_progress : 1,
						/* Is a multi-page lock
						 * request in progress?
						 */
	/* boolean_t */		lock_restart : 1,
						/* Should lock request in
						 * progress restart search?
						 */
	/* boolean_t */		use_old_pageout : 1,
						/* Use old pageout primitives? 
						 */
	/* boolean_t */		use_shared_copy : 1,
						/* Use shared (i.e.,
						 * delayed) copy on write */
	/* boolean_t */		shadowed: 1;	/* Shadow may exist */
	
	kspinlock_t		Lock;		/* Synchronization */
	
	//list_node_s		cached_list;
						/* Attachment point for the list
						 * of objects cached as a result
						 * of their can_persist value
						 */
	//u_intptr_t		last_alloc;	/* last allocation offset */
	
};
typedef struct vm_object *vm_object_t;

void vm_object_init();

vm_object_t vm_object_alloc();


