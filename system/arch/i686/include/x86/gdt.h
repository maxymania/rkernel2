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
#include <sysstd/uint.h>


// various segment selectors.
#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_KCPU  3  // kernel per-cpu data
#define SEG_UCODE 4  // user code
#define SEG_UDATA 5  // user data+stack
#define SEG_TSS   6  // this process's task state

// cpu->gdt[NSEGS] holds the above segments.
#define NSEGS 7


// Segment Descriptor / GDT element
struct segdesc {
  uint lim_15_0 : 16;  // Low bits of segment limit
  uint base_15_0 : 16; // Low bits of segment base address
  uint base_23_16 : 8; // Middle bits of segment base address
  uint type : 4;       // Segment type (see STS_ constants)
  uint s : 1;          // 0 = system, 1 = application
  uint dpl : 2;        // Descriptor Privilege Level
  uint p : 1;          // Present
  uint lim_19_16 : 4;  // High bits of segment limit
  uint avl : 1;        // Unused (available for software use)
  uint rsv1 : 1;       // Reserved
  uint db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
  uint g : 1;          // Granularity: limit scaled by 4K when set
  uint base_31_24 : 8; // High bits of segment base address
};

// Normal segment
#define SEG(type, base, lim, dpl) (struct segdesc)    \
{ ((lim) >> 12) & 0xffff, (uint)(base) & 0xffff,      \
  ((uint)(base) >> 16) & 0xff, type, 1, dpl, 1,       \
  (uint)(lim) >> 28, 0, 0, 1, 1, (uint)(base) >> 24 }
#define SEG16(type, base, lim, dpl) (struct segdesc)  \
{ (lim) & 0xffff, (uint)(base) & 0xffff,              \
  ((uint)(base) >> 16) & 0xff, type, 1, dpl, 1,       \
  (uint)(lim) >> 16, 0, 0, 1, 0, (uint)(base) >> 24 }


#define DPL_USER    0x3     /* User DPL */

/* Application segment type bits */
#define STA_X       0x8     /* Executable segment */
#define STA_E       0x4     /* Expand down (non-executable segments) */
#define STA_C       0x4     /* Conforming code segment (executable only) */
#define STA_W       0x2     /* Writeable (non-executable segments) */
#define STA_R       0x2     /* Readable (executable segments) */
#define STA_A       0x1     /* Accessed */

// System segment type bits
#define STS_T16A    0x1     /* Available 16-bit TSS */
#define STS_LDT     0x2     /* Local Descriptor Table */
#define STS_T16B    0x3     /* Busy 16-bit TSS */
#define STS_CG16    0x4     /* 16-bit Call Gate */
#define STS_TG      0x5     /* Task Gate / Coum Transmitions */
#define STS_IG16    0x6     /* 16-bit Interrupt Gate */
#define STS_TG16    0x7     /* 16-bit Trap Gate */
#define STS_T32A    0x9     /* Available 32-bit TSS */
#define STS_T32B    0xB     /* Busy 32-bit TSS */
#define STS_CG32    0xC     /* 32-bit Call Gate */
#define STS_IG32    0xE     /* 32-bit Interrupt Gate */
#define STS_TG32    0xF     /* 32-bit Trap Gate */

// Task state segment format
struct taskstate {
  u_int32_t link;         // Old ts selector
  u_int32_t esp0;         // Stack pointers and segment selectors
  u_int16_t ss0;        //   after an increase in privilege level
  u_int16_t padding1;
  u_int32_t *esp1;
  u_int16_t ss1;
  u_int16_t padding2;
  u_int32_t *esp2;
  u_int16_t ss2;
  u_int16_t padding3;
  void *cr3;         // Page directory base
  u_int32_t *eip;         // Saved state from last task switch
  u_int32_t eflags;
  u_int32_t eax;          // More saved state (registers)
  u_int32_t ecx;
  u_int32_t edx;
  u_int32_t ebx;
  u_int32_t *esp;
  u_int32_t *ebp;
  u_int32_t esi;
  u_int32_t edi;
  u_int16_t es;         // Even more saved state (segment selectors)
  u_int16_t padding4;
  u_int16_t cs;
  u_int16_t padding5;
  u_int16_t ss;
  u_int16_t padding6;
  u_int16_t ds;
  u_int16_t padding7;
  u_int16_t fs;
  u_int16_t padding8;
  u_int16_t gs;
  u_int16_t padding9;
  u_int16_t ldt;
  u_int16_t padding10;
  u_int16_t t;          // Trap on task switch
  u_int16_t iomb;       // I/O map base address
};

// Gate descriptors for interrupts and traps
struct gatedesc {
  uint off_15_0 : 16;   // low 16 bits of offset in segment
  uint cs : 16;         // code segment selector
  uint args : 5;        // # args, 0 for interrupt/trap gates
  uint rsv1 : 3;        // reserved(should be zero I guess)
  uint type : 4;        // type(STS_{TG,IG32,TG32})
  uint s : 1;           // must be 0 (system)
  uint dpl : 2;         // descriptor(meaning new) privilege level
  uint p : 1;           // Present
  uint off_31_16 : 16;  // high bits of offset in segment
};

// Set up a normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears FL_IF, trap gate leaves FL_IF alone
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction.
#define SETGATE(gate, istrap, sel, off, d)                \
{                                                         \
  (gate).off_15_0 = (uint)(off) & 0xffff;                \
  (gate).cs = (sel);                                      \
  (gate).args = 0;                                        \
  (gate).rsv1 = 0;                                        \
  (gate).type = (istrap) ? STS_TG32 : STS_IG32;           \
  (gate).s = 0;                                           \
  (gate).dpl = (d);                                       \
  (gate).p = 1;                                           \
  (gate).off_31_16 = (uint)(off) >> 16;                  \
}

