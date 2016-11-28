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

int vm_range_bmlkup(vm_range_t range, int i){
	int s = i % 32;
	i /= 32;
	return (range->rang_pages_tbm[i]>>s) & 1;
}

int vm_range_get   (vm_range_t range, vaddr_t rva, paddr_t *pag, vm_prot_t *prot){
	vm_page_t pobj;
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

