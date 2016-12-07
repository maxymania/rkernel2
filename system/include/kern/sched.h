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
#include <sys/kspinlock.h>
#include <kern/ring.h>

#define SCHED_NRQS 32

struct thread;
struct cpu;

struct scheduler{
	linked_ring_s       sched_run_ring[SCHED_NRQS];   /* one queue for each priority */
	signed int          sched_run_decay[SCHED_NRQS];  /* one decay value for each priority */
	struct thread*      sched_idle;                   /* idle thread */
	
	u_intptr_t          sched_thread_count;           /* Number of threads on this core. */
	
	kspinlock_t         sched_lock;                   /* lock for all the fields */
};

void sched_init();

void sched_instanciate(struct cpu* cpu);

/*
 * Inserts a new thread into the scheduler of a given CPU.
 */
void sched_insert(struct cpu* cpu, struct thread* thread);

/*
 * Remove a thread out of the scheduler of a given CPU.
 */
struct thread* sched_remove(struct cpu* cpu);

/*
 * Performs a Thread-preemption. This function must only be called from within
 * a 'preemption-event'. Usually, this is performed from within an interrupt request
 * through the system timer, however it can also be induced.
 * Interrupts are usually turned off, during such an event (on most platforms).
 */
void sched_preempt();

