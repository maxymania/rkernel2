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
#include <vm/vm_top.h>
#include <vm/region_mapper.h>
#include <vm/protection_domain.h>
#include <vm/dataspace.h>
#include <sys/kernslice.h>
#include <sysarch/pages.h>
#include <sys/physmem_alloc.h>
#include <stdio.h>

#ifdef SYSARCH_PAGESIZE_SHIFT
#define MUL_PAGESIZE(x)  ((x)<<SYSARCH_PAGESIZE_SHIFT)
#define DIV_PAGESIZE(x)  ((x)>>SYSARCH_PAGESIZE_SHIFT)
#define ROUND_DOWN(x) x &= ~((1<<SYSARCH_PAGESIZE_SHIFT)-1)
#else
#define MUL_PAGESIZE(x)  ((x)*SYSARCH_PAGESIZE)
#define DIV_PAGESIZE(x)  ((x)/SYSARCH_PAGESIZE)
#define ROUND_DOWN(x) x -= (x%SYSARCH_PAGESIZE)
#endif

#define PMBM(slice) ((slice)->ks_memory_allocator)

#define VM_PROT_KMEM  VM_PROT_READ | VM_PROT_WRITE

static int grow_critical(vaddr_t *addr,vaddr_t *size,int n){
	int i,j;
	list_node_t list;
	rmo_t rmo;
	rm_t rm;
	pd_t pd;
	dataspace_t ds;
	paddr_t* pages;
	paddr_t curpage;
	vaddr_t va,sr;
	struct kernslice* slice;
	
	rm = rm_get_kernel();
	pd = pd_get_kernel();
	
	slice = pmap_kernslice(pd->pd_pmap);
	
	list = &(rm->rm_childs);
	if(list->next==list)return 0;
	rmo = containerof((list->next),struct region_mapper_object,rmo_member);
	if(rmo->rmo_type!=RMO_DATASPACE)return 0;
	
	ds = (dataspace_t) rmo->rmo_object;
	
	pages = ds->ds_pages;
	for( i=0 ; i<(ds->ds_n_pages) ; ++i ) if(!pages[i]) break;
	
	if( i>=(ds->ds_n_pages) ) return 0;
	
	va = pd->pd_begin + rmo->rmo_offset;
	sr = 0;
	va += i * SYSARCH_PAGESIZE;
	
	*addr = va;
	
	for( j=0; j<n ; ++i, ++j, va += SYSARCH_PAGESIZE, sr += SYSARCH_PAGESIZE ){
		if( i==(ds->ds_n_pages) ) break;
		if(!vm_phys_alloc(PMBM(slice),&curpage)) break;
		pages[i] = curpage;
		pd_enter(pd, va, curpage, VM_PROT_KMEM, 0);
	}
	*size = sr;
	
	if(!sr)return 0;
	return 1;
}

int vm_alloc_critical(vaddr_t *addr /* [out] */,vaddr_t *size /* [in/out]*/){
	vaddr_t s;
	
	s  = *size;
	s += SYSARCH_PAGESIZE-1;
	ROUND_DOWN(s);
	printf("Critical: %d\n",(int)((s)>>12));
	
	return grow_critical(addr,size,DIV_PAGESIZE(s));
}

#if 1
int vm_kalloc_ll(vaddr_t *addr /* [out] */,vaddr_t *size /* [in/out]*/){
	vaddr_t s;
	
	s  = *size;
	s += SYSARCH_PAGESIZE-1;
	ROUND_DOWN(s);
	printf("LL: %d\n",(int)((s)>>12));
	
	return grow_critical(addr,size,DIV_PAGESIZE(s));
}
#endif






