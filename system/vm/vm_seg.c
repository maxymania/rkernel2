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
#include <vm/vm_as.h>
#include <vm/vm_seg.h>
#include <kern/zalloc.h>
#include <string.h>

static zone_t vm_seg_zone;  /* Zone for user vm_seg structures. */
static zone_t vm_kseg_zone; /* Zone for kernel vm_seg structures. */

void vm_seg_init(){
	vm_seg_zone = zinit(sizeof(struct vm_seg),0,"user-mode segment zone");
	vm_kseg_zone = zinit(sizeof(struct vm_seg),0,"kernel-mode segment zone");
}

vm_seg_t vm_seg_alloc(int kernel){
	vm_seg_t seg = zalloc(kernel ? vm_kseg_zone : vm_seg_zone);
	if(!seg) return 0;
	memset((void*)seg,0,sizeof(vm_seg_t));
	kernlock_init(&(seg->seg_lock));
	return seg;
}

void vm_seg_initobj(vm_seg_t seg){
	seg->_bt_node.V = seg;
	seg->_bt_node.K = seg->seg_begin;
}



