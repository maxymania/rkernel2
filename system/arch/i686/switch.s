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

.text
.global __i686_switch
.global __i686_initthread
__i686_switch:
	# Save old callee-save registers
	pushl %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	
	# THREAD_LOCAL_CONTEXT := %esp
	movl %gs:4, %eax
	movl %esp, 4(%eax)
	
	# %esp := CPU_LOCAL_TLS
	movl %gs:8, %esp
	
	call sched_preempt
	
	# %esp := THREAD_LOCAL_CONTEXT
	movl %gs:4, %eax
	movl 4(%eax), %esp
	
	# Load new callee-save registers
	popl %edi
	popl %esi
	popl %ebx
	popl %ebp
	ret
#


__i686_initthread:
	movl %esp, %eax      # save old stack pointer.
	movl 4(%eax),%esp    # Set new stack pointer to 1st argument.
	
	movl 12(%eax),%edx   # Push 3rd argument onto the stack.
	pushl %edx
	movl 8(%eax),%edx    # Push 2nd argument onto the stack.
	pushl %edx
	
	# emulate 'call __i686_switch' so we can return from __i686_switch.
	call 1
	
	popl %edx
	sti
	call *%edx # Call 2nd(3rd)
2:
	hlt
	jmp 2
	
1:
	pushl $0
	pushl $0
	pushl $0
	pushl $0
	
	# THREAD_LOCAL_CONTEXT := %esp
	movl 16(%eax), %edx
	movl %esp, (%edx)
	
	# Restore the old stack pointer.
	movl %eax, %esp
	
	#return
	ret
#


