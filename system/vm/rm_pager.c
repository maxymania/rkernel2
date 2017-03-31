/*
 * 
 * Copyright (c) 2017 Simon Schmidt
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
#include <vm/rm_pager.h>
#include <vm/vm_errcode.h>

#ifdef SYSARCH_PAGESIZE_SHIFT
#define MUL_PAGESIZE(x)  ((x)<<SYSARCH_PAGESIZE_SHIFT)
#define DIV_PAGESIZE(x)  ((x)>>SYSARCH_PAGESIZE_SHIFT)
#define ROUND_DOWN(x) x &= ~((1<<SYSARCH_PAGESIZE_SHIFT)-1)
#else
#define MUL_PAGESIZE(x)  ((x)*SYSARCH_PAGESIZE)
#define DIV_PAGESIZE(x)  ((x)/SYSARCH_PAGESIZE)
#define ROUND_DOWN(x) x -= (x%SYSARCH_PAGESIZE)
#endif

static void dataspace_map(pd_t pd,dataspace_t ds,vaddr_t base,vaddr_t size, vm_prot_t prot){
	unsigned int i,n;
	paddr_t *pages;
	if(size > (ds->ds_size)) size = ds->ds_size;
	n = (unsigned int) DIV_PAGESIZE(size);
	if(n > (ds->ds_n_pages)) n = ds->ds_n_pages;
	pages = ds->ds_pages;
	for(i=0;i<n;++i, base += SYSARCH_PAGESIZE )
		pd_enter(pd,base,pages[i],prot,0);
}

int rm_dispatch_pagefault(pd_t pd,void** object,int *type,struct page_fault* pfo){
	switch(*type){
	case RMO_UNDEFINED: return VM_FAILURE;
	case RMO_DATASPACE:
		dataspace_map(pd,(dataspace_t) *object,pfo->offset,pfo->size,pfo->type);
		return VM_OK;
	case RMO_REGION_MAPPER:
		*object = rm_ll_lookup((rm_t) (*object), pfo, type);
		return VM_AGAIN;
	}
	
	/* We should not get here! */
	return VM_FAILURE;
}

int rm_pagefault(pd_t pd,rm_t rm,vaddr_t va, vm_prot_t fault_type){
	int type,ret;
	void* object;
	//int typbak;
	//void* objbak;
	
	struct page_fault fault;
	
	/*
	 * SEGFAULT if virtual address below start address.
	 */
	if(va < pd->pd_begin) return VM_SEGFAULT;
	
	fault.ptr     = va;
	fault.type    = fault_type;
	fault.size    = pd->pd_end;
	fault.offset  = pd->pd_begin;
	fault.ptr    -= fault.offset;
	fault.size   -= fault.offset;
	
	/*
	 * ROUND_UP( fault.size );
	 */
	fault.size   += SYSARCH_PAGESIZE-1;
	ROUND_DOWN(fault.size);
	
	/*
	 * SEGFAULT if virtual address above end address/limit.
	 */
	if(fault.ptr >= fault.size) return VM_SEGFAULT;
	
	object = rm;
	type = RMO_REGION_MAPPER;
	for(;;){
		//objbak = object;
		//typbak = type;
		ret = rm_dispatch_pagefault(pd,&object,&type,&fault);
		if(ret==VM_AGAIN)continue;
		//if(ret==VM_FAILURE){}
		return ret;
	}
	/* Unreachable. */
	__builtin_unreachable();
	//return VM_SEGFAULT;
}

