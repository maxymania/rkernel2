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
#include <kern/sched.h>
#include <kern/wait_queue.h>
#include <sys/thread.h>

/*
 * The function waitqueue_elem(thread) retrieves a pointer to the t_wait_entry-field
 * and at the same time, it sets    thread->t_wait_entry.data := thread  .
 *
 * The good aspect of this function is, that the linked_ring_t-instance,
 * it returns, is guaranteed to be perfectly initialized.
 */
static inline linked_ring_t waitqueue_elem(struct thread* thread){
	linked_ring_t ring = &(thread->t_wait_entry);
	ring->data = thread;
	return ring;
}


void waitqueue_enter(struct wait_queue* queue,struct thread* thread,int after){
	linked_ring_insert(&(queue->wq_threads), waitqueue_elem(thread), after);
}

int waitqueue_get_first(struct wait_queue* queue){
	linked_ring_t head,elem;
	struct thread* thread;
	head = &(queue->wq_threads);
	elem = head->prev;
	
	if(elem==head) return 0;
	
	thread = elem->data;
	linked_ring_remove(elem);
	thread->t_wait_queue = 0;
	
	sched_actualize(thread);
	
	return 1;
}

