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
#include <sysmaster/syscalls.h>
#include <sys/cpu.h>
#include <sys/kernslice.h>
#include <sys/kterm.h>
#include <sysplatform/caps.h>
#include <sysarch/halt.h>
#include <sys/physmem_alloc.h>
#include <vm/pmap.h>
#include <stdio.h>
#include <kern/zalloc.h>
#include <vm/vm_top.h>

void kern_prove_alive();

static void kern_initmem(){
	u_intptr_t             Pi;
	paddr_t                Pt;
	struct kernslice*      kern;
	struct physmem_bmaset* bmas;
	int i;
	kern = kernel_get_current_cpu()->cpu_kernel_slice;
	i = vm_phys_bm_bootinit(
		kern->ks_memory_ranges,
		kern->ks_num_memory_ranges,
		&Pi,
		&Pt,
		&bmas
	);
	kern->ks_memory_allocator = bmas;
	printf("vm_phys_bm_bootinit() = %d\n",i);
	printf("bmas = %p\n",bmas);
	if(bmas){
		printf("bmas->pmb_maps = %p\n",bmas->pmb_maps);
		printf("bmas->pmb_n_maps = %u\n",(unsigned int)bmas->pmb_n_maps);
		for(i=0;i<bmas->pmb_n_maps;++i){
			printf("bmas->pmb_maps[%d] = %d\n",i,(unsigned int)(bmas->pmb_maps[i]->pmb_length));
		}
	}
	//zone_bootstrap();
	//pmap_init();
	vm_init();
}

void kernel_main(void) {
	int caps = platform_get_cap_stage();
	switch(caps){
	case platform_CPU_PTR:
		kterm_init();
		kern_initmem();
		kern_prove_alive();
		break;
	case platform_ALIVE:
	case platform_HIGHER_HALF:
		//console_init();
		kterm_init();
		kern_prove_alive();
		break;
	}
	/* Should not get here. */
	arch_halt();
}

