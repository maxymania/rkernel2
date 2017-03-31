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
#include <vm/pmap.h>
#include <sys/kspinlock.h>


/*
 * A virtual address space.
 */
struct protection_domain {
	vaddr_t       pd_begin;
	vaddr_t       pd_end;
	pmap_t        pd_pmap;
	u_int32_t     pd_refc;
	kspinlock_t   pd_lock; /* Synchronized ->as_pmap */
};

typedef struct protection_domain* pd_t;

void pd_init();

pd_t pd_get_kernel();
int pd_enter(pd_t dom, vaddr_t va, paddr_t pa, vm_prot_t prot, vm_flags_t flags);
int pd_remove(pd_t dom, vaddr_t vab, vaddr_t vae);

