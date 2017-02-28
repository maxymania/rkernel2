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
#include <vm/vm_top.h>

#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>

#include <vm/vm_priv.h>
#include <sysarch/pages.h>
#include <sys/kernslice.h>
#include <vm/pmap.h>
#include <kern/zalloc.h>

#ifdef SYSARCH_PAGESIZE_SHIFT
#define MUL_PAGESIZE(x)  ((x)<<SYSARCH_PAGESIZE_SHIFT)
#define DIV_PAGESIZE(x)  ((x)>>SYSARCH_PAGESIZE_SHIFT)
#define ROUND_DOWN(x) x &= ~((1<<SYSARCH_PAGESIZE_SHIFT)-1)
#else
#define MUL_PAGESIZE(x)  ((x)*SYSARCH_PAGESIZE)
#define DIV_PAGESIZE(x)  ((x)/SYSARCH_PAGESIZE)
#define ROUND_DOWN(x) x -= (x%SYSARCH_PAGESIZE)
#endif

#define NORMAL   0
#define CRITICAL 1

static int vm_kalloc_generic(vaddr_t *addr /* [out] */,vaddr_t *size /* [in/out]*/, int level){
	vm_map_t map;
	vm_map_entry_t entry;
	vm_object_t obj;
	vm_page_t page;
	
	vaddr_t begin,base,end;
	
	/*
	 * Round-Up the size.
	 */
	vaddr_t lsiz = *size + SYSARCH_PAGESIZE - 1;
	ROUND_DOWN(lsiz);
	
	map = vm_map_get_kernel();
	
	/*
	 * Allocate an object.
	 */
	if(level) obj = vm_object_alloc_critical();
	else      obj = vm_object_alloc();
	
	/*
	 * If the allocation failed, give up.
	 */
	if(!obj) return 0;
	
	/*
	 * Allocate an kentry.
	 */
	entry = vm_map_kentry_alloc();
	
	/*
	 * If the allocation failed, give up.
	 */
	if(!obj) {
		zfree(entry);
		return 0;
	}
	
	entry->object.vm_object = obj;
	lists_push_tail(&(obj->parents),&(entry->child));
	
	/*
	 * Insert the entry into the Map.
	 */
	if(!vm_map_insert_entry(map,entry,lsiz)){
		/*
		 * If the mapping failed, give up.
		 */
		zfree(entry);
		zfree(obj);
		return 0;
	}
	
	/*
	 * It is time to aquire the segment lock, 
	 */
	kernlock_lock(&(obj->Lock));
	
	base = begin = entry->start;
	end = entry->end;
	
	/*
	 * Now, we allocate memory for this segment.
	 */
	while((begin-1)<end){
		if(level) page = vm_page_grab_critical(pmap_kernslice(map->pmap));
		else      page = vm_page_grab(pmap_kernslice(map->pmap));
		if(!page) {
			kernlock_unlock(&(obj->Lock));
			return 0; /* XXX: Potential resource Leakage. */
		}
		
		page->object = obj;
		page->offset = begin-base;
		
		list_push_tail(&(obj->memq),&(page->list));
		
		begin+=SYSARCH_PAGESIZE;
	}
	
	kernlock_unlock(&(obj->Lock));
	
	
	/*
	 * Finally, we map the entire allocated memory to the segment's address range.
	 */
	//if(!vm_seg_eager_map(seg,as,VM_PROT_KMEM)) goto endKalloc2;
	
	/*
	 * Now, the memory had been successfully allocated and mapped. Return address and size.
	 */
	*addr = base;
	*size = (entry->end - entry->start)+1;
	
	return 1;
}

