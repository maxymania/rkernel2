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
#include <vm/vm_range.h>
#include <vm/vm_top.h>
#include <vm/vm_priv.h>
#include <sysarch/pages.h>
#include <kern/zalloc.h>
#include <string.h>

static zone_t vm_range_zone;  /* Zone for user vm_range structures. */
static zone_t vm_krange_zone; /* Zone for kernel vm_range structures. */
static zone_t vm_crange_zone; /* Zone for critical kernel vm_range structures. */

#ifdef SYSARCH_PAGESIZE_SHIFT
#define DIV_PAGESIZE(x)  ((x)>>SYSARCH_PAGESIZE_SHIFT)
#else
#define DIV_PAGESIZE(x)  ((x)/SYSARCH_PAGESIZE)
#endif

/* This is the size of a cache line (in x86). */
#define CACHE_LINE  128

static u_intptr_t z_range_buf[1<<12] __attribute__ ((aligned (CACHE_LINE)));

void vm_range_init(){
	vm_range_zone = zinit(sizeof(struct vm_range),ZONE_AUTO_REFILL,"user-mode range zone");
	vm_krange_zone = zinit(sizeof(struct vm_range),ZONE_AUTO_REFILL|ZONE_AR_CRITICAL,"kernel-mode range zone");
	vm_crange_zone = zinit(sizeof(struct vm_range),0,"critical kernel-mode range zone");
	zcram(vm_crange_zone,(void*)z_range_buf,sizeof(z_range_buf));
}

void vm_range_refill(){
	vaddr_t begin,size;
	if( zcount(vm_crange_zone) < 128 ){
		size = (vaddr_t)zbufsize(vm_crange_zone) * 256;
		if(!vm_alloc_critical(&begin,&size)) return;
		zcram(vm_crange_zone,(void*)begin,(size_t)size);
	}
}

int vm_range_bmlkup(vm_range_t range, int i){
	int s = i % 32;
	i /= 32;
	return (range->rang_pages_tbm[i]>>s) & 1;
}

void vm_range_bmset(vm_range_t range, int i){
	int s = i % 32;
	i /= 32;
	(range->rang_pages_tbm)[i] |= (1<<s);
}

void vm_range_bmclr(vm_range_t range, int i){
	int s = i % 32;
	i /= 32;
	(range->rang_pages_tbm)[i] &= ~(1<<s);
}

int vm_range_get   (vm_range_t range, vaddr_t rva, paddr_t *pag, vm_prot_t *prot){
	vm_page_t pobj;
	rva = DIV_PAGESIZE(rva);
	for(;;){
		if(!range) return 0;
		if(rva>=VM_RANGE_NUM) {
			range = range->rang_next;
			rva -= VM_RANGE_NUM;
			continue;
		}
		if(vm_range_bmlkup(range,(int)rva)) { // 1 => paddr_t
			*pag = range->rang_pages[rva].page_addr;
		} else {                              // 0 => vm_page_t
			pobj = range->rang_pages[rva].page_obj ;
			if(!pobj) return 0;
			*pag = pobj->pg_phys;
			*prot &= ~(pobj->pg_prohib);
		}
		return -1;
	}
}

vm_range_t vm_range_alloc(int kernel, struct kernslice* slice){
	vm_range_t range = zalloc(kernel ? vm_krange_zone : vm_range_zone);
	if(!range) return 0;
	memset((void*)range,0,sizeof(struct vm_range));
	kernlock_init(&(range->rang_lock));
	range->rang_slice = slice;
	range->rang_refc = 1;
	return range;
}

struct vm_range *vm_range_alloc_critical(struct kernslice* slice){
	vm_range_t range = zalloc(vm_crange_zone);
	if(!range) return 0;
	memset((void*)range,0,sizeof(struct vm_range));
	kernlock_init(&(range->rang_lock));
	range->rang_slice = slice;
	range->rang_refc = 1;
	return range;
}

static void vm_range_destroy(vm_range_t range){
	int i;
	for(i=0;i<VM_RANGE_NUM;++i){
		if(vm_range_bmlkup(range,i))
			vm_page_free(range->rang_slice,range->rang_pages[i].page_addr);
		else	vm_page_drop(range->rang_pages[i].page_obj);
	}
	zfree((void*)range);
}

void vm_range_drop(vm_range_t range){
	vm_range_t next;
	u_int32_t refc;
	
restart:
	kernlock_lock(&(range->rang_lock));
		range->rang_refc--;
		refc = range->rang_refc;
	kernlock_unlock(&(range->rang_lock));
	if(refc) return;
	
	next = range->rang_next;
	vm_range_destroy(range);
	range = next;
	goto restart;
}
