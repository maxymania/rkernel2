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
#include <sys/physmem.h>

/*
 * A kernel Slice represent a Set of CPUs and resources, that belong together.
 * It structurally resembles "NUMA domains". A typical use case is to implement
 * NUMA support by dedicating one kernel Slice to every NUMA domain.
 *
 * However kernel Slices can also be used to split up very large SMP systems,
 * or even to split up large NUMA domains in order to improve performance, when
 * appropriate.
 */
struct kernslice{
	u_intptr_t            ks_kernslice_id;       /* The unique ID of this kernel Slice. */
	struct physmem_range* ks_memory_ranges;      /* Table of Physical Memory Ranges. */
	u_intptr_t            ks_num_memory_ranges;  /* The length of the Table of Physical Memory Ranges. */
};

