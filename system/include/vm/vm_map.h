/*
 * Copyright (c) 2017 Simon Schmidt
 *
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University.
 * Copyright (c) 1993,1994 The University of Utah and
 * the Computer Systems Laboratory (CSL).
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON, THE UNIVERSITY OF UTAH AND CSL ALLOW FREE USE OF
 * THIS SOFTWARE IN ITS "AS IS" CONDITION, AND DISCLAIM ANY LIABILITY
 * OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF
 * THIS SOFTWARE.
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
#include <vm/pmap.h>
#include <vm/vm_types.h>
#include <sys/kspinlock.h>

#include <utils/list.h>

struct vm_object;
struct vm_map;

/*
 *	Types defined:
 *
 *	vm_map_t		the high-level address map data structure.
 *	vm_map_entry_t		an entry in an address map.
 *	vm_map_version_t	a timestamp of a map, for use with vm_map_lookup
 *	vm_map_copy_t		represents memory copied from an address map,
 *				 used for inter-map copy operations
 */

/*
 *	Type:		vm_map_object_t [internal use only]
 *
 *	Description:
 *		The target of an address mapping, either a virtual
 *		memory object or a sub map (of the kernel map).
 */

typedef union vm_map_object {
	struct vm_object	*vm_object;	/* object object */
	struct vm_map		*sub_map;	/* belongs to another map */
} vm_map_object_t;


/*
 *	Type:		vm_map_entry_t [internal use only]
 *
 *	Description:
 *		A single mapping within an address map.
 *
 *	Implementation:
 *		Address map entries consist of start and end addresses,
 *		a VM object (or sub map) and offset into that object,
 *		and user-exported inheritance and protection information.
 *		Control information for virtual copy operations is also
 *		stored in the address map entry.
 */
struct vm_map_entry {
	list_node_s     links; /* List Entry for parent map entries (links to other entries) */
	vaddr_t         start; /* First valid Virtual Address. */
	vaddr_t         end;   /* Last valid Virtual Address. (start+LENGTH-1) */
	
	list_node_s     child; /* List Entry for child list of parents */
	
	vm_map_object_t object; /* object I point to */
	u_intptr_t      offset; /* offset into object */
	
	struct vm_map   *parent; /* The parent map. */
	
	unsigned int
			is_shared:1,	/* region is shared */
			is_sub_map:1,	/* Is "object" a submap? */
			in_transition:1, /* Entry being changed */
			needs_wakeup:1,  /* Waiters on in_transition */
	/* Only used when object is a vm_object: */
			needs_copy:1; /* does object need to be copied */
	
	/* Only in task maps: */
	
	vm_prot_t       protection;      /* protection code */
	vm_prot_t       max_protection;  /* maximum protection */
	//u_int32_t      inheritance;    /* inheritance */
	//u_int16_t       wired_count;	 /* can be paged if = 0 */
	//u_int16_t       user_wired_count;/* for vm_wire */
	
	/* 0 for normal map entry
	 * or persistent kernel map projected buffer entry;
	 * -1 for non-persistent kernel map projected buffer entry;
	 * pointer to corresponding kernel map entry for user map
	 * projected buffer entry */
	//struct vm_map_entry     *projected_on;
	
};

typedef struct vm_map_entry *vm_map_entry_t;


/*
 *	Type:		vm_map_t [exported; contents invisible]
 *
 *	Description:
 *		An address map -- a directory relating valid
 *		regions of a task's address space to the corresponding
 *		virtual memory objects.
 *
 *	Implementation:
 *		Maps are doubly-linked lists of map entries, sorted
 *		by address.  One hint is used to start
 *		searches again from the last successful search,
 *		insertion, or removal.  Another hint is used to
 *		quickly find free space.
 */
struct vm_map {
	list_node_s  list;       /* Map Entry List */
	list_node_s  parents;    /* List of parent mappings */
	vaddr_t      min_offset; /* start of range */
	vaddr_t      max_offset; /* end of range (max_offset+size-1) */
	
	pmap_t       pmap;       /* Physical map. (NULL for sub-maps and the like.) */
	u_intptr_t   size;       /* virtual size */
	u_int32_t    ref_count;	 /* Reference count */
	
	int /*bool*/ wiring_required; /* All memory wired? */
	
	kspinlock_t  lock;       /* Lock for map data */
	kspinlock_t  ref_lock;   /* Lock for ref_count field */
	kspinlock_t  pmap_lock;  /* Lock for pmap field */
};
typedef struct vm_map *vm_map_t;


void vm_map_init();

vm_map_t vm_map_get_kernel();

/*
 * Insert an Entry into the Map.
 */
int vm_map_insert_entry(
	vm_map_t map,
	vm_map_entry_t entry,
	u_intptr_t size);

/*
 * Remove an Entry from the Map.
 */
int vm_map_remove_entry(vm_map_t map, vm_map_entry_t entry);

vm_map_entry_t vm_map_kentry_alloc();

vm_map_entry_t vm_map_entry_alloc();

