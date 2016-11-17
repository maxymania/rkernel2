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

// Routines to let C code use special x86 instructions.

static inline void
lgdt(u_intptr_t p, int size)
{
  volatile u_int16_t pd[3];

  pd[0] = size-1;
  pd[1] = (p      )&0xFFFF;
  pd[2] = (p >> 16)&0xFFFF;

  asm volatile("lgdt (%0)" : : "r" (pd));
}

static inline void
lidt(u_intptr_t p, int size)
{
  volatile u_int16_t pd[3];

  pd[0] = size-1;
  pd[1] = (p      )&0xFFFF;
  pd[2] = (p >> 16)&0xFFFF;

  asm volatile("lidt (%0)" : : "r" (pd));
}

static inline void
loadgs(u_int16_t v)
{
  asm volatile("movw %0, %%gs" : : "r" (v));
}


static inline void
cli(void)
{
  asm volatile("cli");
}

static inline void
sti(void)
{
  asm volatile("sti");
}

static inline u_int32_t
rcr2(void)
{
  uint val;
  asm volatile("movl %%cr2,%0" : "=r" (val));
  return val;
}

static inline void
lcr3(u_int32_t val)
{
  asm volatile("movl %0,%%cr3" : : "r" (val));
}
