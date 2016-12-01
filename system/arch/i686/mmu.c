/*
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
#include <x86/pmap.h>
#include <vm/tlb_cache.h>
#include <x86/x86.h>
#include <x86/mmu.h>
#include <sys/cpu.h>
#include <sys/kernslice.h>
#include <sys/physmem_alloc.h>

//#include <stdio.h>

static struct pmap p_inst_kernel;

extern pte_t _i686_kernel_page_dir[];

void  _i686_pmap_pte_set(paddr_t pa,int i,pte_t pte);
pte_t _i686_pmap_pte_get(paddr_t pa,int i);
void  _i686_pmap_pdinit(paddr_t pa);

//static int is_kernel(pmap_t pmap){
//	return ((&p_inst_kernel) == pmap)?1:0;
//}

/*
 * Initializes the MMU subsystem of the HAL.
 */
void pmap_init(){
	int i;
	paddr_t phys;
	struct physmem_bmaset* bmas;
	
	p_inst_kernel.slice = kernel_get_current_cpu()->cpu_kernel_slice;
	bmas = p_inst_kernel.slice->ks_memory_allocator;
	p_inst_kernel.pdir = ((u_intptr_t)_i686_kernel_page_dir)-0xC0000000;
	p_inst_kernel.vab  = (u_intptr_t)0xC1000000;
	p_inst_kernel.vae  = (u_intptr_t)0xFFFFFFFF;
	/* We have already initialized the following page tables in boot.s: 768 .. 768+3 */
	for(i = 768+4; i<1024; ++i){
		if(vm_phys_alloc(bmas, &phys)){
			pmap_zero_page(phys);
			phys |= PTE_PW;
		}else phys = 0;
		_i686_kernel_page_dir[i] = phys;
	}
}

/*
 * This function returns the kernels address space instance. Don't 'pmap_destroy()' it.
 */
pmap_t pmap_kernel(){
	return &p_inst_kernel;
}

/*
 * Creates a virtual address space instance/Page table.
 */
pmap_t pmap_create(){
	return 0;
}

/*
 * Destroys a virtual address space instance. Don't use it on 'pmap_kernel()'.
 */
void pmap_destroy(pmap_t pmap){
}

/* 
 * Returns the Kernel-slice, this pagetable allocates it's memory from.
 */
struct kernslice* pmap_kernslice(pmap_t pmap){
	return 0;
}

/* 
 * Retrieves the bounds of the virtual address space/address range of the current
 * address space.
 *
 * The range is [ vstartp , vendp+1 ) or [ vstartp , vendp ] .
 */
void pmap_get_address_range(pmap_t pmap, vaddr_t *vstartp, vaddr_t *vendp){
}

/*
 * Maps a given physical memory page (pa) to a given virtual address (va).
 */
int pmap_enter(pmap_t pmap, vaddr_t va, paddr_t pa, vm_prot_t prot, vm_flags_t flags){
	unsigned int i = PDX(va);
	unsigned int j = PTX(va);
	paddr_t pta;
	pte_t   pte = PTE_ADDR(pa) | PTE_P;
	
	if(prot & VM_PROT_WRITE) pte |= PTE_W;
	
	if(pmap == &p_inst_kernel){
		pta = _i686_kernel_page_dir[i];
		if(!PTE_FLAGS(pta)) return 0;
		_i686_pmap_pte_set(PTE_ADDR(pta),j,pte);
		return 1;
	}
	
	
	return 0;
}

/*
 * Unmap a certain range of virtual addresses from this address space.
 */
int pmap_remove(pmap_t pmap, vaddr_t vab, vaddr_t vae){
	return 0;
}

/*
 * Unmap all pages from this address space.
 */
int pmap_remove_all(pmap_t pmap){
	return 0;
}

/*
 * Changes the mappings in the range by replacing the memory protection.
 */
int pmap_protect(pmap_t pmap, vaddr_t vab, vaddr_t vae, vm_prot_t prot){
	return 0;
}


/*
 * ----------------------------------------------
 * | TLB management (local CPU/local CPU-core). |
 * ----------------------------------------------
 */

/*
 * TLB flush all.
 */
void mmu_tlb_flush_all(){
	asm volatile("movl %%cr3,%%eax;movl %%eax,%%cr3" ::);
}

/*
 * Flush a range of page-mappings from the TLB.
 */
void mmu_tlb_flush_range(vaddr_t begin, vaddr_t end){
	begin &= ~0xfff;
	end   &= ~0xfff;
	for(;begin<=end;begin+=0x1000)
		invlpg((void*)begin);
}

/*
 * Flush a single page-mapping from the TLB.
 */
void mmu_tlb_flush_page(vaddr_t pos){
	invlpg((void*)pos);
}

/*
 * The i686 Architecture is not Virtually Indexed, so these functions don't need to be implemented.
 */

/*
 * TLB flush all.
 */
void mmu_cache_flush_all(){}

/*
 * Flush a range of page-mappings from the TLB.
 */
void mmu_cache_flush_range(vaddr_t begin, vaddr_t end){
	(void)begin;
	(void)end;
}

/*
 * Flush a single page-mapping from the TLB.
 */
void mmu_cache_flush_page(vaddr_t pos){
	(void)pos;
}

