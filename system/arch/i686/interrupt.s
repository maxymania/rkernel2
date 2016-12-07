/*
 * 
 * Copyright (c) 2006-2016 Frans Kaashoek, Robert Morris, Russ Cox,
 *                         Massachusetts Institute of Technology
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
# various segment selectors.
# SEG_KCODE 1  // kernel code
# SEG_KDATA 2  // kernel data+stack
# SEG_KCPU  3  // kernel per-cpu data
# SEG_UCODE 4  // user code
# SEG_UDATA 5  // user data+stack
# SEG_TSS   6  // this process's task state

.text
.global __i686_interrupt
.global __i686_isr
__i686_isr:
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal
	# SEG_KDATA<<3
	movw $16, %ax
	movw %ax, %ds
	movw %ax, %es
	# SEG_KCPU<<3
	movw $24, %ax
	movw %ax, %fs
	movw %ax, %gs
	
	pushl %esp
	call __i686_interrupt
	addl $4, %esp
	
	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	addl $8, %esp  # trapno and errcode
	iret

