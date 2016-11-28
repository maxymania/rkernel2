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

#define NOT(x) (!(x))

#define GIVE_UP do{\
		kernlock_unlock(&(as->as_lock)); \
		return VM_FAILURE; \
	} while(0)

#define DO_SEGFAULT do{\
		kernlock_unlock(&(as->as_lock)); \
		return VM_SEGFAULT; \
	} while(0)

static int vm_seg_pagefault(vm_as_t as, vm_seg_t seg, vaddr_t va, vm_prot_t prot);

int vm_as_pagefault(vm_as_t as,vaddr_t va, vm_prot_t prot) {
	struct bintree_node **bt;
	int ret;
	kernlock_lock(&(as->as_lock));
	
	bt = bt_floor(&(as->as_segs),va);
	if(!bt) DO_SEGFAULT;
	if(!*bt) DO_SEGFAULT;
	
	ret = vm_seg_pagefault(as,((vm_seg_t)((*bt)->V)),va,prot);
	
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


static int vm_seg_pagefault(vm_as_t as,vm_seg_t seg, vaddr_t va, vm_prot_t prot) {
	paddr_t pa;
	vm_prot_t iprod;
	struct vm_mem * mem;
	
	kernlock_lock(&(seg->seg_lock));
	va -= seg->seg_begin;
	
	if(prot & ~(seg->seg_prot)) DO_SEGFAULT;
	iprod = seg->seg_prot;
	
	mem = seg->seg_mem;
	
	if(!mem) GIVE_UP;
	
	if(mem->mem_default_ro && NOT(mem->mem_dirty   ) && NOT(prot & VM_PROT_WRITE  ))
	iprod &= ~VM_PROT_WRITE  ;
	
	if(mem->mem_default_nx && NOT(mem->mem_executed) && NOT(prot & VM_PROT_EXECUTE))
	iprod &= ~VM_PROT_EXECUTE;
	
	if(!vm_mem_lookup(mem,va, &pa, &iprod)) GIVE_UP;
	if(prot & ~(iprod)) DO_SEGFAULT;
	
	if(prot & VM_PROT_WRITE  ) mem->mem_dirty    = 1;
	if(prot & VM_PROT_EXECUTE) mem->mem_executed = 1;
	mem->mem_accessed = 1;
	
	pmap_enter(as->as_pmap,va,pa,iprod,0);
	
	kernlock_unlock(&(seg->seg_lock));
	return VM_OK;
}

