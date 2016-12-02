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
#include <vm/vm_as.h>
#include <vm/vm_seg.h>
#include <vm/vm_priv.h>
#include <vm/pmap.h>
#include <kern/zalloc.h>

#include <stdio.h>

static zone_t vm_as_zone; /* Zone for vm_as structures. */

static struct vm_as kernel_as;

void vm_as_init(){
	vm_as_zone = zinit(sizeof(struct vm_as),ZONE_AUTO_REFILL,"VM address space zone");
	kernel_as.as_segs = 0;
	kernel_as.as_pmap = pmap_kernel();
	kernlock_init(&(kernel_as.as_lock_pmap));
	kernlock_init(&(kernel_as.as_lock_segs));
	kernlock_init(&(kernel_as.as_lock));
	pmap_get_address_range(kernel_as.as_pmap, &(kernel_as.as_begin),&(kernel_as.as_end));
}

void vm_as_mcram(){
	zrefill(vm_as_zone,64,64);
}

static int vm_find_free(vm_as_t as, vm_seg_t dseg, vaddr_t lrp /* Last relative pointer (size -1) */){
	vm_bintree_t * __restrict__ bt;
	vm_seg_t seg;
	vaddr_t begin = as->as_begin;
	vaddr_t end = begin+lrp;
	
	while(end<=as->as_end){
		bt = bt_ceiling(&(as->as_segs),begin);
		if(bt && *bt){
			seg = (vm_seg_t)((*bt)->V);
			if(end>=seg->seg_begin){
				begin = seg->seg_end+1;
				end = begin+lrp;
				continue;
			}
		}
		dseg->seg_begin = begin;
		dseg->seg_end   = end;
		return 1;
	}
	return 0;
}

vm_as_t vm_as_get_kernel(){ return &kernel_as; }

int vm_insert_entry(vm_as_t as, vaddr_t size, struct vm_seg * seg) {
	vm_bintree_t entry;
	
	kernlock_lock(&(as->as_lock_segs));
	if(!vm_find_free(as,seg,size-1)) {
		kernlock_unlock(&(as->as_lock_segs));
		return 0;
	}
	
	vm_seg_initobj(seg);
	entry = &(seg->_bt_node);
	bt_insert(&(as->as_segs),&entry);
	
	kernlock_unlock(&(as->as_lock_segs));
	
	if(entry) { /* Insert failed. */
		return 0;
	}
	return 1;
}

int vm_remove_entry(vm_as_t as, struct vm_seg * seg) {
	vm_bintree_t* entry = 0;
	vm_bintree_t res;
	vaddr_t begin = seg->seg_begin;
	vaddr_t end = seg->seg_end;
	
	kernlock_lock(&(as->as_lock_segs));
	entry = bt_lookup(&(as->as_segs),begin);
	
	if(entry && *entry){
		if( (*entry)->V != seg ) entry = 0;
		else bt_remove(entry,&res);
	}
	
	kernlock_unlock(&(as->as_lock_segs));
	
	if(entry && *entry) { /* Remove failed. */
		return 0;
	}
	
	kernlock_lock(&(as->as_lock_pmap));
	
	pmap_remove(as->as_pmap,begin,end);
	
	kernlock_unlock(&(as->as_lock_pmap));
	
	return 1;
}

