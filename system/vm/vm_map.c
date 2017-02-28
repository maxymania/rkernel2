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
#include <vm/vm_map.h>
#include <kern/zalloc.h>
#include <string.h>
#include <xcpu/vm.h>

static zone_t vm_map_zone;         /* Zone for vm_map structures. */
static zone_t vm_map_entry_zone;   /* Zone for vm_map_entry objects. (User mode) */
static zone_t vm_map_kentry_zone;  /* Zone for vm_map_entry objects. (Kernel mode) */


/* This is the size of a cache line (in x86). */
#define CACHE_LINE  128

static u_int8_t z_map_buf[1<<12] __attribute__ ((aligned (CACHE_LINE)));

static struct vm_map kernel_map;

void vm_map_init(){
	vm_map_zone        = zinit(sizeof(struct vm_map),ZONE_AUTO_REFILL,"VM address space zone");
	vm_map_entry_zone  = zinit(sizeof(struct vm_map_entry),ZONE_AUTO_REFILL,"user mode segment zone");
	vm_map_kentry_zone = zinit(sizeof(struct vm_map_entry),0,"kernel mode segment zone");
	
	zcram(vm_map_kentry_zone,(void*)z_map_buf,sizeof(z_map_buf));
	
	list_init(&(kernel_map.list));
	list_init(&(kernel_map.parents));
	/* kernel_map.min_offset */
	/* kernel_map.max_offset */
	kernel_map.pmap = pmap_kernel();
	/* kernel_map.size */
	kernel_map.ref_count = ~0;
	kernlock_init(&(kernel_map.lock));
	kernlock_init(&(kernel_map.ref_lock));
	kernlock_init(&(kernel_map.pmap_lock));
	
	pmap_get_address_range(kernel_map.pmap, &(kernel_map.min_offset),&(kernel_map.max_offset));
	kernel_map.size = (kernel_map.max_offset-kernel_map.min_offset)+1;
}

vm_map_t vm_map_get_kernel(){ return &kernel_map; }

static int vm_map_find_free(
	vm_map_t map,
	vm_map_entry_t dme,
	vaddr_t lrp /* Last relative pointer (size -1) */,
	list_node_t *next_node
){
	vaddr_t begin,end;
	vm_map_entry_t entry;
	list_node_t node;
	begin = map->min_offset;
	end = begin+lrp;
	
	node = &(map->list);
	
	while(end<=map->max_offset){
		node = node->next;
		if(node != &(map->list)){
			entry = containerof(node,struct vm_map_entry,links);
			if( end >= entry->start ){
				begin = entry->end+1;
				end = begin+lrp;
				continue;
			}
		}
		dme->start = begin;
		dme->end   = end;
		*next_node = node;
		return 1;
	}
	return 0;
}

/*
 * Insert an Entry into the Map.
 */
int vm_map_insert_entry(
	vm_map_t map,
	vm_map_entry_t entry,
	u_intptr_t size
){
	list_node_t next_node;
	int retvar = 0;
	
	kernlock_lock(&(map->lock));
	
	if(vm_map_find_free(map,entry,size-1,&next_node)){
		list_add_before(next_node,&(entry->links));
		entry->parent = map;
		retvar = 1;
	}
	
	kernlock_unlock(&(map->lock));
	
	return retvar;
}

/*
 * Remove an Entry from the Map.
 */
int vm_map_remove_entry(vm_map_t map, vm_map_entry_t entry){
	vaddr_t begin,end;
	begin = entry->start;
	end   = entry->end;
	
	if(entry->parent != map) return 0;
	
	kernlock_lock(&(map->lock));
	
	list_item_remove(&(entry->links));
	list_clear(&(entry->links));
	
	kernlock_unlock(&(map->lock));
	
	entry->parent = 0;
	
	xcpu_cache_flush_range(map->pmap,begin,end);
	
	kernlock_lock(&(map->pmap_lock));
	
	pmap_remove(map->pmap,begin,end);
	
	kernlock_unlock(&(map->pmap_lock));
	
	xcpu_tlb_flush_range(map->pmap,begin,end);
	
	return 1;
}

static void vm_map_entry_construct(vm_map_entry_t entry){
	list_clear(&(entry->links));
	entry->start            = 0;
	entry->end              = 0;
	list_clear(&(entry->child));
	entry->object.vm_object = 0;
	entry->offset           = 0;
	entry->parent           = 0;
	
	entry->is_shared        = 0;
	entry->is_sub_map       = 0;
	entry->in_transition    = 0;
	entry->needs_wakeup     = 0;
	entry->needs_copy       = 0;
	entry->protection       = 0;
	entry->max_protection   = 0;
	
}

vm_map_entry_t vm_map_kentry_alloc(){
	vm_map_entry_t entry;
	entry = zalloc(vm_map_kentry_zone);
	if(entry)
		vm_map_entry_construct(entry);
	return entry;
}

vm_map_entry_t vm_map_entry_alloc(){
	vm_map_entry_t entry;
	entry = zalloc(vm_map_entry_zone);
	if(entry)
		vm_map_entry_construct(entry);
	return entry;
}

