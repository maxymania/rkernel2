/*
 * 
 * Copyright (c) 2017 Simon Schmidt
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
#include <vm/protection_domain.h>

static struct protection_domain kernel_pd;

void pd_init(){
	vaddr_t start,end;
	kernlock_init(&kernel_pd.pd_lock);
	kernel_pd.pd_pmap = pmap_kernel();
	pmap_get_address_range(kernel_pd.pd_pmap,&start,&end);
	kernel_pd.pd_begin = start;
	kernel_pd.pd_end   = end;
}

pd_t pd_get_kernel(){
	return &kernel_pd;
}
int pd_enter(pd_t dom, vaddr_t va, paddr_t pa, vm_prot_t prot, vm_flags_t flags){
	int ret;
	kernlock_lock(&(dom->pd_lock));
	ret = pmap_enter(dom->pd_pmap, va, pa, prot, flags);
	kernlock_unlock(&(dom->pd_lock));
	return ret;
}
int pd_remove(pd_t dom, vaddr_t vab, vaddr_t vae){
	int ret;
	kernlock_lock(&(dom->pd_lock));
	ret = pmap_remove(dom->pd_pmap, vab, vae);
	kernlock_unlock(&(dom->pd_lock));
	return ret;
}


