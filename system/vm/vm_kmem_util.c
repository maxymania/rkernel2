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
#include <vm/vm_top.h>
#include <vm/vm_as.h>
#include <vm/vm_seg.h>
#include <vm/vm_mem.h>
#include <vm/vm_range.h>
#include <vm/vm_priv.h>
#include <sysarch/pages.h>
#include <sys/kernslice.h>
#include <sys/physmem_alloc.h>
#include <vm/pmap.h>
#include <kern/zalloc.h>

#ifdef SYSARCH_PAGESIZE_SHIFT
#define MUL_PAGESIZE(x)  ((x)<<SYSARCH_PAGESIZE_SHIFT)
#define DIV_PAGESIZE(x)  ((x)>>SYSARCH_PAGESIZE_SHIFT)
#define ROUND_DOWN(x) x &= ~((1<<SYSARCH_PAGESIZE_SHIFT)-1)
#else
#define MUL_PAGESIZE(x)  ((x)*SYSARCH_PAGESIZE)
#define DIV_PAGESIZE(x)  ((x)/SYSARCH_PAGESIZE)
#define ROUND_DOWN(x) x -= (x%SYSARCH_PAGESIZE)
#endif

#define NORMAL   0
#define CRITICAL 1

#define PMBM(slice) ((slice)->ks_memory_allocator)

#define VM_PROT_KMEM  VM_PROT_READ | VM_PROT_WRITE

static struct vm_mem* vm_mem_kfilled(vaddr_t size,struct kernslice* slice, int level){
	/*
	 * Assumption: size is a multiple of SYSARCH_PAGESIZE.
	 */
	vaddr_t N = DIV_PAGESIZE(size); /* XXX: this should be rounded up by default. */
	vaddr_t i,j,M;
	paddr_t page;
	vm_range_t range,next = 0;
	
	/* This shouldn't happen. */
	if(N<1) return 0;
	
	/*
	 * Allocate the memory structure.
	 */
	vm_mem_t mem;
	if(level) mem = vm_mem_alloc_critical();
	else      mem = vm_mem_alloc(1);
	if(!mem)return 0;
	
	/*
	 * If the mapping is only one memory page big, put-in the page directly.
	 */
	if(N==1){
		if(!vm_phys_alloc(PMBM(slice),&page)) goto FAILED;
		mem->mem_phys_type = VMM_IS_PGADDR;
		mem->mem_pgaddr = page;
		return mem;
	}
	
	/*
	 * Otherwise, allocate one or more vm_range_t structs.
	 */
	if(level) range = vm_range_alloc_critical(slice);
	else      range = vm_range_alloc(1,slice);
	if(!range) goto FAILED;
	mem->mem_pmrange = range;
	mem->mem_phys_type = VMM_IS_PMRANGE;
	
	/*
	 * Allocate Physical Memory and put it into the vm_range_t
	 */
	M = N;
	if(M>VM_RANGE_NUM) M = VM_RANGE_NUM;
	for(j=0;j<M;++j){
		if(!vm_phys_alloc(PMBM(slice),&page)) goto FAILED2;
		range->rang_pages[j].page_addr = page;
		vm_range_bmset(range,j);
	}
	
	/*
	 * Allocate subsequent ranges.
	 */
	for(i=VM_RANGE_NUM;i<N;i+=VM_RANGE_NUM){
		/*
		 * Allocate another range and append it to the list.
		 */
		if(level) next = vm_range_alloc_critical(slice);
		else      next = vm_range_alloc(1,slice);
		if(!next) goto FAILED;
		range->rang_next = next;
		range = next;
		
		/*
		 * XXX: This is duplicate code. See above.
		 */
		M = N-i;
		if(M>VM_RANGE_NUM) M = VM_RANGE_NUM;
		for(j=0;j<M;++j){
			if(!vm_phys_alloc(PMBM(slice),&page)) goto FAILED2;
			range->rang_pages[j].page_addr = page;
			vm_range_bmset(range,j);
		}
	}
	return mem;
	
	/*
	 * On error, we need to free allocated resources.
	 */
	FAILED2:
	range = mem->mem_pmrange;
	while(range){
		for(j=0;j<VM_RANGE_NUM;++j){
			if(vm_range_bmlkup(range,j))
				vm_phys_free(PMBM(slice),range->rang_pages[j].page_addr);
		}
		next = range->rang_next;
		zfree(range);
		range = next;
	}
	FAILED:
	zfree(mem);
	return 0;
}

static int vm_seg_kfill(vm_seg_t seg,pmap_t pmap, int level){
	vaddr_t size = (seg->seg_end - seg->seg_begin)+1;
	seg->seg_prot = VM_PROT_KMEM;
	seg->seg_mem = vm_mem_kfilled(size,pmap_kernslice(pmap),level);
	if(!(seg->seg_mem)){
		pmap_remove(pmap,seg->seg_begin,seg->seg_end);
		return 0;
	}
	return 1;
}

int vm_kalloc_ll(vaddr_t *addr /* [out] */,vaddr_t *size /* [in/out]*/){
	int res = 0;
	vm_as_t as;
	vm_seg_t seg;
	
	/*
	 * Round-Up the size.
	 */
	vaddr_t lsiz = *size + SYSARCH_PAGESIZE - 1;
	ROUND_DOWN(lsiz);
	
	
	as = vm_as_get_kernel();
	seg = vm_seg_alloc(1);
	if(!seg) return res;
	
	kernlock_lock(&(as->as_lock));
	
	if(! vm_insert_entry(as,lsiz,seg)) {
		zfree((void*)seg);
		goto endKalloc;
	}
	kernlock_lock(&(seg->seg_lock));
	kernlock_unlock(&(as->as_lock));
	
	if(!vm_seg_kfill(seg,as->as_pmap,NORMAL)) goto endKalloc2; /* TODO: Remove segment. */
	
	if(!vm_seg_eager_map(seg,as,VM_PROT_KMEM)) goto endKalloc2; /* TODO: Deallocate. */
	
	*addr = seg->seg_begin;
	*size = (seg->seg_end - seg->seg_begin)+1;
	res = 1;
	
endKalloc2:
	kernlock_unlock(&(seg->seg_lock));
	return res;
endKalloc:
	kernlock_unlock(&(as->as_lock));
	return res;
}

int vm_alloc_critical(vaddr_t *addr /* [out] */,vaddr_t *size /* [in/out]*/){
	int res = 0;
	vm_as_t as;
	vm_seg_t seg;
	
	/*
	 * Round-Up the size.
	 */
	vaddr_t lsiz = *size + SYSARCH_PAGESIZE - 1;
	ROUND_DOWN(lsiz);
	
	as = vm_as_get_kernel();
	seg = vm_seg_alloc_critical();
	if(!seg) return res;
	
	kernlock_lock(&(as->as_lock));
	
	if(! vm_insert_entry(as,lsiz,seg)) {
		zfree((void*)seg);
		goto endKalloc;
	}
	kernlock_lock(&(seg->seg_lock));
	kernlock_unlock(&(as->as_lock));
	
	if(!vm_seg_kfill(seg,as->as_pmap,CRITICAL)) goto endKalloc2; /* TODO: Remove segment. */
	
	if(!vm_seg_eager_map(seg,as,VM_PROT_KMEM)) goto endKalloc2; /* TODO: Deallocate. */
	
	*addr = seg->seg_begin;
	*size = (seg->seg_end - seg->seg_begin)+1;
	res = 1;
	
endKalloc2:
	kernlock_unlock(&(seg->seg_lock));
	return res;
endKalloc:
	kernlock_unlock(&(as->as_lock));
	return res;
}

