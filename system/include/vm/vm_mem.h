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
#include <vm/vm_range.h>
/* #include <sys/kspinlock.h> */

/*
 * Management for resident Memory
 *
 * A small structure is kept for each resident segment: 'vm_mem_t'. The structure
 * contains a reference to the memory itself (eighter a page-address,
 * a vm_page_t object or a vm_range_t object), as well as the sundry status bits.
 *
 * Fields in this structure are locked either by the lock on the segment that the
 * vm_mem_t belongs to.
 *
 * Legend:
 *  (B) : 'vm_bstore_t' aka. "backing storage"
 *  (f) : this information is determined by page-faults and possibly a false
 *        negative.
 *  (x) : provided, the platform supports executable space protection.
 */

/* The type of Physical Memory. */
#define VMM_IS_INVAL   0
#define VMM_IS_PGADDR  1
#define VMM_IS_PGOBJ   2
#define VMM_IS_PMRANGE 3

struct vm_mem {
	/* The Physical Memory. */
	union {
		paddr_t    mem_pgaddr;
		vm_page_t  mem_pgobj;
		vm_range_t mem_pmrange;
	};
	unsigned int
		mem_phys_type : 2,  /* The type of the physical memory. */
		mem_default_ro : 1, /* Default to Read-only, when mapped. */
		mem_default_nx : 1, /* Default to non-executable, when mapped. (x) */
		mem_accessed : 1,   /* The memory has been read or written to. (f) */
		mem_dirty : 1,      /* The memory has been written to. (f) */
		mem_executed : 1,   /* The memory has been executed. (f) (x) */
		mem_precious : 1;   /* Data must be written back, even if clean. (B) */
};

