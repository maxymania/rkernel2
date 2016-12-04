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
#pragma once
#include <machine/types.h>

/*
 * Layout of the trap frame built on the stack by the hardware and by intvec.s
 * interrupt.s, and passed to __i686_interrupt().
 */
struct trapframe {
  // registers as pushed by pusha
  u_int32_t edi;
  u_int32_t esi;
  u_int32_t ebp;
  u_int32_t oesp;      // useless & ignored
  u_int32_t ebx;
  u_int32_t edx;
  u_int32_t ecx;
  u_int32_t eax;

  // rest of trap frame
  u_int16_t gs;
  u_int16_t padding1;
  u_int16_t fs;
  u_int16_t padding2;
  u_int16_t es;
  u_int16_t padding3;
  u_int16_t ds;
  u_int16_t padding4;
  u_int32_t trapno;

  // below here defined by x86 hardware
  u_int32_t err;
  u_int32_t eip;
  u_int16_t cs;
  u_int16_t padding5;
  u_int32_t eflags;

  // below here only when crossing rings, such as from user to kernel
  u_int32_t esp;
  u_int16_t ss;
  u_int16_t padding6;
};

