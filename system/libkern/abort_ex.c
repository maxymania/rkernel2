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
#include <libkern/abort.h>
#include <libkern/panic.h>
#include <libkern/assert.h>
#include <libkern/iopipe.h>
#include <sys/kterm.h>

/*
 * Prints a panic message, and then it shuts down the system.
 */
void panic(const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	iopipe_printf(kterm_instance, "panic: ");
	iopipe_vioprintf(kterm_instance,fmt,ap);
	iopipe_printf(kterm_instance, "\n");
	va_end(ap);
	abort();
}

void __kernel_assert(int i, const char* fmt){
	if(i)return;
	iopipe_printf(kterm_instance, "assertion failed: %s\n",fmt);
	abort();
}

