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
#include <vm/vm_top.h>
#include <vm/vm_as.h>
#include <vm/vm_seg.h>
#include <vm/vm_mem.h>
#include <vm/vm_range.h>
#include <kern/zalloc.h>

#include <sys/cpu.h>
#include <sys/kernslice.h>
#include <vm/pmap.h>
#include <sys/physmem_alloc.h>

#include <stdio.h>

static void testmapping(){
	vaddr_t va = 0xC2000000;
	paddr_t pa;
	struct cpu *cpu = kernel_get_current_cpu();
	int ok = vm_phys_alloc(cpu->cpu_kernel_slice->ks_memory_allocator, &pa);
	printf("vm_phys_alloc = %d , %p\n",ok,(void*)pa);
	if(ok){
		ok = pmap_enter(pmap_kernel(),va,pa,VM_PROT_READ|VM_PROT_WRITE,0);
		printf("pmap_enter = %d\n",ok);
		if(ok){
			char* chr = (char*)va;
			chr[0] = '1';
			chr[1] = '2';
			chr[2] = '3';
			chr[3] = 0;
			printf("chr = %s\n",chr);
		}
	}
}


/*
 * This function initializes the kernel virtual memory system.
 */
void vm_init(){
	zone_bootstrap();
	pmap_init     ();
	vm_as_init    ();
	vm_seg_init   ();
	vm_mem_init   ();
	vm_range_init ();
	
	testmapping();
	
	//vm_seg_refill  ();
	//vm_mem_refill  ();
	//vm_range_refill();
	//vm_as_refill   ();
}

void vm_refill(){
	vm_seg_refill  ();
	vm_mem_refill  ();
	vm_range_refill();
}

