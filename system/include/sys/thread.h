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
#pragma once

#include <machine/types.h>

struct cpu;
struct kernel_stack;

struct thread{
	/* Scheduler */
	struct thread* t_next_queue;  /* Next thread in queue. */
	struct thread* t_prev_queue;  /* Previous thread in queue. */
	struct cpu*    t_current_cpu; /* The CPU this thread is currently running on. */
	
	/* Context */
	u_intptr_t     t_storage[4];  /* The thread's TLS (ASM). */
	u_intptr_t     t_istacks[2];  /* Interrupt stacks. Default is t_istacks[0] */
	struct kernel_stack*
	               t_istobjs[2];  /* The corresponding Stack Objects to 't_istacks' */
	u_intptr_t     t_stateflags;  /* Flags, indicating the Thread's state. */
};

#define THREAD_LOCAL_INT_STACK    t_storage[0] /* (current)Interupt stack. */
#define THREAD_LOCAL_CONTEXT      t_storage[1] /* Pointer to saved context. */

#define THREAD_SF_INTSTACK_2      0x0001   /* If set, interrupt stack is t_istacks[1] */
#define THREAD_SF_PREEMPT         0x0002   /* If set, thread is preempted. */

void thread_init();

struct thread* thread_allocate();

struct thread* kernel_get_current_thread();

void kernel_set_current_thread(struct thread* thread);

void thread_enter_syscall();

void thread_exit_syscall();

