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
#include <sys/physmem_alloc.h>
#include <sysarch/pages.h>

#define DIV_32(x)  ((x) >> 5)
#define MOD_32(x)  ((x) & 31)
#define BIT_32(x)  (1 << MOD_32(x))

#ifdef SYSARCH_PAGESIZE_SHIFT
#define MUL_PAGESIZE(x)  ((x)<<SYSARCH_PAGESIZE_SHIFT)
#define DIV_PAGESIZE(x)  ((x)>>SYSARCH_PAGESIZE_SHIFT)
#else
#define MUL_PAGESIZE(x)  ((x)*SYSARCH_PAGESIZE)
#define DIV_PAGESIZE(x)  ((x)/SYSARCH_PAGESIZE)
#endif

static int bitmap_search(struct physmem_bmalloc* pmbm,paddr_t *res){
	u_int32_t i;
	u_int32_t* __restrict__ bitmap = pmbm->pmb_bitmap;
	u_int32_t n                    = pmbm->pmb_length;
	for(i=0;i<n;++i){
		if(bitmap[DIV_32(i)] & BIT_32(i)) continue; /* If Used, continue. */
		/*
		 * We found a free page. Now, we need to multiply the index with the Page-size,
		 * and add the offset to the begin of the physical memory region, this bitmap
		 * belongs to.
		 */
		bitmap[DIV_32(i)] |= BIT_32(i);
		*res = MUL_PAGESIZE(i) + pmbm->pmb_range.pm_begin;
		return -1;
	}
	/* No memory found. */
	return 0;
}

int vm_phys_alloc(struct physmem_bmaset* pmas,paddr_t *res) {
	u_int32_t i,n;
	int status = 0;
	kernlock_lock(&(pmas->pmb_lock));
	for(i=0,n=pmas->pmb_n_maps;i<n;++i){
		status = bitmap_search(pmas->pmb_maps[i],res);
		if(status) break;
	}
	kernlock_unlock(&(pmas->pmb_lock));
	return status;
}

int vm_phys_free(struct physmem_bmaset* pmas,paddr_t page) {
	struct physmem_bmalloc* pmbm;
	u_int32_t i,j,n;
	int status = 0;
	kernlock_lock(&(pmas->pmb_lock));
	for(i=0,n=pmas->pmb_n_maps;i<n;++i){
		pmbm = pmas->pmb_maps[i];
		if(
			(pmbm->pmb_range.pm_begin < page)||
			(pmbm->pmb_range.pm_end >= page)
		) continue;
		status = -1;
		j = (u_int32_t)DIV_PAGESIZE(page-(pmbm->pmb_range.pm_begin));
		pmbm->pmb_bitmap[DIV_32(j)] &= ~BIT_32(j);
		break;
	}
	kernlock_unlock(&(pmas->pmb_lock));
	return status;
}

