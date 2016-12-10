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
#include <x86/x86.h>
#include <x86/traps.h>

// I/O Addresses of the two programmable interrupt controllers
#define IO_PIC1         0x20    // Master (IRQs 0-7)
#define IO_PIC2         0xA0    // Slave (IRQs 8-15)

#define IRQ_SLAVE       2       // IRQ at which slave connects to master

#define PIC_EOI	        0x20    /* End-of-interrupt command code */


/*
 * Intel 8253/8254/82C54 Programmable Interval Timer (PIT).
 * Only used on uniprocessors;
 * SMP machines use the local APIC timer.
 */

#define IO_TIMER1       0x040           // 8253 Timer #1

/*
 * Frequency of all three count-down timers;
 * (TIMER_FREQ/freq) is the appropriate count
 * to generate a frequency of freq Hz.
 */

#define TIMER_FREQ      1193182
#define TIMER_DIV(x)    ((TIMER_FREQ+(x)/2)/(x))

#define TIMER_MODE      (IO_TIMER1 + 3) // timer mode port
#define TIMER_SEL0      0x00    // select counter 0
#define TIMER_RATEGEN   0x04    // mode 2, rate generator
#define TIMER_16BIT     0x30    // r/w counter 16 bits, LSB first


// Current IRQ mask.
// Initial IRQ mask has interrupt 2 enabled (for slave 8259A).
static u_int16_t irqmask = 0xFFFF & ~(1<<IRQ_SLAVE);

static void
picsetmask(u_int16_t mask)
{
  irqmask = mask;
  outb(IO_PIC1+1, mask);
  outb(IO_PIC2+1, mask >> 8);
}

static void
picenable(int irq)
{
	picsetmask(irqmask & ~(1<<irq));
}


// Initialize the 8259A interrupt controllers.
void __i686_picinit()
{
	// mask all interrupts
	outb(IO_PIC1+1, 0xFF);
	outb(IO_PIC2+1, 0xFF);

	// Set up master (8259A-1)

	// ICW1:  0001g0hi
	//    g:  0 = edge triggering, 1 = level triggering
	//    h:  0 = cascaded PICs, 1 = master only
	//    i:  0 = no ICW4, 1 = ICW4 required
	outb(IO_PIC1, 0x11);

	// ICW2:  Vector offset
	outb(IO_PIC1+1, T_IRQ0);

	// ICW3:  (master PIC) bit mask of IR lines connected to slaves
	//        (slave PIC) 3-bit # of slave's connection to master
	outb(IO_PIC1+1, 1<<IRQ_SLAVE);

	// ICW4:  000nbmap
	//    n:  1 = special fully nested mode
	//    b:  1 = buffered mode
	//    m:  0 = slave PIC, 1 = master PIC
	//      (ignored when b is 0, as the master/slave role
	//      can be hardwired).
	//    a:  1 = Automatic EOI mode
	//    p:  0 = MCS-80/85 mode, 1 = intel x86 mode
	outb(IO_PIC1+1, 0x3);

	// Set up slave (8259A-2)
	outb(IO_PIC2, 0x11);                  // ICW1
	outb(IO_PIC2+1, T_IRQ0 + 8);      // ICW2
	outb(IO_PIC2+1, IRQ_SLAVE);           // ICW3
	// NB Automatic EOI mode doesn't tend to work on the slave.
	// Linux source code says it's "to be investigated".
	outb(IO_PIC2+1, 0x3);                 // ICW4

	// OCW3:  0ef01prs
	//   ef:  0x = NOP, 10 = clear specific mask, 11 = set specific mask
	//    p:  0 = no polling, 1 = polling mode
	//   rs:  0x = NOP, 10 = read IRR, 11 = read ISR
	outb(IO_PIC1, 0x68);             // clear specific mask
	outb(IO_PIC1, 0x0a);             // read IRR by default

	outb(IO_PIC2, 0x68);             // OCW3
	outb(IO_PIC2, 0x0a);             // OCW3

	if(irqmask != 0xFFFF)
		picsetmask(irqmask);
}

void __i686_piceoi(int irq) {
	if(irq >= 8)
		outb(IO_PIC2,PIC_EOI);
	outb(IO_PIC1,PIC_EOI);
}

void __i686_timerinit()
{
	int time;
	/* Interrupt 50 times/sec. */
	time = TIMER_DIV(50);
	outb(TIMER_MODE, TIMER_SEL0 | TIMER_RATEGEN | TIMER_16BIT);
	outb(IO_TIMER1, time % 256);
	outb(IO_TIMER1, time / 256);
	picenable(IRQ_TIMER);
}

