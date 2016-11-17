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
#include <machine/types.h>
#include <sys/kernslice.h>
#include <sys/cpu.h>
#include <sysarch/hal.h>
#include <x86/cpu_arch.h>

extern const u_int32_t  _i686_multiboot_memdata[5];

/*
 * A Pointer to the end of the Kernel.
 */
extern const char _kernel_end[];

static struct kernslice slice;
static struct cpu cpu;
static struct physmem_range memrange[2];
static struct cpu_arch cpu_arch;

void kernel_main(void);

static void _i686_init(){
	u_int32_t endkernel = (u_int32_t)(const void*)_kernel_end;
	endkernel-=0xC0000000;
	u_int32_t flags = _i686_multiboot_memdata[0];
	cpu.cpu_cpu_id = 0;
	cpu.cpu_kernel_slice = &slice;
	cpu.cpu_ks_next = 0;
	cpu.cpu_current_thread = 0;
	cpu.cpu_arch = &cpu_arch;
	
	/*
	 * Assign the pointer to the CPU structure to the field in the CPU-private segment.
	 */
	cpu.CPU_LOCAL_SELF  = (u_intptr_t)&cpu;
	
	slice.ks_kernslice_id  = 0;
	slice.ks_memory_ranges = memrange;
	if(flags&1){
		memrange[0].pm_begin = 0;
		memrange[0].pm_end = _i686_multiboot_memdata[1]<<10;
		if(endkernel>0x100000) memrange[1].pm_begin = endkernel;
		else memrange[1].pm_begin = 0x100000;
		memrange[1].pm_end = (_i686_multiboot_memdata[2]<<10)+0x100000;
		slice.ks_num_memory_ranges = 2;
	}
	
	hal_initcpu(&cpu);
}


void _i686_boot_main(void) {
	/*
	 * We put it into a seperate function to prevent it's variables to reside
	 * on the Stack.
	 */
	_i686_init();
	
	kernel_main();
}

