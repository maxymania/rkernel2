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
#include <sys/cpu.h>
#include <sys/kernslice.h>
#include <sys/kterm.h>
#include <sysplatform/caps.h>
#include <sysarch/halt.h>
#include <sys/physmem_alloc.h>
#include <sys/thread.h>
#include <stdio.h>
#include <kern/stacks.h>
#include <vm/vm_top.h>

#include <libkern/panic.h>

void kern_prove_alive();

static void kern_initmem(){
	u_intptr_t             Pi;
	paddr_t                Pt;
	struct kernslice*      kern;
	struct physmem_bmaset* bmas;
	kern = kernel_get_current_cpu()->cpu_kernel_slice;
	vm_phys_bm_bootinit(
		kern->ks_memory_ranges,
		kern->ks_num_memory_ranges,
		&Pi,
		&Pt,
		&bmas
	);
	kern->ks_memory_allocator = bmas;
}

void kern_printmem();

static void main();

void kernel_main(void) {
	int caps = platform_get_cap_stage();
	switch(caps){
	case platform_INTERRUPTS:
		main();
		break;
	case platform_MMU:
		kernel_stacks_init();
		kterm_init();
		kern_initmem();
		kern_printmem();
		vm_init();
		kernel_cpu_init_stack(kernel_get_current_cpu());
		kern_prove_alive();
		break;
	case platform_CPU_PTR:
		kernel_stacks_init();
		kterm_init();
		kern_initmem();
		kern_printmem();
		kern_prove_alive();
		break;
	case platform_ALIVE:
	case platform_HIGHER_HALF:
		kterm_init();
		kern_prove_alive();
		break;
	}
	/* Should not get here. */
	arch_halt();
}

static void main(){
	struct thread* thread;
	
	/* Initialize the kernel-stack allocator. */
	kernel_stacks_init();
	
	/* Initilalize the terminal. */
	kterm_init();
	
	/* Initialize the bitmap physical memory allocator.  */
	kern_initmem();
	
	/* Initialize the VM system. */
	vm_init();
	
	/* Allocate the 'cpu->CPU_LOCAL_STACK' stack. */
	kernel_cpu_init_stack(kernel_get_current_cpu());
	
	/* Initialize the Thread-System. */
	thread_init();
	
	/* Allocate and set the first thread. */
	thread = thread_allocate();
	if(!thread) panic("Couldn't allocate the Idle thread!");
	kernel_set_current_thread(thread);
	
	printf("Hey, we need to do more!\n");
	/* TODO: do more initilalization. */
	
	/* Idle-process. */
	arch_halt();
}
