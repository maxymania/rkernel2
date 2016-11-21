/*
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
#include <sysarch/hal.h>
#include <sys/cpu.h>
#include <x86/cpu_arch.h>
#include <x86/x86.h>

/*
 * The local CPU structure pointer.
 */
extern struct cpu *cpu_ptr asm("%gs:0");

/*
 * The thread-local storage.
 */
extern u_intptr_t *cpu_tls asm("%gs:4");

struct cpu *kernel_get_current_cpu() {
	return cpu_ptr;
}

void hal_initcpu(struct cpu* cpu){
	struct cpu_arch *cpu_arch = cpu->cpu_arch;
	
	/*
	 * Map "logical" addresses to virtual addresses using identity map.
	 * Cannot share a CODE descriptor for both kernel and user
	 * because it would have to have DPL_USR, but the CPU forbids
	 * an interrupt from CPL=0 to DPL=3.
	 */
	cpu_arch->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
	cpu_arch->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
	cpu_arch->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
	cpu_arch->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
	cpu_arch->gdt[SEG_TSS]   = SEG16(STS_T32A, &cpu_arch->tss, sizeof(cpu_arch->tss)-1, 0);
	cpu_arch->gdt[SEG_TSS].s = 0;
	
	/* Map cpu_local -- that's private per cpu. */
	cpu_arch->gdt[SEG_KCPU] = SEG(STA_W, cpu->cpu_local, sizeof(cpu->cpu_local), 0);
	
	lgdt((u_intptr_t)cpu_arch->gdt,sizeof(cpu_arch->gdt));
	loadgs(SEG_KCPU << 3);
}

void hal_after_thread_switch(){
	struct cpu_arch *cpu_arch = cpu_ptr->cpu_arch;
	cpu_arch->tss.ss0  = SEG_KDATA << 3;
	cpu_arch->tss.esp0 = cpu_tls[1];
	/*
	 * setting IOPL=0 in eflags *and* iomb beyond the tss segment limit
	 * forbids I/O instructions (e.g., inb and outb) from user space
	 */
	cpu_arch->tss.iomb = (ushort) 0xFFFF;
	ltr(SEG_TSS << 3);
}

