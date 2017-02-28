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
#include <vm/vm_object.h>
#include <kern/zalloc.h>
#include <string.h>
#include <xcpu/vm.h>
#include <vm/vm_priv.h>

static zone_t vm_object_zone;   /* VM object Zone. */

static zone_t vm_c_object_zone; /* Zone for vm_map_entry objects. */

/* This is the size of a cache line (in x86). */
#define CACHE_LINE  128

static u_int8_t z_obj_buf[1<<12] __attribute__ ((aligned (CACHE_LINE)));

void vm_object_init(){
	vm_object_zone     = zinit(sizeof(struct vm_object),ZONE_AUTO_REFILL,"VM object zone.");
	vm_c_object_zone   = zinit(sizeof(struct vm_object),0,"VM object zone. (Critical)");
	
	zcram(vm_c_object_zone,(void*)z_obj_buf,sizeof(z_obj_buf));
}

static void vm_object_constructor(vm_object_t obj){
	list_init(&(obj->memq));
	list_init(&(obj->parents));
	obj->size = 0;
	
	obj->ref_count           = 0;
	obj->resident_page_count = 0;
	obj->shadow_offset       = 0;
	obj->copy_strategy       = 0; /* XXX TODO */
	obj->absent_count        = 0;
	obj->all_wanted          = 0;
	
	obj->paging_in_progress  = 0;
	obj->pager_created       = 0;
	obj->pager_initialized   = 0;
	obj->pager_ready         = 0;
	obj->can_persist         = 0;
	obj->internal            = 1;
	obj->temporary           = 0;
	obj->alive               = 0;
	obj->lock_in_progress    = 0;
	obj->lock_restart        = 0;
	obj->use_old_pageout     = 0;
	obj->use_shared_copy     = 0;
	
	obj->Lock                = 0;
}

struct vm_object* vm_object_alloc_critical(){
	vm_object_t obj = zalloc(vm_c_object_zone);
	if(obj)
		vm_object_constructor(obj);
	return obj;
}
vm_object_t vm_object_alloc(){
	vm_object_t obj = zalloc(vm_c_object_zone);
	if(obj)
		vm_object_constructor(obj);
	return obj;
}

