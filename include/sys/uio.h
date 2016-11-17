/*
 * Copyright (c) 2016 Simon Schmidt
 *
 * All or some portions of this file are derived from specifications of UNIX
 * error codes as seen in other UNIX-systems and UNIX-like systems such as Linux,
 * BSD, research-UNIX version 7, and System V. For the fundamental source, that
 * inspired the structure of this file, I credit:
 *  - Bell Labs (AT&T/Unix System Laboratories, Inc.), the team that worked on
 *                             UNIX especially Dennis Ritchie and Ken Thompson.
 *  - CSRG at University of California, Berkeley.
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
#include <machine/stdtypes.h>

struct iovec{
	void*   iov_base; /* Begin of the memory Region. */
	size_t  iov_len;  /* Length of the memory Region */
};

#define UIO_SMALLIOV    8       /* 8 on stack, else malloc */
#define UIO_MAXIOV      1024    /* max 1K of iov's */

