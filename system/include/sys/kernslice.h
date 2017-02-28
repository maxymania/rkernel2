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

#include <machine/types.h>
#include <sys/physmem.h>
#include <sys/kspinlock.h>
#include <utils/list.h>


struct cpu;
struct physmem_bmaset;

/*
 * A kernel Slice represent a Set of CPUs and resources, that belong together.
 * It structurally resembles "NUMA domains". A typical use case is to implement
 * NUMA support by dedicating one kernel Slice to every NUMA domain.
 *
 * However kernel Slices can also be used to split up very large SMP systems,
 * or even to split up large NUMA domains in order to improve performance, when
 * appropriate.
 */
struct kernslice{
	u_intptr_t             ks_kernslice_id;       /* The unique ID of this kernel Slice. */
	struct physmem_range*  ks_memory_ranges;      /* Table of Physical Memory Ranges. */
	u_intptr_t             ks_num_memory_ranges;  /* The length of the Table of Physical Memory Ranges. */
	struct cpu*            ks_cpu_list;           /* The first CPU in the CPU list. Use cpu->cpu_ks_next for the next one. */
	struct physmem_bmaset* ks_memory_allocator;   /* Physical page allocator bitmap-set. */
	
	struct{ /* Raw Memory allocator */
		u_intptr_t     range_idx;             /* Index into ks_memory_ranges */
		paddr_t        pos_in_range;          /* position within ->pm_begin and ->pm_end */
	} ks_raw_memory;
	
	kspinlock_t            ks_memory_raw_lock;    /* Raw memory allocator lock*/
	kspinlock_t            ks_memory_lock;        /* Memory allocator lock*/
	
	list_node_s            ks_memory_free_list;   /* List of Free Pages. */
	list_node_s            ks_memory_fictitious;  /* List of Fictitious Pages. */
	
	u_intptr_t             ks_memory_free_count;
	u_intptr_t             ks_memory_fic_count;
};

