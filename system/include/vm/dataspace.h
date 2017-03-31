/*
 * 
 * Copyright (c) 2017 Simon Schmidt
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
#include <vm/vm_types.h>
#include <sysarch/paddr.h>
#include <sys/kspinlock.h>

enum{
	DS_ALLOC_STATIC,
	DS_ALLOC_ZALLOC,
};

struct dataspace {
	kspinlock_t   ds_lock;
	unsigned int
		      ds_n_pages : 27, /* Up to 128M pages. */
		      ds_alloc   :  4, /* Allocation type. */
	:0;
	u_int32_t     ds_refc;
	vaddr_t       ds_size;
	paddr_t       ds_pages[0];
};

typedef struct dataspace* dataspace_t;

void ds_construct(dataspace_t ds,vaddr_t bufsize);


