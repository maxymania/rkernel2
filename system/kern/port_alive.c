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
#include <sysplatform/console.h>
#include <sys/kterm.h>
#include <sys/cpu.h>
#include <sys/kernslice.h>
#include <sys/physmem_alloc.h>
#include <sysarch/halt.h>
#include <stdio.h>
#include <libkern/printinfo.h>


/*
 * When called, we should print something, to proove, that we live.
 */
void kern_prove_alive() {
	struct cpu           *cpu = 0;
	struct kernslice     *kern = 0;
	struct physmem_range *prange = 0;
	u_intptr_t            prangen = 0;
	u_int32_t i;
	
	cpu = kernel_get_current_cpu();
	if(cpu) kern = cpu->cpu_kernel_slice;
	if(kern) prange  = kern->ks_memory_ranges;
	if(kern) prangen = kern->ks_num_memory_ranges;
	
	printf("Test <( %i )>\n",99);
	printf("cpu = %p\n",cpu);
	printf("kern = %p\n",kern);
	printf("prange = %p\n",prange);
	printf("prangen = %u\n",(unsigned int)prangen);
	for(i=0;i<prangen;++i){
		printinfo("\tprange[~u].pm_begin = 0x~H\n",(unsigned int)i,prange[i].pm_begin);
		printinfo("\tprange[~u].pm_end   = 0x~H\n",(unsigned int)i,prange[i].pm_end);
	}
	
	arch_halt();
}

void kern_printmem(){
	int i;
	struct kernslice*      kern;
	struct physmem_bmaset* bmas;
	kern = kernel_get_current_cpu()->cpu_kernel_slice;
	bmas = kern->ks_memory_allocator;
	/* printf("vm_phys_bm_bootinit() = %d\n",i); */
	printf("bmas = %p\n",bmas);
	if(bmas){
		printf("bmas->pmb_maps = %p\n",bmas->pmb_maps);
		printf("bmas->pmb_n_maps = %u\n",(unsigned int)bmas->pmb_n_maps);
		for(i=0;((unsigned int)i)<bmas->pmb_n_maps;++i){
			printf("bmas->pmb_maps[%d] = %d\n",i,(unsigned int)(bmas->pmb_maps[i]->pmb_length));
		}
	}
}

