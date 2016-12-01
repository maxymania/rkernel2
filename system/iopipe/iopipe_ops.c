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

ssize_t iopipe_read (struct iopipe* iopipe, void* buf, size_t size){
	struct kern_uio     kbu;
	struct iovec iov  = { buf,size };
	kbu.kbu_iovec     = &iov;
	kbu.kbu_iovec_n   = 1;
	kbu.kbu_totalsize = size;
	kbu.kbu_rw        = KBU_READ;
	kbu.kbu_origin    = KBU_AS_SYS;
	return iopipe->iop_ops->io_read(iopipe,&kbu);
}

ssize_t iopipe_write (struct iopipe* iopipe, const void* buf, size_t size){
	struct kern_uio     kbu;
	struct iovec iov  = { (void*)buf,size };
	kbu.kbu_iovec     = &iov;
	kbu.kbu_iovec_n   = 1;
	kbu.kbu_totalsize = size;
	kbu.kbu_rw        = KBU_WRITE;
	kbu.kbu_origin    = KBU_AS_SYS;
	return iopipe->iop_ops->io_write(iopipe,&kbu);
}

ssize_t iopipe_read_v (struct iopipe* iopipe, struct kern_uio* kbu){
	return iopipe->iop_ops->io_read(iopipe,kbu);
}
ssize_t iopipe_write_v (struct iopipe* iopipe, struct kern_uio* kbu){
	return iopipe->iop_ops->io_write(iopipe,kbu);
}

