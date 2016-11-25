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
#include <machine/types.h>
#include <machine/stdtypes.h>

/*
 * A zone is a collection of fixed size memory buffers, that can be allocated
 * efficiently. All buffers have the same size, as the same type is assumed.
 * For every type of data, a zone should be created.
 *
 * This code is largely inspired by the MACH operating system by the CMU.
 */

typedef struct zone *zone_t;

void zone_bootstrap();

/* Initialize a zone. */
zone_t zinit(size_t size, unsigned int memtype, const char* name);

/* Allocate an object from a zone. */
void*  zalloc(zone_t zone);

/* Free an object, that was allocated from a zone. */
void   zfree(void* object);

/* Cram new memory into the zone. */
void   zcram(zone_t zone, void* mem, size_t size);

