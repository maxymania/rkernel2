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
#include <vm/vm_page.h>
#include <sys/kernslice.h>
#include <sys/physmem_alloc.h>
#include <kern/zalloc.h>

void vm_page_free(struct kernslice* slice, paddr_t addr){
	vm_phys_free(slice->ks_memory_allocator,addr);
}

void vm_page_drop(vm_page_t page){
	u_int32_t refc;
	kernlock_lock(&(page->pg_lock));
		page->pg_refc--;
		refc = page->pg_refc;
	kernlock_unlock(&(page->pg_lock));
	if(!refc){
		vm_page_free(page->pg_slice,page->pg_phys);
		zfree((void*)page);
	}
}

