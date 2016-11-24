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
typedef void* vm_bintree_t; /* TODO: implement the binary tree. */

/*
 * A virtual address space.
 */
struct vm_as {
	vaddr_t       as_begin;
	vaddr_t       as_end;
	vm_bintree_t  as_segs;
	pmap_t        as_pmap;
	kspinlock_t   as_lock;
};

typedef struct vm_as* vm_as_t;

