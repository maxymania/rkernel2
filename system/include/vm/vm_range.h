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
#include <vm/vm_page.h>
/* #include <sys/kspinlock.h> */

#define VM_RANGE_NUM 116

/*
 * Type bits (vm_range_t->rang_pages_tbm) :
 *  0 => vm_page_t
 *  1 => paddr_t
 */
union vm_range_page {
	vm_page_t page_obj;
	paddr_t   page_addr;
};

struct vm_range {
	union vm_range_page rang_pages[VM_RANGE_NUM];
	u_int32_t           rang_pages_tbm[4]; /* Type-bitmap for rang_pages. */
	u_int32_t           rang_refc;
	struct vm_range*    rang_next;
	kspinlock_t         rang_lock;
};

typedef struct vm_range *vm_range_t;

