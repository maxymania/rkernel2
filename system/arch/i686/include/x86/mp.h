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
#include <sysarch/paddr.h>
#include <machine/types.h>


// See MultiProcessor Specification Version 1.[14]

struct mp {             // floating pointer
  u_int8_t signature[4];           // "_MP_"
  u_int32_t physaddr;               // phys addr of MP config table
  u_int8_t length;                 // 1
  u_int8_t specrev;                // [14]
  u_int8_t checksum;               // all bytes must add up to 0
  u_int8_t type;                   // MP system config type
  u_int8_t imcrp;
  u_int8_t reserved[3];
};

struct mpconf {         // configuration table header
  u_int8_t signature[4];           // "PCMP"
  u_int16_t length;                // total table length
  u_int8_t version;                // [14]
  u_int8_t checksum;               // all bytes must add up to 0
  u_int8_t product[20];            // product id
  u_int32_t *oemtable;               // OEM table pointer
  u_int16_t oemlength;             // OEM table length
  u_int16_t entry;                 // entry count
  u_int32_t *lapicaddr;              // address of local APIC
  u_int16_t xlength;               // extended table length
  u_int8_t xchecksum;              // extended table checksum
  u_int8_t reserved;
};

struct mpproc {         // processor table entry
  u_int8_t type;                   // entry type (0)
  u_int8_t apicid;                 // local APIC id
  u_int8_t version;                // local APIC verison
  u_int8_t flags;                  // CPU flags
    #define MPBOOT 0x02           // This proc is the bootstrap processor.
  u_int8_t signature[4];           // CPU signature
  u_int32_t feature;                 // feature flags from CPUID instruction
  u_int8_t reserved[8];
};

struct mpioapic {       // I/O APIC table entry
  u_int8_t type;                   // entry type (2)
  u_int8_t apicno;                 // I/O APIC id
  u_int8_t version;                // I/O APIC version
  u_int8_t flags;                  // I/O APIC flags
  u_int32_t *addr;                  // I/O APIC address
};

// Table entry types
#define MPPROC    0x00  // One per processor
#define MPBUS     0x01  // One per bus
#define MPIOAPIC  0x02  // One per I/O APIC
#define MPIOINTR  0x03  // One per bus interrupt source
#define MPLINTR   0x04  // One per system interrupt source

//PAGEBREAK!
// Blank page.