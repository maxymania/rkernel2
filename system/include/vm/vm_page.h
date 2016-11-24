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
 * By default, a physical memory page is held, only by one reference, and, also
 * only by an address-space, that belongs to the same 'kernslice', the page belongs
 * to.
 *
 * If there is more than one reference to a page, or it is being referenced by an
 * address-space from a different 'kernslice', or additional attributes are needed,
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


