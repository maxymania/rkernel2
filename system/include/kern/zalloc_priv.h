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
#include <kern/zalloc.h>
#include <sys/kspinlock.h>
/*
 * A zone is a collection of fixed size memory buffers, that can be allocated
 * efficiently. All buffers have the same size, as the same type is assumed.
 * For every type of data, a zone should be created.
 *
 * This code is largely inspired by the MACH operating system by the CMU.
 */

struct zone {
	size_t       zn_bufsize;  /* The buffer size of elements. */
	void*        zn_freelist; /* A 'Linked Stack' of free objects. */
	const char*  zn_name;
	u_int32_t    zn_count;    /* Number of free elements. */
	unsigned int zn_memtype;
	kspinlock_t  zn_lock;
};

