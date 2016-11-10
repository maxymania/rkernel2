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

#include <sysmaster/syscalls.h>
#include <sysplatform/console.h>
#include <sys/kterm.h>
#include <sysarch/halt.h>
#include <stdio.h>

static int cous(const char* chr){
	int i=0;
	while(*chr)chr++,i++;
	return i;
}

static void print(const char* chr){
	//console_write_text(chr,cous(chr));
	kern_instance->iop_ops->io_write(kern_instance,chr,cous(chr));
}


/*
 * When called, we should print something, to proove, that we live.
 */
void kern_prove_alive() {
	printf("Test <( %i )>\n",99);
	print("Dear developer, We are alive!\n");
	print("WE ARE ALIVE!\n");
	print("WE ARE ALIVE!\n");
	arch_halt();
}


