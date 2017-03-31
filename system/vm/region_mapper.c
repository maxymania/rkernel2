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
#include <vm/region_mapper.h>
#include <sysarch/pages.h>
#include <vm/dataspace.h>
#include <vm/pmap.h>
#include <stdio.h>

#ifdef SYSARCH_PAGESIZE_SHIFT
#define ROUND_DOWN(x) x &= ~((1<<SYSARCH_PAGESIZE_SHIFT)-1)
#else
#define ROUND_DOWN(x) x -= (x%SYSARCH_PAGESIZE)
#endif

static void rm_construct(rm_t rm,vaddr_t size){
	list_init(&(rm->rm_childs));
	rm->rm_size = size;
	rm->rm_refc = 1;
	kernlock_init(&(rm->rm_lock));
}
static void rmo_construct(rmo_t rmo){
	list_clear(&(rmo->rmo_member));
	rmo->rmo_object = 0;
	rmo->rmo_type = RMO_UNDEFINED;
	rmo->rmo_static = 0;
}

static struct region_mapper kernel_rm;

#define N_RMOS 4
static kspinlock_t kernel_lock;
static unsigned int kernel_rmoi;

static struct region_mapper_object kernel_rmos[N_RMOS];

static struct {
	struct dataspace ds;
	paddr_t entries[130];
} kernel_rmodss[N_RMOS];

static rmo_t rm_kalloc(){
	unsigned int i;
	rmo_t rmo;
	dataspace_t ds;
	
	
	kernlock_lock(&kernel_lock);
	
	if(kernel_rmoi>=N_RMOS) return (rmo_t) 0;
	i = kernel_rmoi;
	kernel_rmoi++;
	
	rmo = &(kernel_rmos[i]);
	ds = (dataspace_t) &(kernel_rmodss[i]);
	rmo_construct(rmo);
	ds_construct(ds,sizeof(*kernel_rmodss));
	if(ds->ds_n_pages>128)
		ds->ds_n_pages=128;
	
	for(i = 0;i<ds->ds_n_pages;++i)
		ds->ds_pages[i] = 0;
	ds->ds_size = ds->ds_n_pages * SYSARCH_PAGESIZE;
	
	printf("Capacity: ds->ds_size %d\n",(int)(ds->ds_size));
	
	rmo->rmo_static = 1;
	rmo->rmo_type = RMO_DATASPACE;
	rmo->rmo_object = ds;
	rmo->rmo_size = ds->ds_size;
	
	kernlock_unlock(&kernel_lock);
	
	return rmo;
}



void rm_init(){
	rmo_t rmo;
	vaddr_t start,end;
	pmap_get_address_range(pmap_kernel(),&start,&end);
	
	kernlock_init(&kernel_lock);
	kernel_rmoi = 0;
	
	/* Normalize 'end' */
	ROUND_DOWN(end);
	end -= start;
	end += SYSARCH_PAGESIZE;
	rm_construct(&kernel_rm, end);
	
	rmo = rm_kalloc();
	
	rm_insert(&kernel_rm,rmo);
}

int rm_insert(rm_t rm, rmo_t rmo){
	vaddr_t begin,end;
	rmo_t other;
	list_node_t list,node;
	list = &(rm->rm_childs);
	
	begin = 0;
	list_foreach(list,node){
		end = begin+rmo->rmo_size;
		other = containerof(node,struct region_mapper_object,rmo_member);
		
		/*
		 * Does the new Region Mapping touch the current one?
		 */
		if(end>other->rmo_offset){
			/*
			 * Skip it.
			 */
			begin = other->rmo_offset + other->rmo_size;
			continue;
		}
		rmo->rmo_offset = begin;
		list_add_before(&(other->rmo_member),&(rmo->rmo_member));
		return -1;
	}
	
	end = begin+rmo->rmo_size;
	
	/*
	 * Does the new Region Mapping touch the end of the region mapper?
	 */
	if(end>rm->rm_size) return 0;
	
	rmo->rmo_offset = begin;
	list_push_tail(list,&(rmo->rmo_member));
	return -1;
}

rm_t rm_get_kernel(){
	return &kernel_rm;
}

void* rm_ll_lookup(rm_t rm, struct page_fault* fault, int *type){
	vaddr_t fault_ptr;
	rmo_t object;
	list_node_t list,node;
	
	list = &(rm->rm_childs);
	fault_ptr = fault->ptr;
	
	list_foreach(list,node){
		object = containerof(node,struct region_mapper_object,rmo_member);
		if( fault_ptr < object->rmo_offset ) break;
		if( fault_ptr < (object->rmo_offset + object->rmo_size) ){
			if(object->rmo_type==RMO_UNDEFINED) return 0;
			fault->ptr    -= object->rmo_offset;
			fault->size   -= object->rmo_offset;
			fault->offset += object->rmo_offset;
			if(fault->size > object->rmo_size) fault->size = object->rmo_size;
			*type = object->rmo_type;
			return object->rmo_object;
		}
	}
	return 0;
}

