/*
 * 
 * Copyright (c) 2016 Simon Schmidt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
#include <vm/vm_types.h>
#include <vm/pmap.h>
#include <vm/tree.h>
#include <sys/kspinlock.h>

/*
 * Type: vm_bintree_t
 *
 * Description:
 *      A table of segments ('vm_seg_t' objects) in this address space, indexed
 *      after their address ranges.
 *
 * Implementation:
 *      The index consists of a Binary Searcht Tree using the begin-attribute of
 *      the address range ('vm_seg_t->seg_begin') as key. Lookups are performed by
 *      first doing a floor-lookup on the tree, so that an entry is returned,
 *      which's key is eighter equals or lower than the address (L), the lookup
 *      searches for, and then, it is checked, wether or not the address (L) is
 *      in the range of the segment.
 */
typedef struct bintree_node* vm_bintree_t;
struct vm_seg;

/*
 * A virtual address space.
 */
struct vm_as {
	vaddr_t       as_begin;
	vaddr_t       as_end;
	vm_bintree_t  as_segs;
	pmap_t        as_pmap;
	
	/*
	 * vm_as_t uses a fine-grained locking model.
	 */
	kspinlock_t   as_lock_pmap; /* protects ->as_pmap */
	kspinlock_t   as_lock_segs; /* protects ->as_segs */
	kspinlock_t   as_lock;      /* cruft */
};

typedef struct vm_as* vm_as_t;

void vm_as_init();

/* Crams a minimal amount of Memory into the vm_as_t object pool. */
void vm_as_mcram();

int vm_as_pagefault(vm_as_t as,vaddr_t va, vm_prot_t fault_type);

vm_as_t vm_as_get_kernel();

/* It is recommended to hold the 'struct vm_seg' lock in the vm_*_entry() functions. */

int vm_insert_entry(vm_as_t as, vaddr_t size, struct vm_seg * seg);

int vm_remove_entry(vm_as_t as, struct vm_seg * seg);

