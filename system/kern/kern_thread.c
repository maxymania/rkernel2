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
#include <sysarch/hal.h>
#include <sys/thread.h>
#include <sys/cpu.h>
#include <kern/zalloc.h>
#include <kern/stacks.h>

#define loop(i,n) for(i=0;i<n;++i)

static zone_t thread_zone; /* Thread zone */

struct thread thread_template;

void thread_init(){
	thread_zone = zinit(sizeof(struct thread),ZONE_AUTO_REFILL,"threads");
	
	thread_template.t_next_queue  = 0;
	thread_template.t_prev_queue  = 0;
	thread_template.t_current_cpu = 0;
	/* thread_template.t_storage[] (later) */
	/* thread_template.t_istacks[] (later) */
	/* thread_template.t_istobjs[] (later) */
	thread_template.t_stateflags  = 0;
}

struct thread* thread_allocate(){
	int i;
	struct thread* thr = zalloc(thread_zone);
	if(thr==0) return 0;
	*thr = thread_template;
	
	/* thr->t_istobjs[] */
	loop(i,2) thr->t_istobjs[i] = kernel_stack_allocate();
	loop(i,2) if(!(thr->t_istobjs[i])) goto failure;
	
	/* thr->t_istacks[] */
	loop(i,2) thr->t_istacks[i] = thr->t_istobjs[i]->st_sp;
	
	/* thr->t_storage[] */
	thr->THREAD_LOCAL_INT_STACK = thr->t_istacks[0];
	
	return thr;
failure:
	loop(i,2) if(thr->t_istobjs[i]) kernel_stack_release(thr->t_istobjs[i]);
	zfree(thr);
	return 0;
}

struct thread* kernel_get_current_thread(){
	return kernel_get_current_cpu()->cpu_current_thread;
}

void kernel_set_current_thread(struct thread* thread){
	struct cpu* cpu = kernel_get_current_cpu();
	
	cpu->cpu_current_thread = thread;
	cpu->CPU_LOCAL_TLS = (u_intptr_t)thread->t_storage;
	thread->t_current_cpu = cpu;
	hal_after_thread_switch();
}

void thread_update_int_stack(struct thread* thread){
	u_intptr_t sp = ((thread->t_stateflags)&THREAD_SF_INTSTACK_2)
			?thread->t_istacks[1]:thread->t_istacks[0];
	thread->THREAD_LOCAL_INT_STACK = sp;
}

void thread_enter_syscall(){
	struct thread* thread = kernel_get_current_cpu()->cpu_current_thread;
	thread->t_stateflags |= THREAD_SF_INTSTACK_2;
	thread->THREAD_LOCAL_INT_STACK = thread->t_istacks[1];
	hal_after_thread_switch();
}

void thread_exit_syscall(){
	struct thread* thread = kernel_get_current_cpu()->cpu_current_thread;
	thread->t_stateflags &= ~THREAD_SF_INTSTACK_2;
	thread->THREAD_LOCAL_INT_STACK = thread->t_istacks[0];
	hal_after_thread_switch();
}

