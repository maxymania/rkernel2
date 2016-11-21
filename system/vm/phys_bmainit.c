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
#define MUL_32(x)  ((x) << 5)

#define UNUMBER 0x1000
#define NNUMBER 0x10

#ifdef SYSARCH_PAGESIZE_SHIFT
#define MUL_PAGESIZE(x)  ((x)<<SYSARCH_PAGESIZE_SHIFT)
#define DIV_PAGESIZE(x)  ((x)>>SYSARCH_PAGESIZE_SHIFT)
#else
#define MUL_PAGESIZE(x)  ((x)*SYSARCH_PAGESIZE)
#define DIV_PAGESIZE(x)  ((x)/SYSARCH_PAGESIZE)
#endif

static struct physmem_bmaset   prealloc_bmas;
static struct physmem_bmalloc  prealloc_pbma[NNUMBER];
static struct physmem_bmalloc* prealloc_pbma_ptr[NNUMBER];

static u_int32_t bitmap_block[UNUMBER];


int vm_phys_bm_bootinit(
		struct physmem_range *rng,
		u_intptr_t n_ranges,
		u_intptr_t *Pi,
		paddr_t *Pt,
		struct physmem_bmaset** Pbma
){
	u_int32_t *bitmap_current = bitmap_block;
	u_int32_t *bitmap_last    = bitmap_current+UNUMBER;
	struct physmem_range * __restrict__ range = rng;
	u_intptr_t i;
	paddr_t  j,n,m,k;
	
	prealloc_bmas.pmb_maps   = prealloc_pbma_ptr;
	prealloc_bmas.pmb_n_maps = 0;
	kernlock_init(&(prealloc_bmas.pmb_lock));
	*Pbma = &prealloc_bmas;
	for(i=0;i<n_ranges;++i){
		if(i >= NNUMBER) {
			*Pi = i;
			*Pt = range[i].pm_begin;
			return -1;
		}
		prealloc_bmas.pmb_n_maps++;
		prealloc_pbma_ptr[i] = &prealloc_pbma[i];
		
		m = DIV_PAGESIZE(range[i].pm_end-range[i].pm_begin);
		n = DIV_32( m + 31 );// + (SYSARCH_PAGESIZE-1)
		for(j=0;j<n;++j){
			if((&bitmap_current[j]) >= bitmap_last) break;
			bitmap_current[j] = 0;
		}
		k = MUL_32(j);
		if(k>m) k=m;
		prealloc_pbma[i].pmb_range  = range[i];
		prealloc_pbma[i].pmb_bitmap = bitmap_current;
		prealloc_pbma[i].pmb_length = k;
		bitmap_current = &bitmap_current[j];
		
		if(bitmap_current >= bitmap_last){
			*Pi = i;
			*Pt = range[i].pm_begin+MUL_PAGESIZE( MUL_32(j) );
			return -1;
		}
	}
	return 0;
}

