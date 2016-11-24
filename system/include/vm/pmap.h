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
#pragma once
#include <sysarch/paddr.h>
#include <vm/vm_types.h>

/*
 * This entire API is being implemented by the hardware-abstraction layer (HAL).
 * This API is heavily inspired by Mach's pmap API.
 */

/*
 * Implemented by the HAL. This data type implements the machine-dependent layer of
 * an virtual address space. However, Whereas the kernel is technically mapped into
 * every virtual address space, the kernel address space is represented by a single,
 * special and unique 'pmap_t'-instance. All changes to the kernel address space, and
 * all operations to the kernel's 'pmap_t'-instance SHALL be visible to all individual
 * virtual address spaces.
 *
 * The kernel resides in the upper half of the available address range, and SHALL BE
 * accessible from the kernel mode (aka. supervisor mode) only (unless there is no usable
 * hardware protection). It SHALL BE shared and identical across all virtual address spaces.
 * The lower half of the available address range SHALL BE accessible from user mode.
 */
struct pmap;
typedef struct pmap *pmap_t;

struct kernslice;

/*
 * Initializes the MMU subsystem of the HAL.
 */
void pmap_init();

/*
 * This function returns the kernels address space instance. Don't 'pmap_destroy()' it.
 */
pmap_t pmap_kernel();

/*
 * Creates a virtual address space instance/Page table.
 */
pmap_t pmap_create();

/*
 * Destroys a virtual address space instance. Don't use it on 'pmap_kernel()'.
 */
void pmap_destroy(pmap_t pmap);

/* 
 * Returns the Kernel-slice, this pagetable allocates it's memory from.
 */
struct kernslice* pmap_kernslice(pmap_t pmap);

/* 
 * Retrieves the bounds of the virtual address space/address range of the current
 * address space.
 *
 * The range is [ vstartp , vendp+1 ) or [ vstartp , vendp ] .
 */
void pmap_get_address_range(pmap_t pmap, vaddr_t *vstartp, vaddr_t *vendp);

/*
 * Maps a given physical memory page (pa) to a given virtual address (va).
 */
int pmap_enter(pmap_t pmap, vaddr_t va, paddr_t pa, vm_prot_t prot, vm_flags_t flags);

/*
 * Unmap a certain range of virtual addresses from this address space.
 */
int pmap_remove(pmap_t pmap, vaddr_t vab, vaddr_t vae);

/*
 * Unmap all pages from this address space.
 */
int pmap_remove_all(pmap_t pmap);

/*
 * Changes the mappings in the range by replacing the memory protection.
 */
int pmap_protect(pmap_t pmap, vaddr_t vab, vaddr_t vae, vm_prot_t prot);

/*
 * This function zeroes out a Memory Page.
 */
void pmap_zero_page(paddr_t pa);

/*
 * ----------------------------------------------
 * | TLB management (local CPU/local CPU-core). |
 * ----------------------------------------------
 */

/*
 * TLB flush all.
 */
void pmap_tlb_flush_all();

/*
 * Flush a range of page-mappings from the TLB.
 */
void pmap_tlb_flush_range(u_intptr_t begin, u_intptr_t end);

/*
 * Flush a single page-mapping from the TLB.
 */
void pmap_tlb_flush_page(u_intptr_t pos);


