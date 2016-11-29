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
#include <xcpu/vm.h>
#include <vm/tlb_cache.h>

/*
 * TLB flush all.
 */
void xcpu_tlb_flush_all(pmap_t pmap){
	(void)pmap;
	mmu_tlb_flush_all();
}

/*
 * Flush a range of page-mappings from the TLB.
 */
void xcpu_tlb_flush_range(pmap_t pmap, vaddr_t begin, vaddr_t end){
	(void)pmap;
	mmu_tlb_flush_range(begin,end);
}

/*
 * Flush a single page-mapping from the TLB.
 */
void xcpu_tlb_flush_page(pmap_t pmap, vaddr_t pos){
	(void)pmap;
	mmu_tlb_flush_page(pos);
}



/*
 * Cache flush all.
 */
void xcpu_cache_flush_all(pmap_t pmap){
	(void)pmap;
}

/*
 * Flush a range of page-mappings from the Cache.
 */
void xcpu_cache_flush_range(pmap_t pmap, vaddr_t begin, vaddr_t end){
	(void)pmap;
	(void)begin;
	(void)end;
}

/*
 * Flush a single page-mapping from the Cache.
 */
void xcpu_cache_flush_page(pmap_t pmap, vaddr_t pos){
	(void)pmap;
	(void)pos;
}

