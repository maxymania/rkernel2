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
#include <kern/lock.h>
#include <kern/wait.h>
#include <sys/errno.h>

#define WAKEUP_ENOLCK do{\
	waitqueue_get_first(&(lock->sl_queue)); \
	return ENOLCK; \
}while(0)

static inline int sl_lock_internal(struct shared_lock* lock,int type){
	switch(type){
	case SL_TYPE_SHARED:
		/* If The lock is draining, terminate the algorithm. */
		if((lock->sl_flags) & SL_FLAG_DRAIN) return ENOLCK;
		
		/*
		 * Wait, until the exclusive lock had been drained, if any.
		 */
		while(lock->sl_number<0){
			waitqueue_wait(&(lock->sl_lock),&(lock->sl_queue),0);
			
			/* If The lock is draining, terminate the algorithm. */
			if((lock->sl_flags) & SL_FLAG_DRAIN) WAKEUP_ENOLCK;
		}
		lock->sl_number++;
		
		/*
		 * Wake up the next waiting thread.
		 */
		waitqueue_get_first(&(lock->sl_queue));
		break;
	case SL_TYPE_EXCLUSIVE:
		/* If The lock is draining, terminate the algorithm. */
		if((lock->sl_flags) & SL_FLAG_DRAIN) return ENOLCK;
		
		/*
		 * Wait, until all locks had been drained.
		 */
		while(lock->sl_number!=0){
			waitqueue_wait(&(lock->sl_lock),&(lock->sl_queue),0);
			
			/* If The lock is draining, terminate the algorithm. */
			if((lock->sl_flags) & SL_FLAG_DRAIN) WAKEUP_ENOLCK;
		}
		lock->sl_number = -1;
		break;
	}
	return 0;
}
#undef WAKEUP_ENOLCK


/*
 * Acquires a lock. A fair algorithm is used to acquire it.
 */
int sl_lock(struct shared_lock* lock,int type){
	int result;
	kernlock_lock(&(lock->sl_lock));
	
	/*
	 * If The lock is draining, terminate the algorithm.
	 */
	if((lock->sl_flags) & SL_FLAG_DRAIN){
		kernlock_unlock(&(lock->sl_lock));
		return ENOLCK;
	}
	
	/*
	 * If the queue is not empty, enqueue yourself for fairness.
	 */
	if(!linked_ring_empty(&(lock->sl_queue.wq_threads)))
		waitqueue_wait(&(lock->sl_lock),&(lock->sl_queue),1);
	
	result = sl_lock_internal(lock,type);
	
	kernlock_unlock(&(lock->sl_lock));
	return result;
}

/*
 * Acquires a lock. An unfair algorithm is used to acquire it.
 */
int sl_lock_greedy(struct shared_lock* lock,int type){
	int result;
	kernlock_lock(&(lock->sl_lock));
	
	result = sl_lock_internal(lock,type);
	
	kernlock_unlock(&(lock->sl_lock));
	return result;
}

/*
 * Releases a lock.
 */
void sl_unlock(struct shared_lock* lock,int type){
	kernlock_lock(&(lock->sl_lock));
	switch(type){
	case SL_TYPE_SHARED:
		lock->sl_number--;
		break;
	case SL_TYPE_EXCLUSIVE:
		lock->sl_number = 0;
		break;
	}
	waitqueue_get_first(&(lock->sl_queue));
	kernlock_unlock(&(lock->sl_lock));
}

/*
 * Drains a lock object.
 */
void sl_drain(struct shared_lock* lock){
	kernlock_lock(&(lock->sl_lock));
	
	/*
	 * Indicate, that this queue shall be drained.
	 */
	lock->sl_flags |= SL_FLAG_DRAIN;
	
	/*
	 * Wakeup the next waiting thread.
	 */
	waitqueue_get_first(&(lock->sl_queue));
	
	/*
	 * Wait until, all shared or exclusive locks have been drained.
	 */
	while(lock->sl_number!=0){
		waitqueue_wait(&(lock->sl_lock),&(lock->sl_queue), /*after=*/1);
	}
	
	/*
	 * Wait until, all waiting threads have been drained.
	 */
	while(!linked_ring_empty(&(lock->sl_queue.wq_threads))){
		waitqueue_wait(&(lock->sl_lock),&(lock->sl_queue), /*after=*/1);
	}
	kernlock_unlock(&(lock->sl_lock));
}

/*
 * This algorithm is equal to:
 * {
 *  sl_lock[_greedy](type);
 *  sl_unlock(type);
 * }
 */
int sl_touch(struct shared_lock* lock,int type){
	kernlock_lock(&(lock->sl_lock));
	switch(type){
	case SL_TYPE_SHARED:
		/*
		 * Wait, until the exclusive lock had been drained, if any.
		 */
		while(lock->sl_number<0){
			waitqueue_wait(&(lock->sl_lock),&(lock->sl_queue),0);
		}
		break;
	case SL_TYPE_EXCLUSIVE:
		/*
		 * Wait, until all locks had been drained.
		 */
		while(lock->sl_number!=0){
			waitqueue_wait(&(lock->sl_lock),&(lock->sl_queue),0);
		}
		break;
	}
	waitqueue_get_first(&(lock->sl_queue));
	kernlock_unlock(&(lock->sl_lock));
	return 0;
}


