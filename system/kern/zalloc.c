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
#include <kern/zalloc_priv.h>
#include <libkern/panic.h>
#include <vm/vm_top.h>
#include <stdio.h>

static struct zone s_zone_zone;
typedef void* Pointer;
static zone_t zone_zone = 0;

/* This code is largely inspired by the MACH operating system by the CMU. */

/* This is the size of a cache line (in x86). */
#define BUF_LINE  128

static u_int8_t szz_buf[1<<16] __attribute__ ((aligned (BUF_LINE)));

static void _zcram(zone_t zone, void* mem, size_t size);
static void _zrefill(zone_t zone, u_int32_t min, u_int32_t num);

static size_t calc_bufsize(size_t size) {
	size_t num = 0;
	size_t mul = 1;
	size += sizeof(Pointer);
	while(num<size){
		if(num<BUF_LINE){
			mul<<=1;
			num = mul;
		}else{
			num += mul;
		}
	}
	return num;
}

static Pointer remove_top(zone_t zone){
	Pointer top = zone->zn_freelist;
	if(top){
		/* Set the zn_freelist-pointer to the next element. */
		zone->zn_freelist = *((Pointer*)top);
		/* Stash a reference to the zone in the 'next'-field. */
		*((Pointer*)top) = (Pointer) zone;
		/* use the space behind the 'next'-field. */
		top += sizeof(Pointer);
		zone->zn_count--;
	}
	return top;
}

void zone_bootstrap(){
	s_zone_zone.zn_bufsize = calc_bufsize(sizeof(struct zone));
	s_zone_zone.zn_freelist = 0;
	s_zone_zone.zn_name = "zone";
	s_zone_zone.zn_count = 0;
	kernlock_init(&(s_zone_zone.zn_lock));
	_zcram(&s_zone_zone,szz_buf,sizeof(szz_buf));
	zone_zone = &s_zone_zone;
}

zone_t zinit(size_t size, unsigned int memtype, const char* name){
	zone_t z;
	if(!zone_zone) panic("zinit: no zone_zone");
	z = zalloc(zone_zone);
	if(!z) panic("zinit");
	z->zn_bufsize = calc_bufsize(size);
	z->zn_freelist = 0;
	z->zn_memtype = memtype;
	if(name)
		z->zn_name = name;
	else
		z->zn_name = "(null)";
	kernlock_init(&(z->zn_lock));
	return z;
}


void* zalloc(zone_t zone){
	Pointer ret;
	if(!zone) panic("zalloc: null zone");
	
	kernlock_lock(&(zone->zn_lock));
	if((zone->zn_memtype) & ZONE_AUTO_REFILL){
		_zrefill(zone,32,32);
	}
	
	ret = remove_top(zone);
	kernlock_unlock(&(zone->zn_lock));
	return ret;
}

void   zfree(void* object){
	if(!object) return;
	object -= sizeof(Pointer);
	zone_t zone = (zone_t) (*((Pointer*)object));
	
	kernlock_lock(&(zone->zn_lock));
		/* Insert the element in the '->zn_freelist' */
		*((Pointer*)object) = zone->zn_freelist;
		zone->zn_freelist = object;
		zone->zn_count++;
	kernlock_unlock(&(zone->zn_lock));
}

static void _zcram(zone_t zone, void* mem, size_t size){
	size_t bufsize = zone->zn_bufsize;
	if(!mem) panic("zcram: memory at zero");
	for( ; size >= bufsize ; size -= bufsize , mem += bufsize){
		/* Insert the chunk into the '->zn_freelist' */
		*((Pointer*)mem) = zone->zn_freelist;
		zone->zn_freelist = mem;
		zone->zn_count++;
	}
}

void   zcram(zone_t zone, void* mem, size_t size){
	kernlock_lock(&(zone->zn_lock));
		_zcram(zone,mem,size);
	kernlock_unlock(&(zone->zn_lock));
}

u_int32_t zcount(zone_t zone){
	return zone->zn_count;
}

size_t zbufsize(zone_t zone){
	return zone->zn_bufsize;
}

static void _zrefill(zone_t zone, u_int32_t min, u_int32_t num){
	vaddr_t begin,size;
	if( zone->zn_count < min ){
		size = (vaddr_t)(zone->zn_bufsize) * num;
		if((zone->zn_memtype) & ZONE_AR_CRITICAL){
			printf("%s : vm_alloc_critical\n",zone->zn_name);
			vm_refill();
			if(!vm_alloc_critical(&begin,&size)) return;
		}else{
			printf("%s : vm_kalloc_ll\n",zone->zn_name);
			if(!vm_kalloc_ll(&begin,&size)) return;
		}
		_zcram(zone,(void*)begin,(size_t)size);
	}
}

void zrefill(zone_t zone, u_int32_t min, u_int32_t num){
	kernlock_lock(&(zone->zn_lock));
		_zrefill(zone,min,num);
	kernlock_unlock(&(zone->zn_lock));
}

