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

int rm_pagefault(pd_t pd,rm_t rm,vaddr_t va, vm_prot_t fault_type){
	int type;
	void* object;
	dataspace_t ds;
	paddr_t paddr;
	struct page_fault fault = { 0,va,fault_type };
	
	if(va <  pd->pd_begin) return VM_SEGFAULT;
	if(va >= pd->pd_end) return VM_SEGFAULT;
	fault.offset  = pd->pd_begin;
	fault.ptr    -= fault.offset;
	
	object = rm;
	do{
		rm = (rm_t) object;
		object = rm_ll_lookup(rm, &fault, &type);
	}while((type==RMO_REGION_MAPPER) && object);
	
	if(!object) return VM_SEGFAULT;
	switch(type){
	case RMO_DATASPACE:
		ds = (dataspace_t) object;
		paddr = ds->ds_pages[fault.ptr/SYSARCH_PAGESIZE];
		
		/* XXX this is preliminary */
		if(!paddr)return VM_SEGFAULT;
		
		pd_enter(pd, va, paddr, fault_type, 0);
		return VM_OK;
	}
	return VM_SEGFAULT;
}

