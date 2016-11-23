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
#include <x86/x86.h>
#include <x86/mmu.h>
#include <sys/cpu.h>
#include <sys/kernslice.h>
#include <sys/kspinlock.h>
#include <string.h>
#include <libkern/panic.h>

static kspinlock_t map_sl = 0;

extern pte_t _i686_kernel_page_dir[];
extern pte_t _i686_kernel_page_table[];
#define KPTMIN 3072
//#define KPTMIN 4000
#define KPTMAX 4096

static u_intptr_t map_page(paddr_t pa){
	int i;
	u_intptr_t res = 0;
	kernlock_lock(&map_sl);
	for(i=KPTMIN;i<KPTMAX;++i)
		if(_i686_kernel_page_table[i] == 0) break;
	if( (i>=KPTMIN) && (i<KPTMAX) ){
		_i686_kernel_page_table[i] = PTE_ADDR(pa) | PTE_PW;
		res = 0xC0000000 + (u_intptr_t)(i<<12);
		invlpg((void*)res);
	}
	kernlock_unlock(&map_sl);
	return res;
}
static void unmap_page(u_intptr_t va){
	u_intptr_t i = (va-0xC0000000)>>12;
	//kernlock_lock(&map_sl);
	_i686_kernel_page_table[i] = 0;
	invlpg((void*)va);
	//kernlock_lock(&map_sl);
}

/*
 * This function zeroes out a Memory Page.
 */
void pmap_zero_page(paddr_t pa){
	u_intptr_t va = map_page(pa);
	if(!va)panic("Cannot map Physical page.");
	memset((void*)va,0,4096);
	unmap_page(va);
}

/*
 * This function sets a page table/directory entry in a page table/directory.
 */
void _i686_pmap_pte_set(paddr_t pa,int i,pte_t pte){
	u_intptr_t va = map_page(pa);
	if(!va)panic("Cannot map Physical page.");
	((pte_t*)va)[i] = pte;
	unmap_page(va);
}

/*
 * This function reads a page table/directory entry from a page table/directory.
 */
pte_t _i686_pmap_pte_get(paddr_t pa,int i){
	pte_t pte;
	u_intptr_t va = map_page(pa);
	if(!va)return 0;
	pte = ((pte_t*)va)[i];
	unmap_page(va);
	return pte;
}

/*
 * This function initializes a new page table.
 */
void _i686_pmap_pdinit(paddr_t pa){
	int i;
	u_intptr_t va = map_page(pa);
	if(!va)panic("Cannot map Physical page.");
	for(i=0;i<768;++i){
		((pte_t*)va)[i] = 0;
	}
	for(i=768;i<1024;++i){
		((pte_t*)va)[i] = _i686_kernel_page_dir[i];
	}
	unmap_page(va);
}
