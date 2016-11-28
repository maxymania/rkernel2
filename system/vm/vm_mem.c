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

