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
#include <sys/iopipe.h>
#include <sysplatform/console.h>
#include "ccterm_output.h"

static ssize_t ccterm_read (struct iopipe* iopipe, void* buf, size_t size){
	return 0;
}
static ssize_t ccterm_write (struct iopipe* iopipe, const void* buf, size_t size){
	const u_int8_t* cbuf = buf;
	size_t i,j;
	struct ccterm_buffer* cctb = iopipe->iop_data;
	
	
	for(i=0;i<size;++i){
		u_int16_t cur,next;
		for(j=0;;j++){
			cur = cctb->o_end;
			next = CCTB_CUT(cur+1);
			if(cctb->o_begin != next) break;
			if(j>3) {
				cctb->o_end = cur;
				return i;
			}
			cctb->ops->o_consume(cctb);
		}
		cctb->o_buffer[cur] = cbuf[i];
		cctb->o_end = next;
	}
	if(cctb->o_begin != cctb->o_end) cctb->ops->o_consume(cctb);
	return size;
}

static struct ccterm_buffer ccbuffer;

static const struct iopipe_ops ccterm_ops = {
	.io_read = ccterm_read,
	.io_write = ccterm_write,
};

static struct iopipe ccterm = {
	.iop_data = &ccbuffer,
	.iop_ops  = &ccterm_ops,
};

extern const struct ccterm_ops ccterm_lite_ops;

struct iopipe* ccterm_get(){
	return &ccterm;
}

void ccterm_init(){
	console_init();
	ccbuffer.ops = &ccterm_lite_ops;
}


