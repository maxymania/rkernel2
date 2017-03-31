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
#include <vm/page_fault.h>
#include <sys/kspinlock.h>
#include <utils/list.h>

enum {
	RMO_UNDEFINED,
	RMO_DATASPACE,
	RMO_REGION_MAPPER, /* Another region mapper. */
};

struct region_mapper {
	list_node_s   rm_childs;
	vaddr_t       rm_size;
	u_int32_t     rm_refc;
	kspinlock_t   rm_lock;
};

struct region_mapper_object {
	vaddr_t       rmo_offset;
	vaddr_t       rmo_size;
	list_node_s   rmo_member;
	void*         rmo_object;
	unsigned int
		      rmo_type  :4,
		      rmo_static:1, /* Don't zfree() ! */
	:0;
};

typedef struct region_mapper* rm_t;
typedef struct region_mapper_object* rmo_t;

void rm_init();

rm_t rm_get_kernel();

int rm_insert(rm_t rm, rmo_t rmo);

void* rm_ll_lookup(rm_t rm, struct page_fault* fault, int *type);

