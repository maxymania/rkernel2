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

#define THREAD_STACK_INT  0
#define THREAD_STACK_KERN 1
#define THREAD_STACK_USER 2

/*
 * Note that the CPU stack is shared accros threads on a CPU. Used for the thread
 * scheduler and other Thread switching functions.
 */
#define THREAD_STACK_CPU  3


struct cpu;

struct thread{
	struct thread* t_next_queue;  /* Next thread in queue. */
	struct cpu*    t_current_cpu; /* The CPU this thread is currently running on. */
	u_intptr_t     t_stacks[4];   /* The Stack pointers. */
};

struct thread* kernel_get_current_thread();

