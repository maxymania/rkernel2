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
#include <vm/vm_types.h>

/*
 * This function initializes the kernel virtual memory system.
 */
void vm_init();

/*
 * Allocates a chunk of kernel-memory.
 */
int vm_kalloc_ll(vaddr_t *addr /* [out] */,vaddr_t *size /* [in/out]*/);

/*
 * Refills the critical kernel-vm object zones, if necessary. Do this after vm_alloc_critical().
 */
void vm_refill();

/*
 * Allocates a critical chunk of memory. Used for the vm_seg_t, vm_mem_t and vm_range_t -zones.
 */
int vm_alloc_critical(vaddr_t *addr /* [out] */,vaddr_t *size /* [in/out]*/);