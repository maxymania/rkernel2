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

struct ccterm_buffer;


struct ccterm_ops {
	void (*o_consume) (struct ccterm_buffer* buf);
};

struct ccterm_buffer{
	u_int8_t  o_buffer[1024];
	u_int16_t o_begin,o_end;
	const struct ccterm_ops* ops;
};

#define CCTB_IDX(buf,i) (buf)->o_buffer[ ((buf)->o_begin+i)&1023 ]
#define CCTB_CUT(i) ((i)&1023)
#define CCTB_EOB(buf,i) (((buf)->o_begin+i)&1023)==((buf)->o_end)

