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
#include <sysarch/paddr.h>
#include <vm/vm_types.h>
#include <sys/kernslice.h>
#include <sys/kspinlock.h>

/*
 * Management of resident pages
 *
 * By default, a resident page of physical memory is matched by the following predicates:
 *  - The page is represented by it's physical memory address ('paddr_t')
 *  - The page is held by ONLY ONE reference.
 *  - The page is allocated from a "kernslice"'s (KS) bitmap.
 *  - If part of an 'vm_range_t', the 'vm_range_t->rang_slice' holds this kernslice (KS)
 *  - Otherwise this kernslice (KS) is obtained through 'pmap_kernslice(vm_as_t->as_pmap)'
 *  - There are no additional attributes to the page necessary than its 'paddr_t'.
 *
 * If one of those predicates don't match, be it that there is more than one
 * reference to the page, or it is allocated from a different 'kernslice' than
 * that of the 'vm_range_t' or the 'vm_as_t', or additional attributes are needed,
 * a small structure is kept for this page.
 */

struct vm_page {
	paddr_t           pg_phys;   /* Physical address of this page. */
	vm_prot_t         pg_prohib; /* Uses prohibited for this page. */
	struct kernslice* pg_slice;  /* The kslice, this page is belonging to. */
	u_int32_t         pg_refc;   /* Reference count. */
	kspinlock_t       pg_lock;   /* Modification lock. */
};
typedef struct vm_page *vm_page_t;

