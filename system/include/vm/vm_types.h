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

typedef u_int16_t  vm_prot_t;
typedef u_int16_t  vm_flags_t;

/*
 * In the terminology of Virtual Memory, we talk about "virtual" and "physical"
 * addresses. The virtual addresses are (logically) in the format of a pointer.
 *
 * Address-ranges are expressed by specifying the first and the last valid pointer
 * in the address range. So, in order to iterate through a range, the following
 * code is needen:     for(i = begin; i <= end; i+=STEP);
 */
typedef u_intptr_t vaddr_t;

#define VM_PROT_READ     4
#define VM_PROT_WRITE    2
#define VM_PROT_EXECUTE  1

#define PMAP_NOCACHE     1


