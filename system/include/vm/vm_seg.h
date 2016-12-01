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
//#include <sysarch/paddr.h>
#include <vm/vm_types.h>
#include <vm/tree.h>
#include <sys/kspinlock.h>

struct vm_as;
typedef struct vm_mem    *vm_mem_t;
typedef struct vm_bstore *vm_bstore_t;

/*
 * Type: vm_seg_t
 *
 * Description:
 *      A single segment within a virtual address space.
 *
 * Implementation:
 *     Address map entries consist of an addresses range, a resident Memory object
 *     (optional), a backing storage (optional) and offset into that backing storage.
 */

struct vm_seg {
	/* The binary tree node structure. */
	struct bintree_node _bt_node;
	
	/* The address-range of the segment. */
	vaddr_t     seg_begin;
	vaddr_t     seg_end;
	
	/* The resident memory object. NULL if the segment is not resident, right now. */
	vm_mem_t    seg_mem;
	
	/* The protection for this segment. */
	vm_prot_t   seg_prot;
	
	/* The backing storage of this segment. NULL, if there is no backing storage. */
	vm_bstore_t seg_bstore;
	u_intptr_t  seg_bstore_offset; /* The offset within the backing store. */
	
	/* The lock for this segment. XXX Maybe replace it with a MUTEX, eventually? */
	kspinlock_t seg_lock;
};

typedef struct vm_seg *vm_seg_t;

void vm_seg_init();

void vm_seg_refill();

vm_seg_t vm_seg_alloc(int kernel);

void vm_seg_initobj(vm_seg_t seg);

int  vm_seg_eager_map(vm_seg_t seg,struct vm_as* as, vm_prot_t prot);

