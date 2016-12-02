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
#include <vm/vm_as.h>
#include <vm/vm_seg.h>
#include <vm/vm_mem.h>
#include <vm/vm_errcode.h>
#include <sysarch/pages.h>
#include <xcpu/vm.h>

#ifdef SYSARCH_PAGESIZE_SHIFT
#define ROUND_DOWN(x) x &= ~((1<<SYSARCH_PAGESIZE_SHIFT)-1)
#else
#define ROUND_DOWN(x) x -= (x%SYSARCH_PAGESIZE)
#endif

#define NOT(x) (!(x))

#define GIVE_UP do{\
		kernlock_unlock(&(as->as_lock)); \
		return VM_FAILURE; \
	} while(0)

#define DO_SEGFAULT do{\
		kernlock_unlock(&(as->as_lock)); \
		return VM_SEGFAULT; \
	} while(0)

static int vm_seg_pagefault(vm_as_t as, vm_seg_t seg, vaddr_t va, vm_prot_t fault_type);

int vm_as_pagefault(vm_as_t as,vaddr_t va, vm_prot_t fault_type) {
	ROUND_DOWN(va);
	vm_bintree_t * __restrict__ bt;
	int ret;
	kernlock_lock(&(as->as_lock));
	
	kernlock_lock(&(as->as_lock_segs));
	bt = bt_floor(&(as->as_segs),va);
	kernlock_unlock(&(as->as_lock_segs));
	
	/*
	 * If no entry has been found, The pointer is not in an valid (mapped) address range.
	 */
	if(!bt) DO_SEGFAULT;
	if(!*bt) DO_SEGFAULT;
	
	ret = vm_seg_pagefault(as,((vm_seg_t)((*bt)->V)),va,fault_type);
	
	kernlock_unlock(&(as->as_lock));
	return ret;
}

#undef GIVE_UP
#undef DO_SEGFAULT

#define GIVE_UP do{\
		kernlock_unlock(&(seg->seg_lock)); \
		return VM_FAILURE; \
	} while(0)

#define DO_SEGFAULT do{\
		kernlock_unlock(&(seg->seg_lock)); \
		return VM_SEGFAULT; \
	} while(0)


static int vm_seg_pagefault(vm_as_t as,vm_seg_t seg, vaddr_t va, vm_prot_t fault_type) {
	paddr_t pa;
	vaddr_t rva;
	vm_prot_t iprod;
	struct vm_mem * mem;
	
	kernlock_lock(&(seg->seg_lock));
	
	/*
	 * If the address is outside the Segment, this is a segmentation fault.
	 */
	if(va > seg->seg_end) DO_SEGFAULT;
	
	rva = va - seg->seg_begin;
	
	/*
	 * Obtain the segment's memory protection.
	 */
	iprod = seg->seg_prot;
	
	/*
	 * Check, if the access type violates the protection.
	 */
	if(fault_type & ~(iprod)) DO_SEGFAULT;
	
	mem = seg->seg_mem;
	
	/* TODO: Pagein. */
	if(!mem) GIVE_UP;
	
	/*
	 * If Read-Only by Default is set on this segment, and no write access
	 * is being performed, then remove write from the privileges.
	 */
	if(mem->mem_default_ro && NOT(mem->mem_dirty   ) && NOT(fault_type & VM_PROT_WRITE  ))
		iprod &= ~VM_PROT_WRITE  ;
	
	/*
	 * If No-Execute by Default is set on this segment, and no execute access
	 * is being performed, then remove execute from the privileges.
	 */
	if(mem->mem_default_nx && NOT(mem->mem_executed) && NOT(fault_type & VM_PROT_EXECUTE))
		iprod &= ~VM_PROT_EXECUTE;
	
	/*
	 * If the lookup failes, give up. XXX: It shouldn't behave like this.
	 */
	if(!vm_mem_lookup(mem,rva, &pa, &iprod)) GIVE_UP;
	
	/*
	 * Check, if the access type violates those protection imposed by individual
	 * page-objects (if any).
	 */
	if(fault_type & ~(iprod)) DO_SEGFAULT;
	
	/*
	 * Based on the fault-type, some of the following bits will be set: the 'accessed-bit',
	 * the 'dirty-bit' and the 'executed-bit'.
	 */
	if(fault_type & VM_PROT_WRITE  ) mem->mem_dirty    = 1;
	if(fault_type & VM_PROT_EXECUTE) mem->mem_executed = 1;
	mem->mem_accessed = 1;
	
	/*
	 * Flush the cache on the given Page address only for the current pmap_t.
	 */
	xcpu_cache_flush_page(as->as_pmap,va);
	
	/*
	 * Map the memory. If it failes, give up.
	 */
	kernlock_lock(&(as->as_lock_pmap));
	
	if( pmap_enter(as->as_pmap,va,pa,iprod,0) ) {
		kernlock_unlock(&(as->as_lock_pmap));
		GIVE_UP;
	}
	
	kernlock_unlock(&(as->as_lock_pmap));
	
	/*
	 * Flush the faulted page on the given Page address only for the current pmap_t.
	 */
	xcpu_tlb_flush_page(as->as_pmap,va);
	
	kernlock_unlock(&(seg->seg_lock));
	return VM_OK;
}

