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
#include <vm/vm_top.h>
#include <vm/vm_priv.h>
#include <vm/pmap.h>
#include <sysarch/pages.h>
#include <kern/zalloc.h>
#include <string.h>

/* This is the size of a cache line (in x86). */
#define CACHE_LINE  128

static u_intptr_t z_seg_buf[1<<10] __attribute__ ((aligned (CACHE_LINE)));

static zone_t vm_seg_zone;  /* Zone for user vm_seg structures. */
static zone_t vm_kseg_zone; /* Zone for kernel vm_seg structures. */
static zone_t vm_cseg_zone; /* Zone for critical kernel vm_seg structures. */

void vm_seg_init(){
	vm_seg_zone = zinit(sizeof(struct vm_seg),ZONE_AUTO_REFILL,"user-mode segment zone");
	vm_kseg_zone = zinit(sizeof(struct vm_seg),ZONE_AUTO_REFILL|ZONE_AR_CRITICAL,"kernel-mode segment zone");
	vm_cseg_zone = zinit(sizeof(struct vm_seg),0,"critical kernel-mode segment zone");
	zcram(vm_cseg_zone,(void*)z_seg_buf,sizeof(z_seg_buf));
}

void vm_seg_refill(){
	vaddr_t begin,size;
	if( zcount(vm_cseg_zone) < 64 ){
		size = (vaddr_t)zbufsize(vm_cseg_zone) * 128;
		if(!vm_alloc_critical(&begin,&size)) return;
		zcram(vm_cseg_zone,(void*)begin,(size_t)size);
	}
}

vm_seg_t vm_seg_alloc(int kernel){
	vm_seg_t seg = zalloc(kernel ? vm_kseg_zone : vm_seg_zone);
	if(!seg) return 0;
	memset((void*)seg,0,sizeof(struct vm_seg));
	kernlock_init(&(seg->seg_lock));
	return seg;
}

struct vm_seg *vm_seg_alloc_critical(){
	vm_seg_t seg = zalloc(vm_cseg_zone);
	if(!seg) return 0;
	memset((void*)seg,0,sizeof(struct vm_seg));
	kernlock_init(&(seg->seg_lock));
	return seg;
}

void vm_seg_initobj(vm_seg_t seg){
	seg->_bt_node.V = seg;
	seg->_bt_node.K = seg->seg_begin;
}

int  vm_seg_eager_map(vm_seg_t seg,struct vm_as* as, vm_prot_t prot) {
	int ret = 0;
	vaddr_t begin,cursor,size;
	paddr_t pa;
	vm_prot_t iprod;
	vm_mem_t mem;
	
	mem = seg->seg_mem;
	if(!mem) return ret;
	
	begin = seg->seg_begin;
	cursor = 0;
	size   = (seg->seg_end - seg->seg_begin)+1;
	
	kernlock_lock(&(as->as_lock_pmap));
	
	for(;cursor<size; cursor += SYSARCH_PAGESIZE){
		iprod = prot;
		if(! vm_mem_lookup(mem,cursor,&pa,&iprod) ) goto endEM;
		if( pmap_enter(as->as_pmap,begin+cursor,pa,iprod,0) ) goto endEM;
	}
	ret = 1;
endEM:
	kernlock_unlock(&(as->as_lock_pmap));
	return ret;
}

