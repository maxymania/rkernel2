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
#include <machine/types.h>
#include <sys/kernslice.h>
#include <sys/cpu.h>
#include <sys/kterm.h>
#include <stdio.h>

extern const u_int32_t  _i686_multiboot_memdata[5];
extern const char _kernel_end[];

struct cpu _i686_boot_cpu;

static struct kernslice slice;
static struct cpu cpu;
static struct physmem_range memrange[2];

void kernel_main(void);

/*
 * Currently, we only have one CPU.
 */

struct cpu *kernel_get_current_cpu() {
	return &cpu;
}

static void _i686_init(){
	u_int32_t endkernel = (u_int32_t)(const void*)_kernel_end;
	endkernel-=0xC0000000;
	u_int32_t flags = _i686_multiboot_memdata[0];
	cpu.cpu_cpu_id = 0;
	cpu.cpu_kernel_slice = &slice;
	cpu.cpu_ks_next = 0;
	cpu.cpu_current_thread = 0;
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
}


void _i686_boot_main(void) {
	_i686_init();
	
	kernel_main();
}

