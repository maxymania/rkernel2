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
#include <vm/vm_mem.h>
#include <vm/vm_top.h>
#include <vm/vm_priv.h>
#include <kern/zalloc.h>
#include <string.h>

static zone_t vm_mem_zone;  /* Zone for user vm_mem structures. */
static zone_t vm_kmem_zone; /* Zone for kernel vm_mem structures. */
static zone_t vm_cmem_zone; /* Zone for critical kernel vm_mem structures. */

/* This is the size of a cache line (in x86). */
#define CACHE_LINE  128

static u_int8_t z_mem_buf[1<<12] __attribute__ ((aligned (CACHE_LINE)));

void vm_mem_init(){
	vm_mem_zone = zinit(sizeof(struct vm_mem),ZONE_AUTO_REFILL,"user-mode memory zone");
	vm_kmem_zone = zinit(sizeof(struct vm_mem),
	               ZONE_AUTO_REFILL|ZONE_AR_CRITICAL,"kernel-mode memory zone");
	vm_cmem_zone = zinit(sizeof(struct vm_mem),0,"critical kernel-mode memory zone");
	zcram(vm_cmem_zone,(void*)z_mem_buf,sizeof(z_mem_buf));
}

void vm_mem_refill(){
	vaddr_t begin,size;
	if( zcount(vm_cmem_zone) < 64 ){
		size = (vaddr_t)zbufsize(vm_cmem_zone) * 256;
		if(!vm_alloc_critical(&begin,&size)) return;
		zcram(vm_cmem_zone,(void*)begin,(size_t)size);
	}
}

int vm_mem_lookup(struct vm_mem* mem, vaddr_t rva, paddr_t *pag, vm_prot_t *prot){
	vm_page_t pgobj;
	switch(mem->mem_phys_type){
	case VMM_IS_PGADDR:
		*pag = mem->mem_pgaddr;
		return -1;
	case VMM_IS_PGOBJ:
		pgobj = mem->mem_pgobj;
		if(!pgobj)return 0;
		*pag = pgobj->pg_phys;
		*prot &= ~(pgobj->pg_prohib);
		return -1;
	case VMM_IS_PMRANGE:
		return vm_range_get(mem->mem_pmrange,rva,pag,prot);
	}
	return 0;
}

struct vm_mem* vm_mem_alloc(int kernel){
	struct vm_mem* mem = zalloc(kernel ? vm_kmem_zone : vm_mem_zone);
	if(!mem) return 0;
	memset((void*)mem,0,sizeof(struct vm_mem));
	return mem;
}

struct vm_mem* vm_mem_alloc_critical(){
	struct vm_mem* mem = zalloc(vm_cmem_zone);
	if(!mem) return 0;
	memset((void*)mem,0,sizeof(struct vm_mem));
	return mem;
}

