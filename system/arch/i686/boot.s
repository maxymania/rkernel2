/*
 * Copyright (c) 2016 Simon Schmidt
 * Based on the work from: http://wiki.osdev.org/Meaty_Skeleton
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
# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Allocate the initial stack.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.
.section .bss, "aw", @nobits
	.align 4096
.global _i686_kernel_page_dir
.global _i686_kernel_page_table
.global _i686_multiboot_memdata
.global _i686_multiboot_mmap
boot_pagedir:
_i686_kernel_page_dir:
boot_page_directory:
	.skip 4096
_i686_kernel_page_table:
boot_pagetab1:
boot_page_table1:
	.skip 4096
boot_page_table2:
	.skip 4096
boot_page_table3:
	.skip 4096
boot_page_table4:
	.skip 4096

_i686_multiboot_memdata:
	.skip 4
	.skip 8
	.skip 8

# Further page tables may be required if the kernel grows beyond 3 MiB.

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	# Multiboot Spec: 3.2 Machine state
	#  %ebx contains the Multiboot-Bootloader INFO.
	# Multiboot Spec: 3.3 Boot information format
	#  4       | mem_lower         |    (present if flags[0] is set)
	#  8       | mem_upper         |    (present if flags[0] is set)
	#  ...
	#  44      | mmap_length       |    (present if flags[6] is set)
	#  48      | mmap_addr         |    (present if flags[6] is set)
	#
	# *(_i686_multiboot_memdata+0) = *(%ebx+0)
	# *(_i686_multiboot_memdata+4) = *(%ebx+4)
	# *(_i686_multiboot_memdata+8) = *(%ebx+8)
	# *(_i686_multiboot_memdata+12) = *(%ebx+44)
	# *(_i686_multiboot_memdata+16) = *(%ebx+48)
	
	# *(_i686_multiboot_memdata+0) = *(%ebx+0)
	movl (%ebx), %edi
	movl $(_i686_multiboot_memdata - 0xC0000000), %esi
	movl %edi, (%esi)
	
	# *(_i686_multiboot_memdata+4) = *(%ebx+4)
	#        *(%esi)               =  *(%edi)
	movl $4, %edi
	addl %ebx, %edi
	movl (%edi), %edi
	movl $(_i686_multiboot_memdata + 4 - 0xC0000000), %esi
	movl %edi, (%esi)
	
	# *(_i686_multiboot_memdata+8) = *(%ebx+8)
	#        *(%esi)               =  *(%edi)
	movl $8, %edi
	addl %ebx, %edi
	movl (%edi), %edi
	movl $(_i686_multiboot_memdata + 8 - 0xC0000000), %esi
	movl %edi, (%esi)
	
	# *(_i686_multiboot_memdata+12) = *(%ebx+44)
	#        *(%esi)                =  *(%edi)
	movl $44, %edi
	addl %ebx, %edi
	movl (%edi), %edi
	movl $(_i686_multiboot_memdata + 12 - 0xC0000000), %esi
	movl %edi, (%esi)
	
	# *(_i686_multiboot_memdata+16) = *(%ebx+48)
	#        *(%esi)                =  *(%edi)
	movl $48, %edi
	addl %ebx, %edi
	movl (%edi), %edi
	movl $(_i686_multiboot_memdata + 16 - 0xC0000000), %esi
	movl %edi, (%esi)
	
	
	# movl %ebx, %edi
	# add $44,$edi
	# movl (%edi),%edi
	# Physical address of boot_pagetab1.
	# TODO: I recall seeing some assembly that used a macro to do the
	#       conversions to and from physical. Maybe this should be done in this
	#       code as well?
	movl $(boot_pagetab1 - 0xC0000000), %edi
	# First address to map is address 0.
	# TODO: Start at the first kernel page instead. Alternatively map the first
	#       1 MiB as it can be generally useful, and there's no need to
	#       specially map the VGA buffer.
	movl $0, %esi
	# Map 1023 pages. The 1024th will be the VGA text buffer.
	movl $1023, %ecx

1:
	# Only map the kernel.
	cmpl $(_kernel_start - 0xC0000000), %esi
	jl 2f
	cmpl $(_kernel_end - 0xC0000000), %esi
	jge 3f

	# Map physical address as "present, writable". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	# Size of page is 4096 bytes.
	addl $4096, %esi
	# Size of entries in boot_pagetab1 is 4 bytes.
	addl $4, %edi
	# Loop to the next entry if we haven't finished.
	loop 1b

3:
	# Map VGA video memory to 0xC0BFF000 as "present, writable".
	movl $(0x000B8000 | 0x003), boot_page_table3 - 0xC0000000 + 1023 * 4

	# The page table is used at both page directory entry 0 (virtually from 0x0
	# to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	# 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	# higher half). The kernel is identity mapped because enabling paging does
	# not change the next instruction, which continues to be physical. The CPU
	# would instead page fault if there was no identity mapping.

	# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4
	
	movl $(boot_page_table2 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 4 + 0
	movl $(boot_page_table2 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 4 + 768 * 4
	
	movl $(boot_page_table3 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 8 + 0
	movl $(boot_page_table3 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 8 + 768 * 4
	
	movl $(boot_page_table4 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 12 + 0
	movl $(boot_page_table4 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 12 + 768 * 4

	# Set cr3 to the address of the boot_page_directory.
	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump. 
	lea 4f, %ecx
	jmp *%ecx

4:
	# At this point, paging is fully set up and enabled.

	# Unmap the identity mapping as it is now unnecessary. 
	movl $0, boot_page_directory + 0
	movl $0, boot_page_directory + 4
	movl $0, boot_page_directory + 8
	movl $0, boot_page_directory + 12

	# Reload crc3 to force a TLB flush so the changes to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

	# Set up the stack.
	mov $stack_top, %esp
	
	# Call the global constructors.
	call _init

	# Sanitize General Purpose Registers
	xor %ecx, %ecx
	movl %ecx, %gs
	movl %ecx, %fs

	# Transfer control to the main kernel.
	#  call kernel_main
	call _i686_boot_main

	# Infinite loop if the system has nothing more to do.
	cli
1:	hlt
	jmp 1b

