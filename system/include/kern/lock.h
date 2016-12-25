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
#include <kern/wait_queue.h>
#include <sys/kspinlock.h>

struct shared_lock {
	struct wait_queue  sl_queue;
	volatile int32_t   sl_number;
	volatile u_int32_t sl_flags;
	kspinlock_t        sl_lock;
};

#define SL_FLAG_DRAIN  1

/*
 * Lock types:
 * SL_TYPE_SHARED: A shared lock allows for other (shared) locks to be acquired.
 * SL_TYPE_EXCLUSIVE: An exclusive lock excludes other locks at the same time.
 * Holding an exclusive lock guarantees, there is no other exclusive or shared lock.
 */

#define SL_TYPE_SHARED    1
#define SL_TYPE_EXCLUSIVE 2

/*
 * Acquires a lock. A fair algorithm is used to acquire it.
 * Returns 0 at success, errno otherwise.
 */
int sl_lock(struct shared_lock* lock,int type);

/*
 * Acquires a lock. An unfair algorithm is used to acquire it.
 * Returns 0 at success, errno otherwise.
 */
int sl_lock_greedy(struct shared_lock* lock,int type);

/*
 * Releases a lock.
 */
void sl_unlock(struct shared_lock* lock,int type);

/*
 * Drains a lock object.
 */
void sl_drain(struct shared_lock* lock);

/*
 * This algorithm is equal to:
 * {
 *  sl_lock[_greedy](type);
 *  sl_unlock(type);
 * }
 */
int sl_touch(struct shared_lock* lock,int type);

