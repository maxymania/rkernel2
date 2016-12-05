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
#include <machine/stdtypes.h>
#include <sysarch/hal.h>
#include <sys/thread.h>
#include <sys/cpu.h>
#include <kern/stacks.h>
#include <sys/kspinlock.h>
#include <vm/vm_top.h>
#include <libkern/panic.h>

static struct kernel_stack * stacks;
static kspinlock_t stacks_sl;

static size_t calc_size(size_t size) {
	size_t num = 1;
	while(num<size){
		num<<=1;
	}
	return num;
}


void kernel_stacks_init(){
	stacks = 0;
	kernlock_init(&(stacks_sl));
}

void kernel_cpu_init_stack(struct cpu* cpu){
	vaddr_t begin,size;
	size = 1<<14; /* 16K */
	if(!vm_kalloc_ll(&begin,&size))
		panic("Failed to allocate cpu stack for CPU %p\n",cpu->cpu_cpu_id);
	if( hal_stack_grows_downward() ){
		cpu->CPU_LOCAL_STACK = (begin+size);
	}else{
		cpu->CPU_LOCAL_STACK = begin;
	}
}

static struct kernel_stack* kernel_stack_new(){
	vaddr_t begin,size;
	struct kernel_stack* kstack = 0;
	size_t base = calc_size( sizeof(struct kernel_stack) );
	if(!vm_kalloc_ll(&begin,&size)) return 0;
	
	size -= base;
	
	if( hal_stack_grows_downward() ){
		kstack = (struct kernel_stack*)(begin+size);
		kstack->st_sp = (begin+size);
	}else{
		kstack = (struct kernel_stack*)begin;
		kstack->st_sp = begin+base;
	}
}

struct kernel_stack* kernel_stack_allocate(){
	struct kernel_stack* kstack;
	kernlock_lock(&(stacks_sl));
	kstack = stacks;
	if(kstack){ stacks = kstack->st_tail; }
	kernlock_unlock(&(stacks_sl));
	if(!kstack) kstack = kernel_stack_new();
	return kstack;
}

void kernel_stack_release(struct kernel_stack* kstack){
	if(!kstack) return;
	kernlock_lock(&(stacks_sl));
	kstack->st_tail = stacks;
	stacks = kstack;
	kernlock_unlock(&(stacks_sl));
}

