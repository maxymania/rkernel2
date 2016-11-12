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
struct kernslice;

/* Architecture specific part of 'struct cpu'. */
struct cpu_arch;

struct cpu{
	u_intptr_t        cpu_cpu_id;         /* The ID of this CPU. */
	struct kernslice* cpu_kernel_slice;   /* The kernel slice, this CPU belongs to. */
	struct cpu*       cpu_ks_next;        /* Next CPU within this kernel slice. */
	
	struct thread*    cpu_current_thread; /* The thread currently running on this CPU. */
	u_intptr_t        cpu_stack;          /* Stack pointer of the Per-CPU stack. */
	u_intptr_t        cpu_local[3];       /* CPU global segment. */
	struct cpu_arch  *cpu_arch;           /* Architecture specific part */
};

#define CPU_LOCAL_SELF   cpu_local[0]   /* struct cpu-instance. */
#define CPU_LOCAL_TLS    cpu_local[1]   /* The current thread's TLS. */
#define CPU_LOCAL_STACK  cpu_local[2]   /* CPU local stack. */


struct cpu* kernel_get_current_cpu();

